#include "aboutpage.h"

#include <filesystem>
#include <chrono>
#include <regex>
#include <future>

#include <imgui.h>
#include <SDL3/SDL_platform_defines.h>
#include <SDL3/SDL_process.h>
#include <pugixml.hpp>

#include <version.h>
#include "utils.h"
#include "../log.h"
#include "../mem.h"
#include "../profile/basepaths.h"
#include "../profile/game.h"
#include "../profile/scriptvars.h"

namespace Impacto::Overlay {

using namespace Impacto::Profile::ScriptVars;

static void ShowPlayTime(char const* label, const uint32_t totalSeconds) {
  const uint32_t hours = totalSeconds / 3600;
  const uint32_t minutes = (totalSeconds % 3600) / 60;
  const uint32_t seconds = totalSeconds % 60;
  ImGui::Text("%s: %02d:%02d:%02d", label, hours, minutes, seconds);
}

[[nodiscard]] static std::optional<std::filesystem::path> GetUninstallerPath() {
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
             "No permission to execute the uninstaller");
      return std::nullopt;
    }

    return path;
#endif
  }();

  return uninstallerPath;
}

class CheckUpdateProcess {
 public:
  enum class UpdateCheckResult {
    NoUpdates,
    UpdatesAvailable,
    Error,
  };

  static void StartCheckUpdate() {
    if (Worker.valid()) {
      ImpLog(LogLevel::Warning, LogChannel::Overlay,
             "Update checker is already running");
      return;
    }

    CachedResult.reset();
    Cancel = false;
    Worker = std::async(std::launch::async, RunCheck);
  }

  [[nodiscard]] static std::optional<UpdateCheckResult> CheckUpdateStatus() {
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

  [[nodiscard]] static bool CheckingForUpdates() { return Worker.valid(); }

 private:
  using ScopedProcess =
      std::unique_ptr<SDL_Process, decltype(&SDL_DestroyProcess)>;

  [[nodiscard]] static UpdateCheckResult RunCheck() {
    const std::filesystem::path uninstallerPath = *GetUninstallerPath();
    const std::string uninstallerPathString = uninstallerPath.string();

    const char* const args[] = {
        uninstallerPathString.c_str(),
        "check-updates",
        "-g",
        "js=false",
        nullptr,
    };

    ScopedProcess process(SDL_CreateProcess(args, true), SDL_DestroyProcess);
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

      const auto waitDuration =
          std::chrono::steady_clock::now() - waitStartTime;
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

  inline static std::future<UpdateCheckResult> Worker;
  inline static std::atomic<bool> Cancel = false;
  inline static std::optional<UpdateCheckResult> CachedResult;
};

void ShowAboutPage(std::string const& selectedGame) {
  ImGui::Text("Committee of Zero");
  ImGui::SameLine();
  ImGui::TextLinkOpenURL("Technical Support", "https://discord.gg/hRtvaYawg6");

  ImGui::Text("Impacto Version %d.%d.%d", VERSION_MAJOR, VERSION_MINOR,
              VERSION_PATCH);

  if (GetUninstallerPath().has_value()) {
    const std::optional<CheckUpdateProcess::UpdateCheckResult>
        updateCheckResult = CheckUpdateProcess::CheckUpdateStatus();
    if (updateCheckResult !=
        CheckUpdateProcess::UpdateCheckResult::UpdatesAvailable) {
      const bool checkingForUpdates = CheckUpdateProcess::CheckingForUpdates();

      ImGui::SameLine();
      ImGui::PushItemFlag(ImGuiItemFlags_Disabled, checkingForUpdates);
      if (ImGui::Button("Check For Updates")) {
        CheckUpdateProcess::StartCheckUpdate();
      }
      ImGui::PopItemFlag();

      if (checkingForUpdates) {
        ImGui::SameLine();
        ImGui::Text("Checking...");
      } else if (updateCheckResult ==
                 CheckUpdateProcess::UpdateCheckResult::NoUpdates) {
        ImGui::SameLine();
        ImGui::Text("Up to date");
      } else if (updateCheckResult ==
                 CheckUpdateProcess::UpdateCheckResult::Error) {
        ImGui::SameLine();
        ImGui::Text("Failed checking for updates");
      }

    } else {
      ImGui::SameLine();
      if (ImGui::Button("Update")) {
        ImGui::OpenPopup("UpdateDialogue");
      }
      if (ImGui::BeginPopup("UpdateDialogue")) {
        ImGui::SeparatorText("Update?");
        ImGui::Text(
            "Selecting \"Update\" will close this application.\n"
            "All unsaved progress will be lost. Are you sure?");

        if (ImGui::Button("Update")) {
          const static std::string uninstallerPath =
              GetUninstallerPath()->string();
          const char* const args[]{uninstallerPath.c_str(), nullptr};
          // Start the process but don't monitor it
          SDL_DestroyProcess(SDL_CreateProcess(args, true));

          Game::Shutdown();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
          ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
      }

      ImGui::SameLine();
      ImGui::Text("Update(s) available");
    }
  }

  ImGui::Text("OS: %s", SDL_GetPlatform());
  ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
              ImGui::GetIO().Framerate);

  if (Profile::Game::HasInit) {
    ImGui::Separator();
    ImGui::Text("%s", GetGameDisplayName(selectedGame).c_str());

    ShowPlayTime("Current Session Play Time", Game::SessionPlayTimeSeconds);

    const bool inGame = ScrWork[SW_GAMESTATE] & 1;
    if (inGame) {
      ShowPlayTime("Current Save Play Time",
                   static_cast<uint32_t>(ScrWork[SW_PLAYTIME]));
    }

    ShowPlayTime("Total Play Time",
                 static_cast<uint32_t>(ScrWork[SW_TOTALPLAYTIME]));
  }
}

}  // namespace Impacto::Overlay
