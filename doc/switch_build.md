# Building for Nintendo Switch (Horizon / devkitPro)

The Switch build is driven by **vcpkg**, like every other platform. Dependencies that
vcpkg builds from source on the desktop platforms (today: `fmt`) are built the same
way here, cross-compiled with the devkitPro toolchain. Dependencies that the
**devkitPro portlibs** already provide as system packages (SDL2, zlib, libogg,
libvorbis, libwebp, libass, ffmpeg, OpenAL) are instead shipped as *overlay ports*
under [`../portfiles-switch/`](../portfiles-switch): vcpkg builds no library binaries
for them and CMake resolves the copy already provided by devkitPro. For libraries
devkitPro ships without a CMake config (libogg, libvorbis, libwebp) the overlay
installs a tiny `*Config.cmake` wrapper around the devkitPro pkg-config, so impacto
uses the same `find_package(... CONFIG)` interface and imported targets on every
platform. `avcpp` sits on top of the system ffmpeg, so it stays on impacto's
`FetchContent` + pkg-config path and is overlaid empty as well. See the
[overlay README](../portfiles-switch/README.md) and
<https://devblogs.microsoft.com/cppblog/using-system-package-manager-dependencies-with-vcpkg/>
for the technique.

## Prerequisites

* **devkitPro** with the devkitA64 toolchain, `libnx`, and the Switch portlibs
  (`switch-dev` group plus `switch-sdl2`, `switch-zlib`, `switch-libogg`,
  `switch-libvorbis`, `switch-libwebp`, `switch-ffmpeg`, `switch-dav1d`,
  `switch-libass`, and OpenAL). Install with `dkp-pacman`.
* The `DEVKITPRO` environment variable pointing at your devkitPro install
  (e.g. `/opt/devkitpro`).
* A **vcpkg** checkout, with the `VCPKG_ROOT` environment variable pointing at it.
* **CMake** ≥ 3.28 and **Ninja**.

## Building

From the repository root:

```bash
cmake --preset ci-release-switch
cmake --build --preset ci-release-switch
```

This configures with the vcpkg toolchain, the `arm64-switch` triplet (which
chainloads `HorizonNX.toolchain`), and the Switch overlay ports, then builds and
runs the `install` step. The staged build — the `impacto` ELF plus `shaders`,
`profiles`, `games`, and the `*.lua` config files — lands in
`release/ci-release-switch/`.

## Producing an NRO

Wrap the installed ELF for distribution:

```bash
elf2nro release/ci-release-switch/impacto release/ci-release-switch/impacto.nro
```

The `switch` job in [`../.github/workflows/impacto.yml`](../.github/workflows/impacto.yml)
runs exactly these steps inside the `ghcr.io/committeeofzero/impacto-switch`
container.
