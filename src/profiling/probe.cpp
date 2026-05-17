#include "probe.h"
#include <array>
#include <atomic>
#include <chrono>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <fmt/format.h>

std::atomic<bool> g_EnableProfiling{false};
static bool g_initialized = false;
static FILE* g_trace_file = nullptr;
static std::mutex g_write_mutex;

static thread_local bool t_instrumenting = false;

void enableProbing() {
  if (g_EnableProfiling.load(std::memory_order_relaxed)) return;
  g_trace_file = fopen("trace.json", "w");
  if (g_trace_file) fmt::print(g_trace_file, "[\n");
  g_initialized = true;
  g_EnableProfiling.store(true, std::memory_order_release);
}

void endProbing() {
  if (!g_EnableProfiling.load(std::memory_order_relaxed)) return;
  g_EnableProfiling.store(false, std::memory_order_release);
  if (g_trace_file) {
    fmt::print(g_trace_file, "\n]\n");
    fclose(g_trace_file);
    g_trace_file = nullptr;
  }
}

#ifdef __cplusplus
extern "C" {
#endif

void __cyg_profile_func_enter(void* this_fn, void* call_site) NOINSTRUMENT;
void __cyg_profile_func_exit(void* this_fn, void* call_site) NOINSTRUMENT;

static inline void NOINSTRUMENT record(void* fn, bool is_enter) {
  char buf[128];
  auto ts = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now().time_since_epoch())
                .count();
  auto tid = reinterpret_cast<uintptr_t>(pthread_self());
  auto [out, size] = fmt::format_to_n(
      buf, sizeof(buf) - 1,
      R"({{"cat":"PERF","name":"0x{:x}","ph":"{}","pid":{},"tid":{},"ts":{}}})",
      reinterpret_cast<uintptr_t>(fn), is_enter ? "B" : "E", ::getpid(), tid,
      ts);
  *out = 0;

  std::lock_guard<std::mutex> lock(g_write_mutex);
  fwrite(buf, 1, size, g_trace_file);
}

void __cyg_profile_func_enter(void* this_fn, void* /*call_site*/) {
  if (!g_initialized) return;
  if (t_instrumenting) return;
  if (!g_EnableProfiling.load(std::memory_order_relaxed)) return;
  t_instrumenting = true;
  record(this_fn, true);
  t_instrumenting = false;
}

void __cyg_profile_func_exit(void* this_fn, void* /*call_site*/) {
  if (!g_initialized) return;
  if (t_instrumenting) return;
  if (!g_EnableProfiling.load(std::memory_order_relaxed)) return;
  t_instrumenting = true;
  record(this_fn, false);
  t_instrumenting = false;
}

#ifdef __cplusplus
}
#endif