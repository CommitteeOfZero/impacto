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
  static std::optional<std::optional<std::filesystem::path>> cached;
  if (cached.has_value()) return *cached;

#if defined(SDL_PLATFORM_ANDROID) || defined(SDL_PLATFORM_SWITCH)
  // Defined no uninstaller
  ImpLog(LogLevel::Info, LogChannel::Overlay,
         "This target does not have an uninstaller present (defined behavior)");
  cached.emplace(std::nullopt);
#else
  std::filesystem::path uninstallerPath =
      std::filesystem::path(Profile::BasePaths::RootInstallDir) /
#if defined(SDL_PLATFORM_LINUX)
      "ImpactoUninstaller";
#elif defined(SDL_PLATFORM_WINDOWS)
      "ImpactoUninstaller.exe";
#elif defined(SDL_PLATFORM_MACOS)
      "ImpactoUninstaller.app";
#else
      "";
  static_assert(false && "No uninstaller location defined for this target");
#endif

  if (!std::filesystem::exists(uninstallerPath)) return std::nullopt;
  uninstallerPath = std::filesystem::canonical(uninstallerPath);

  const std::filesystem::file_status status =
      std::filesystem::status(uninstallerPath);
  const std::filesystem::file_type fileType = status.type();

  if (fileType == std::filesystem::file_type::regular) {
    cached.emplace(uninstallerPath);
  } else {
    switch (fileType) {
      using enum std::filesystem::file_type;
      case not_found:
        ImpLog(LogLevel::Error, LogChannel::Overlay,
               "Uninstaller not found at {:s}", uninstallerPath.string());
        break;

      case none:
      case unknown:
        ImpLog(LogLevel::Error, LogChannel::Overlay,
               "Failed to verify uninstaller file type ({:s})",
               uninstallerPath.string());
        break;

      case directory:
      case block:
      case character:
      case fifo:
      case socket:
        ImpLog(LogLevel::Error, LogChannel::Overlay,
               "Uninstaller path does not point to a regular file ({:s})",
               uninstallerPath.string());
        break;

      case regular:
      case symlink:
        assert(false && "Already ruled out with prior logic");
        break;

      default:
        ImpLog(LogLevel::Error, LogChannel::Overlay,
               "Unknown error occurred when checking the uninstaller's file "
               "type ({:s})",
               uninstallerPath.string());
        break;
    }

    cached.emplace(std::nullopt);
  }
#endif

  return *cached;
}

[[nodiscard]] static std::optional<bool> CheckUpdatesAvailable() {
  const std::filesystem::path uninstallerPath = *GetUninstallerPath();
  const std::string uninstallerPathString = uninstallerPath.string();

  const char* const args[] = {
      uninstallerPathString.c_str(), "check-updates", "-g", "js=false", nullptr,
  };
  const std::unique_ptr<SDL_Process, decltype(&SDL_DestroyProcess)> process(
      SDL_CreateProcess(args, true), SDL_DestroyProcess);

  if (process == nullptr) {
    ImpLog(LogLevel::Error, LogChannel::Overlay,
           "Failed to start check-updates process: {:s}", SDL_GetError());
    return std::nullopt;
  }

  auto waitStartTime = std::chrono::system_clock::now();
  int exitCode = EXIT_SUCCESS;
  while (!SDL_WaitProcess(process.get(), false, &exitCode)) {
    const auto waitDuration = std::chrono::system_clock::now() - waitStartTime;
    if (waitDuration >= std::chrono::seconds(5)) {
      ImpLog(LogLevel::Error, LogChannel::Overlay,
             "Update checker is not responding. Killing process");

      if (!SDL_KillProcess(process.get(), false)) {
        ImpLog(
            LogLevel::Error, LogChannel::Overlay,
            "Failed to kill update-checker. Attempting force-kill. Error: {:s}",
            SDL_GetError());

        if (!SDL_KillProcess(process.get(), true)) {
          ImpLog(LogLevel::Error, LogChannel::Overlay,
                 "Failed to force-kill update checker: {:s}", SDL_GetError());
        }
      }

      return std::nullopt;
    }
  }

  if (exitCode != EXIT_SUCCESS) {
    ImpLog(LogLevel::Error, LogChannel::Overlay,
           "Update checker returned exit code {:d}", exitCode);
    return std::nullopt;
  }

  const std::unique_ptr<SDL_IOStream, decltype(&SDL_CloseIO)> outputStream(
      SDL_GetProcessOutput(process.get()), SDL_CloseIO);
  if (outputStream == nullptr) {
    ImpLog(LogLevel::Error, LogChannel::Overlay,
           "Failed to fetch update checker output: {:s}", SDL_GetError());
    return std::nullopt;
  }

  std::vector<uint8_t> outputBuffer;
  do {
    outputBuffer.emplace_back(0);
  } while (SDL_ReadU8(outputStream.get(), &outputBuffer.back()));

  if (SDL_GetIOStatus(outputStream.get()) != SDL_IO_STATUS_EOF) {
    ImpLog(LogLevel::Error, LogChannel::Overlay,
           "Failed to read from output stream: {:s}", SDL_GetError());
    return std::nullopt;
  }

  // There is no output iff there are no updates
  return outputBuffer.size() > 1;
}

void ShowAboutPage(std::string const& selectedGame) {
  ImGui::Text("Committee of Zero");
  ImGui::SameLine();
  ImGui::TextLinkOpenURL("Technical Support", "https://discord.gg/hRtvaYawg6");

  ImGui::Text("Impacto Version %d.%d.%d", VERSION_MAJOR, VERSION_MINOR,
              VERSION_PATCH);

  if (GetUninstallerPath().has_value()) {
    static std::optional<bool> updatesAvailable = std::nullopt;
    static std::future<std::optional<bool>> updatesAvailableFuture;
    static bool checkedForUpdates = false;

    if (!updatesAvailable.has_value() || *updatesAvailable == false) {
      bool checkingForUpdates = updatesAvailableFuture.valid();

      ImGui::SameLine();
      ImGui::PushItemFlag(ImGuiItemFlags_Disabled, checkingForUpdates);
      if (ImGui::Button("Check For Updates")) {
        updatesAvailableFuture =
            std::async(std::launch::async, CheckUpdatesAvailable);
        updatesAvailable.reset();
        checkingForUpdates = true;
      }
      ImGui::PopItemFlag();

      if (checkingForUpdates &&
          updatesAvailableFuture.wait_for(std::chrono::seconds(0)) !=
              std::future_status::timeout) {
        updatesAvailable = updatesAvailableFuture.get();
        checkedForUpdates = true;
      }

      if (checkingForUpdates) {
        ImGui::SameLine();
        ImGui::Text("Checking...");
      } else if (updatesAvailable.has_value()) {
        assert(*updatesAvailable == false);
        ImGui::SameLine();
        ImGui::Text("Up to date");
      } else if (checkedForUpdates) {
        ImGui::SameLine();
        ImGui::Text("Failed checking for updates");
      }

    } else if (updatesAvailable.has_value()) {
      assert(*updatesAvailable);

      ImGui::SameLine();
      if (ImGui::Button("Update")) {
        const static std::string uninstallerPath =
            GetUninstallerPath()->string();
        const char* const args[]{uninstallerPath.c_str(), nullptr};
        // Start the process but don't monitor it
        SDL_DestroyProcess(SDL_CreateProcess(args, true));
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
