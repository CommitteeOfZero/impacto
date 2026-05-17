#!/usr/bin/env python3
"""
Convert hex addresses in JSON to symbols using addr2line.
Processes JSON objects with a "name" field containing hex addresses
and replaces them with their corresponding symbols.
"""

import json
import subprocess
import sys
import os
from pathlib import Path
import re
from concurrent.futures import ThreadPoolExecutor, as_completed
import threading

# ---------------------------------------------------------------------------
# Global verbosity flag – set to False via --quiet / -q
# ---------------------------------------------------------------------------
VERBOSE = True

def log(*args, **kwargs):
    """Print to stderr only when VERBOSE is True."""
    if VERBOSE:
        print(*args, file=sys.stderr, **kwargs)


def _get_addr2line_path():
    """Return the addr2line binary path, or None if not found."""
    devkitpro = os.environ.get('DEVKITPRO', '/opt/devkitpro')
    path = os.path.join(devkitpro, 'devkitA64/bin/aarch64-none-elf-addr2line')
    return path if os.path.exists(path) else None


class Addr2LineWorker:
    """
    A persistent addr2line process that accepts addresses on stdin and returns
    function names on stdout.  One instance per thread; keeping the process
    alive eliminates the per-address spawn overhead that caused timeouts.

    addr2line with -f prints two lines per query:
        <function name>
        <file>:<line>
    We read both lines and return only the function name.
    """

    def __init__(self, executable, addr2line_path):
        self._proc = subprocess.Popen(
            [addr2line_path, '-f', '-C', '-e', executable],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
            text=True,
            bufsize=1,          # line-buffered
        )

    def lookup(self, address):
        """
        Query the persistent process for one address.
        Returns the demangled function name, or the original address string
        if the lookup fails or returns '??'.
        """
        addr_clean = address[2:] if address.startswith('0x') else address
        try:
            self._proc.stdin.write(addr_clean + '\n')
            self._proc.stdin.flush()
            func_name = self._proc.stdout.readline().strip()  # line 1: function
            self._proc.stdout.readline()                       # line 2: file:line (discard)
        except (BrokenPipeError, OSError):
            return address

        if not func_name or func_name == '??':
            log(f"Debug: addr2line returned '??' for {address}")
            return address
        return func_name

    def close(self):
        try:
            self._proc.stdin.close()
            self._proc.wait(timeout=2)
        except Exception:
            self._proc.kill()


# Thread-local storage: each worker thread keeps its own Addr2LineWorker so
# there is no lock contention on the process stdin/stdout.
_thread_local = threading.local()


def _get_worker(executable, addr2line_path):
    """Return this thread's Addr2LineWorker, creating it on first use."""
    worker = getattr(_thread_local, 'worker', None)
    if worker is None:
        worker = Addr2LineWorker(executable, addr2line_path)
        _thread_local.worker = worker
    return worker


def get_symbol_for_address(address, executable):
    """
    Convert a hex address to a symbol using a persistent addr2line worker.

    Args:
        address:    Hex address string (e.g., "0x4f5500")
        executable: Path to the executable binary

    Returns:
        Demangled function name, or the original address string on failure.
    """
    addr2line_path = _get_addr2line_path()
    if addr2line_path is None:
        log(f"Warning: addr2line not found")
        return address
    worker = _get_worker(executable, addr2line_path)
    return worker.lookup(address)


def get_elf_load_address(executable):
    """
    Return the virtual address of the first PT_LOAD segment in the ELF.

    addr2line expects addresses in the ELF's own virtual address space.
    After subtracting the *runtime* base you get a file-relative offset;
    you must then add the ELF's own link-time load VAddr back so addr2line
    can match the address against DWARF debug info.

    Returns 0 if readelf fails or the output cannot be parsed.
    """
    try:
        result = subprocess.run(
            ['readelf', '-l', '--wide', executable],
            capture_output=True, text=True, timeout=5,
        )
        for line in result.stdout.splitlines():
            # Lines look like:
            #   LOAD  0x000000 0x0000000000000000 0x0000000000000000 ...
            parts = line.split()
            if parts and parts[0] == 'LOAD' and len(parts) >= 3:
                vaddr = int(parts[2], 16)
                log(f"ELF first LOAD segment VAddr: 0x{vaddr:x}")
                return vaddr
    except Exception as e:
        log(f"Warning: could not read ELF load address: {e}")
    return 0


def is_hex_address(value):
    """Return True when *value* is a 0x-prefixed hex string."""
    if isinstance(value, str):
        return bool(re.match(r'^0x[0-9a-fA-F]+$', value))
    return False


def get_first_hex_address(input_file):
    """Return the integer value of the first hex address found in the trace."""
    try:
        with open(input_file, 'r') as f:
            data = json.load(f)
        if not isinstance(data, list):
            return None
        for obj in data:
            if isinstance(obj, dict) and 'name' in obj:
                if is_hex_address(obj['name']):
                    return int(obj['name'], 16)
        return None
    except Exception:
        return None


def find_symbol_offset(symbol_name, executable):
    """
    Find the offset of a symbol in the executable using nm.

    Args:
        symbol_name: Symbol to search for (can be partial)
        executable:  Path to the executable

    Returns:
        Symbol offset (int) or None if not found
    """
    try:
        result = subprocess.run(
            ['nm', '-C', executable],
            capture_output=True,
            text=True,
            timeout=10,
        )

        if result.returncode != 0:
            log(f"Error running nm: {result.stderr}")
            return None

        matches = []
        for line in result.stdout.split('\n'):
            if not line.strip():
                continue
            parts = line.split()
            if len(parts) < 3:
                continue
            addr_str = parts[0]
            symbol   = ' '.join(parts[2:])
            if symbol_name in symbol:
                try:
                    matches.append((symbol, int(addr_str, 16)))
                except ValueError:
                    continue

        if not matches:
            log(f"Symbol '{symbol_name}' not found in {executable}")
            return None

        if len(matches) > 1:
            log(f"Found {len(matches)} matching symbols:")
            for sym, addr in matches:
                log(f"  0x{addr:x} - {sym}")
            log(f"Using: {matches[0][0]}")

        sym, addr = matches[0]
        log(f"Found symbol: {sym} at offset 0x{addr:x}")
        return addr

    except subprocess.TimeoutExpired:
        log("Error: nm timed out")
        return None
    except Exception as e:
        log(f"Error finding symbol: {e}")
        return None


# ---------------------------------------------------------------------------
# Core multithreaded processor
# ---------------------------------------------------------------------------

def resolve_addresses_parallel(addresses, executable, max_workers=None):
    """
    Resolve a collection of unique hex-address strings → symbols in parallel.

    Each worker thread keeps a single persistent addr2line process alive for
    its lifetime, sending addresses on stdin and reading results from stdout.
    This avoids the per-address subprocess spawn cost that causes timeouts.

    Args:
        addresses:   Iterable of unique hex-address strings.
        executable:  Path to the executable binary.
        max_workers: Thread-pool size (default: min(8, cpu_count)).
                     High counts don't help here — addr2line is I/O-bound on
                     the ELF debug sections, not CPU-bound.

    Returns:
        dict mapping each input address string to its resolved symbol string.
    """
    addr2line_path = _get_addr2line_path()
    if addr2line_path is None:
        print("Error: addr2line not found", file=sys.stderr)
        return {a: a for a in addresses}

    unique = list(set(addresses))
    results = {}

    if not unique:
        return results

    if max_workers is None:
        max_workers = min(8, os.cpu_count() or 4)

    log(f"Resolving {len(unique)} unique addresses with {max_workers} workers…")

    completed = 0
    lock = threading.Lock()
    workers = []

    def _lookup(addr):
        worker = _get_worker(executable, addr2line_path)
        return addr, worker.lookup(addr)

    def _collect_workers():
        """Harvest the worker created in each thread so we can close them."""
        w = getattr(_thread_local, 'worker', None)
        if w is not None:
            workers.append(w)

    with ThreadPoolExecutor(max_workers=max_workers) as pool:
        future_to_addr = {pool.submit(_lookup, addr): addr for addr in unique}
        for future in as_completed(future_to_addr):
            addr, symbol = future.result()
            results[addr] = symbol

            with lock:
                completed += 1
                if VERBOSE and completed % 200 == 0:
                    log(f"  … {completed}/{len(unique)} resolved")

        # Cleanly shut down each persistent addr2line process.
        pool.map(lambda _: _collect_workers(), range(max_workers))

    for w in workers:
        w.close()

    return results


def process_json_array(input_file, executable, output_file=None,
                       base_address=0, max_workers=None):
    """
    Process JSON array, converting hex addresses to symbols in parallel.

    Args:
        input_file:   Path to input JSON file
        executable:   Path to the executable binary
        output_file:  Path to output JSON file (None → stdout)
        base_address: Base address for PIE executables (0 = none / auto)
        max_workers:  Thread-pool size (None = auto)
    """
    try:
        with open(input_file, 'r') as f:
            data = json.load(f)

        if not isinstance(data, list):
            print("Error: JSON must be an array", file=sys.stderr)
            sys.exit(1)

        if not os.path.exists(executable):
            print(f"Error: Executable not found: {executable}", file=sys.stderr)
            sys.exit(1)

        log(f"Processing {len(data)} JSON objects…")
        log(f"Executable: {executable}")

        # ── Detect embedded base address ─────────────────────────────────────
        base_address_obj = None
        for i, obj in enumerate(data):
            if isinstance(obj, dict) and obj.get('name') == '__base_address__':
                if 'args' in obj and 'base' in obj['args']:
                    base_address = int(obj['args']['base'], 16)
                    base_address_obj = i
                    log(f"Found base address in JSON: 0x{base_address:x}")
                    break

        if base_address > 0:
            log(f"Using base address: 0x{base_address:x}")

        # ── Collect every hex address that needs resolving ───────────────────
        addr_to_indices = {}   # raw hex string → [list of obj indices]
        for i, obj in enumerate(data):
            if i == base_address_obj:
                continue
            if isinstance(obj, dict) and 'name' in obj:
                name = obj['name']
                if is_hex_address(name):
                    # Compute the relative address we will actually look up.
                    # Only subtract the base when the address is above it;
                    # addresses already below the base are already relative.
                    addr_int = int(name, 16)
                    if base_address > 0 and addr_int >= base_address:
                        addr_int -= base_address
                    lookup_key = f"0x{addr_int:x}"
                    addr_to_indices.setdefault(lookup_key, []).append(i)

        if not addr_to_indices:
            log("No hex addresses found – nothing to do.")
        else:
            # ── Resolve in parallel ──────────────────────────────────────────
            symbol_map = resolve_addresses_parallel(
                addr_to_indices.keys(), executable, max_workers=max_workers
            )

            # ── Apply results back to the data ───────────────────────────────
            changed = 0
            for lookup_key, indices in addr_to_indices.items():
                symbol = symbol_map.get(lookup_key, lookup_key)
                for i in indices:
                    original = data[i]['name']
                    data[i]['name'] = symbol
                    if symbol != original:
                        log(f"[{i}] {original} -> {symbol}")
                        changed += 1

            log(f"\nReplaced {changed} / {sum(len(v) for v in addr_to_indices.values())} addresses.")

        # ── Write output ─────────────────────────────────────────────────────
        output_json = json.dumps(data, indent=2)

        if output_file:
            with open(output_file, 'w') as f:
                f.write(output_json)
            log(f"Output written to: {output_file}")
        else:
            print(output_json)

    except json.JSONDecodeError as e:
        print(f"Error: Invalid JSON: {e}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)


# ---------------------------------------------------------------------------
# Utility / diagnostic helpers (unchanged in behaviour, updated to use log())
# ---------------------------------------------------------------------------

def calculate_base_address(input_file, executable, symbol_name):
    log(f"\nCalculating base address using symbol: {symbol_name}")
    log("=" * 60)

    first_addr = get_first_hex_address(input_file)
    if first_addr is None:
        log("Error: No hex addresses found in trace")
        return

    log(f"First address in trace: 0x{first_addr:x}")
    symbol_offset = find_symbol_offset(symbol_name, executable)
    if symbol_offset is None:
        return

    base_address = first_addr - symbol_offset
    log(f"\nCalculated base address: 0x{base_address:x}")
    log(f"Verify with: python3 addr2line-converter.py --try-base {input_file} 0x{base_address:x}")


def check_debug_symbols(executable):
    try:
        result = subprocess.run(['file', executable], capture_output=True,
                                text=True, timeout=5)
        print(f"File info: {result.stdout.strip()}")

        result = subprocess.run(['readelf', '-S', executable],
                                capture_output=True, text=True, timeout=5)

        has_debug  = '.debug_info'  in result.stdout or '.debug_abbrev' in result.stdout
        has_symtab = '.symtab'      in result.stdout
        has_dynsym = '.dynsym'      in result.stdout

        print(f"\nDebug symbols (.debug_*): {has_debug}")
        print(f"Symbol table (.symtab):    {has_symtab}")
        print(f"Dynamic symbols (.dynsym): {has_dynsym}")

        if not has_debug and not has_symtab:
            print("\nWarning: No debug symbols found. Try:")
            print("  - Rebuild with -g flag")
            print("  - Check if binary is stripped")

    except Exception as e:
        print(f"Error checking debug symbols: {e}", file=sys.stderr)


def test_base_address(input_file, executable, base_address, num_samples=10):
    try:
        with open(input_file, 'r') as f:
            data = json.load(f)

        if not isinstance(data, list):
            log("Error: JSON must be an array")
            return

        log(f"\nTesting base address: 0x{base_address:x}")
        log("=" * 60)

        count = 0
        for obj in data:
            if not isinstance(obj, dict) or 'name' not in obj:
                continue
            original_name = obj['name']
            if is_hex_address(original_name):
                addr     = int(original_name, 16)
                rel_addr = (addr - base_address
                            if base_address > 0 and addr >= base_address
                            else addr)
                lookup   = f"0x{rel_addr:x}"
                symbol   = get_symbol_for_address(lookup, executable)
                log(f"{original_name} - 0x{base_address:x} = {lookup} -> {symbol}")
                count += 1
                if count >= num_samples:
                    break

        if count == 0:
            log("No hex addresses found in JSON")

    except Exception as e:
        log(f"Error: {e}")


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------

def main():
    global VERBOSE

    # ── Strip quiet flag anywhere in argv ────────────────────────────────────
    args = sys.argv[1:]
    if '--quiet' in args or '-q' in args:
        VERBOSE = False
        args = [a for a in args if a not in ('--quiet', '-q')]

    # ── Strip --workers N ─────────────────────────────────────────────────────
    max_workers = None
    if '--workers' in args:
        idx = args.index('--workers')
        if idx + 1 < len(args):
            max_workers = int(args[idx + 1])
            args = args[:idx] + args[idx + 2:]

    if not args:
        print("Usage: python3 addr2line-converter.py <input.json> [output.json]", file=sys.stderr)
        print("                                       [--base-address 0xADDR]", file=sys.stderr)
        print("                                       [--workers N] [--quiet|-q]", file=sys.stderr)
        print("", file=sys.stderr)
        print("Converts hex addresses in JSON 'name' fields to symbols using addr2line.", file=sys.stderr)
        print("", file=sys.stderr)
        print("The executable path defaults to ~/git/impacto/install/LocalSwitch/impacto", file=sys.stderr)
        print("Set DEVKITPRO / IMPACTO_EXECUTABLE environment variables to override.", file=sys.stderr)
        print("", file=sys.stderr)
        print("Options:", file=sys.stderr)
        print("  --test <address>         Test a single address conversion", file=sys.stderr)
        print("  --check-symbols          Check if executable has debug symbols", file=sys.stderr)
        print("  --base-address <addr>    Base address for PIE binary", file=sys.stderr)
        print("  --try-base <addr>        Test a base address (shows first 10 conversions)", file=sys.stderr)
        print("  --calc-base <symbol>     Calculate base address from a known symbol", file=sys.stderr)
        print("  --workers <N>            Thread-pool size (default: auto)", file=sys.stderr)
        print("  --quiet / -q             Suppress all stderr progress output", file=sys.stderr)
        sys.exit(1)

    default_executable = os.path.expanduser("~/git/impacto/install/LocalSwitch/impacto")
    executable = os.environ.get('IMPACTO_EXECUTABLE', default_executable)

    # ── Special sub-commands ──────────────────────────────────────────────────
    if args[0] == '--test' and len(args) > 1:
        symbol = get_symbol_for_address(args[1], executable)
        print(f"{args[1]} -> {symbol}")
        return

    if args[0] == '--check-symbols':
        check_debug_symbols(executable)
        return

    if args[0] == '--try-base' and len(args) > 1:
        input_file   = args[1]
        base_address = int(args[2], 16) if len(args) > 2 else 0
        test_base_address(input_file, executable, base_address)
        return

    if args[0] == '--calc-base' and len(args) > 1:
        input_file  = args[1]
        symbol_name = args[2] if len(args) > 2 else "InitFromProfile"
        calculate_base_address(input_file, executable, symbol_name)
        return

    # ── Normal conversion ─────────────────────────────────────────────────────
    input_file   = args[0]
    output_file  = None
    base_address = 0

    i = 1
    while i < len(args):
        if args[i] == '--base-address' and i + 1 < len(args):
            raw = args[i + 1]
            base_address = int(raw, 16) if raw.startswith('0x') else int(raw)
            log(f"Using base address: 0x{base_address:x}")
            i += 2
        else:
            output_file = args[i]
            i += 1

    process_json_array(input_file, executable, output_file,
                       base_address, max_workers=max_workers)


if __name__ == '__main__':
    main()