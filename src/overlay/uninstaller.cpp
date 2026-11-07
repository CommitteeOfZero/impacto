#include "uninstaller.h"

#include <SDL3/SDL_platform_defines.h>

#include "../log.h"
#include "../game.h"
#include "../profile/basepaths.h"

namespace Impacto::Overlay {

std::optional<std::filesystem::path> GetUninstallerPath() {
  static const auto uninstallerPath =
      []() -> std::optional<std::filesystem::path> {
#if defined(SDL_PLATFORM_ANDROID) || defined(SDL_PLATFORM_SWITCH)
    // Defined no uninstaller
    ImpLog(
        LogLevel::Info, LogChannel::Overlay,
        "This target does not have an uninstaller present (defined behavior)");
    return std::nullopt;
#else
    std::filesystem::path path =
        std::filesystem::path(Profile::BasePaths::RootInstallDir) /
#if defined(SDL_PLATFORM_LINUX)
        "ImpactoUninstaller";
#elif defined(SDL_PLATFORM_WINDOWS)
        "ImpactoUninstaller.exe";
#elif defined(SDL_PLATFORM_MACOS)
        "ImpactoUninstaller.app/Contents/MacOS/ImpactoUninstaller";
#else
        "";
    static_assert(false && "No uninstaller location defined for this target");
#endif

    if (!std::filesystem::is_regular_file(path)) return std::nullopt;
    path = std::filesystem::canonical(path);

    const std::filesystem::file_status status = std::filesystem::status(path);
    if ((status.permissions() & std::filesystem::perms::others_exec) ==
        std::filesystem::perms::none) {
      ImpLog(LogLevel::Error, LogChannel::Overlay,
             "No permission to execute {}", path);
      return std::nullopt;
    }

    return path;
#endif
  }();

  return uninstallerPath;
}

void CheckUpdateProcess::StartCheckUpdate() {
  if (Worker.valid()) {
    ImpLog(LogLevel::Warning, LogChannel::Overlay,
           "Update checker is already running");
    return;
  }

  CachedResult.reset();
  Cancel = false;
  Worker = std::async(std::launch::async, RunCheck);
}

std::optional<CheckUpdateProcess::UpdateCheckResult>
CheckUpdateProcess::CheckUpdateStatus() {
  // Async wait is blocking on destructor
  if (Game::ShouldQuit && Worker.valid()) {
    Cancel = true;
    Worker.wait();
  }

  // std::future can only be read once. Cache the result so it can be read
  // multiple times
  if (!CachedResult.has_value()) {
    if (!Worker.valid() || Worker.wait_for(std::chrono::seconds(0)) ==
                               std::future_status::timeout) {
      return std::nullopt;
    }

    CachedResult = Worker.get();
  }

  return CachedResult;
}

CheckUpdateProcess::UpdateCheckResult CheckUpdateProcess::RunCheck() {
  const std::optional<std::filesystem::path> uninstallerPath =
      GetUninstallerPath();
  if (!uninstallerPath.has_value()) {
    ImpLog(LogLevel::Error, LogChannel::Overlay,
           "Uninstaller path not found; cannot check for updates");
    return UpdateCheckResult::Error;
  }

  const std::string uninstallerPathString = uninstallerPath->string();

  const char* const args[] = {
      uninstallerPathString.c_str(), "check-updates", "-g", "js=false", nullptr,
  };

  ScopedProcess process(SDL_CreateProcess(args, true));
  auto waitStartTime = std::chrono::steady_clock::now();

  if (process == nullptr) {
    ImpLog(LogLevel::Error, LogChannel::Overlay,
           "Failed to start check-updates process: {:s}", SDL_GetError());
    return UpdateCheckResult::Error;
  }

  SDL_IOStream* const stream = SDL_GetProcessOutput(process.get());
  if (stream == nullptr) {
    ImpLog(LogLevel::Error, LogChannel::Overlay,
           "Failed to fetch update-checker output: {:s}", SDL_GetError());

    if (!SDL_KillProcess(process.get(), true)) {
      ImpLog(LogLevel::Error, LogChannel::Overlay,
             "Failed to kill update-checker. Error: {:s}", SDL_GetError());
    }
    SDL_WaitProcess(process.get(), true, nullptr);
    return UpdateCheckResult::Error;
  }

  uint8_t killCount = 0;

  int exitCode = EXIT_SUCCESS;
  while (!SDL_WaitProcess(process.get(), false, &exitCode)) {
    SDL_Delay(100);
    if (Cancel) {
      SDL_KillProcess(process.get(), true);
      return UpdateCheckResult::Error;
    }

    const auto waitDuration = std::chrono::steady_clock::now() - waitStartTime;
    if (waitDuration >= std::chrono::seconds(5)) {
      if (killCount == 2) break;
      ImpLog(LogLevel::Error, LogChannel::Overlay,
             "Update checker is not responding. Killing process");
      SDL_KillProcess(process.get(), killCount++ > 0);
      waitStartTime = std::chrono::steady_clock::now();
    }
  }

  if (killCount > 0 || exitCode != EXIT_SUCCESS) {
    return UpdateCheckResult::Error;
  }

  if (SDL_GetIOStatus(stream) == SDL_IO_STATUS_ERROR) {
    ImpLog(LogLevel::Error, LogChannel::Overlay,
           "Failed to read from output stream: {:s}", SDL_GetError());
    return UpdateCheckResult::Error;
  }

  const bool hasOutput = SDL_ReadU8(stream, nullptr);
  return hasOutput ? UpdateCheckResult::UpdatesAvailable
                   : UpdateCheckResult::NoUpdates;
}

}  // namespace Impacto::Overlay
