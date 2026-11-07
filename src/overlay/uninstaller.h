#pragma once

#include <optional>
#include <future>
#include <memory>
#include <filesystem>

#include <SDL3/SDL_process.h>

namespace Impacto::Overlay {

[[nodiscard]] std::optional<std::filesystem::path> GetUninstallerPath();

class CheckUpdateProcess {
 public:
  enum class UpdateCheckResult {
    NoUpdates,
    UpdatesAvailable,
    Error,
  };

  static void StartCheckUpdate();

  [[nodiscard]] static std::optional<UpdateCheckResult> CheckUpdateStatus();

  [[nodiscard]] static bool CheckingForUpdates() { return Worker.valid(); }

 private:
  using ScopedProcess =
      std::unique_ptr<SDL_Process, decltype([](SDL_Process* process) {
                        SDL_DestroyProcess(process);
                      })>;

  [[nodiscard]] static UpdateCheckResult RunCheck();

  inline static std::future<UpdateCheckResult> Worker;
  inline static std::atomic<bool> Cancel = false;
  inline static std::optional<UpdateCheckResult> CachedResult;
};

}  // namespace Impacto::Overlay
