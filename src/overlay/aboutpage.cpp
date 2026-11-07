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

static void ShowPlayTime(char const* label, int totalSeconds) {
  int hours = totalSeconds / 3600;
  int minutes = (totalSeconds % 3600) / 60;
  int seconds = totalSeconds % 60;
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

  std::filesystem::file_status status;
  std::filesystem::file_type fileType;
  while (true) {
    status = std::filesystem::status(uninstallerPath);
    fileType = status.type();

    if (fileType == std::filesystem::file_type::symlink) {
      uninstallerPath = std::filesystem::read_symlink(uninstallerPath);
    } else {
      break;
    }
  }

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

struct BuildVersion {
  size_t Major;
  size_t Minor;
  size_t Patch;

  std::strong_ordering operator<=>(const BuildVersion& rhs) const {
    const auto majorComp = Major <=> rhs.Major;
    if (majorComp != std::strong_ordering::equal) return majorComp;

    const auto minorComp = Minor <=> rhs.Minor;
    if (minorComp != std::strong_ordering::equal) return minorComp;

    return Patch <=> rhs.Patch;
  }
};

[[nodiscard]] static std::optional<BuildVersion> GetLatestVersion() {
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

  pugi::xml_document checkUpdateDocument;
  pugi::xml_parse_result checkUpdateResult =
      checkUpdateDocument.load_buffer_inplace(outputBuffer.data(),
                                              outputBuffer.size());
  if (!checkUpdateResult) {
    ImpLog(LogLevel::Error, LogChannel::Overlay,
           "Failed to parse output stream as xml");
    return std::nullopt;
  }

  constexpr const char* buildName =
#if defined(SDL_PLATFORM_LINUX)
      "Impacto (Linux)";
#elif defined(SDL_PLATFORM_WINDOWS)
      "Impacto (Windows)";
#elif defined(SDL_PLATFORM_MACOS)

#if defined(__arm__) || defined(__aarch64__)
      "Impacto (Mac OS Intel)";
#elif defined(__x86_64__) || defined(_M_X64)
      "Impacto (Mac OS ARM)";
#else
      "";
  static_assert(false && "No build name defined for this architecture");
#endif

#elif defined(SDL_PLATFORM_ANDROID)
      "Impacto (Android)";
#elif defined(SDL_PLATFORM_SWITCH)
      "Impacto (Switch)";
#else
      "";
  static_assert(false && "No build name defined for this target");
#endif

  const pugi::xml_node latestBuildVersion =
      checkUpdateDocument.child("updates").find_child_by_attribute(
          "update", "name", buildName);
  if (latestBuildVersion.empty()) {
    ImpLog(LogLevel::Error, LogChannel::Overlay,
           "Update checker did not report on current build \"{:s}\"",
           buildName);
    return std::nullopt;
  }

  const char* const latestVersionString =
      latestBuildVersion.attribute("version").value();

  const std::regex versionRegex("^(\\d+)\\.(\\d+)\\.(\\d+)\\.\\d+$");
  std::cmatch versionRegexMatch;
  if (!std::regex_match(latestVersionString, versionRegexMatch, versionRegex)) {
    ImpLog(LogLevel::Error, LogChannel::Overlay,
           "Failed to parse latest version string");
    return std::nullopt;
  }

  return BuildVersion{
      .Major =
          static_cast<size_t>(std::atoll(versionRegexMatch[1].str().c_str())),
      .Minor =
          static_cast<size_t>(std::atoll(versionRegexMatch[2].str().c_str())),
      .Patch =
          static_cast<size_t>(std::atoll(versionRegexMatch[3].str().c_str())),
  };
}

void ShowAboutPage(std::string const& selectedGame) {
  ImGui::Text("Committee of Zero");
  ImGui::SameLine();
  ImGui::TextLinkOpenURL("Technical Support", "https://discord.gg/hRtvaYawg6");

  constexpr BuildVersion currentVersion{
      .Major = VERSION_MAJOR, .Minor = VERSION_MINOR, .Patch = VERSION_PATCH};
  ImGui::Text("Impacto Version %zu.%zu.%zu", currentVersion.Major,
              currentVersion.Minor, currentVersion.Patch);

  if (GetUninstallerPath().has_value()) {
    static std::optional<BuildVersion> latestCheckedVersion = std::nullopt;
    static std::optional<std::future<std::optional<BuildVersion>>>
        latestVersionFuture = std::nullopt;
    static bool checkedForUpdates = false;

    if (!latestCheckedVersion.has_value() ||
        currentVersion >= *latestCheckedVersion) {
      bool checkingForUpdates = latestVersionFuture.has_value();

      ImGui::SameLine();
      ImGui::PushItemFlag(ImGuiItemFlags_Disabled, checkingForUpdates);
      if (ImGui::Button("Check For Updates")) {
        latestVersionFuture = std::async(std::launch::async, GetLatestVersion);
        latestCheckedVersion.reset();
        checkingForUpdates = true;
      }
      ImGui::PopItemFlag();

      if (checkingForUpdates &&
          latestVersionFuture->wait_for(std::chrono::seconds(0)) !=
              std::future_status::timeout) {
        latestCheckedVersion = latestVersionFuture->get();
        latestVersionFuture.reset();
        checkedForUpdates = true;
      }

      if (checkingForUpdates) {
        ImGui::SameLine();
        ImGui::Text("Checking...");
      } else if (latestCheckedVersion.has_value()) {
        assert(currentVersion >= *latestCheckedVersion);
        ImGui::SameLine();
        ImGui::Text("Up to date");
      } else if (checkedForUpdates) {
        ImGui::SameLine();
        ImGui::Text("Failed checking for updates");
      }

    } else if (latestCheckedVersion.has_value()) {
      assert(currentVersion < *latestCheckedVersion);

      ImGui::SameLine();
      if (ImGui::Button("Update")) {
        const static std::string uninstallerPath =
            GetUninstallerPath()->string();
        const char* const args[]{uninstallerPath.c_str(), nullptr};
        // Start the process but don't monitor it
        SDL_DestroyProcess(SDL_CreateProcess(args, true));
      }

      ImGui::SameLine();
      ImGui::Text("Newer version available (%zu.%zu.%zu)",
                  latestCheckedVersion->Major, latestCheckedVersion->Minor,
                  latestCheckedVersion->Patch);
    }
  }

  ImGui::Text("OS: %s", SDL_GetPlatform());
  ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
              ImGui::GetIO().Framerate);
  if (Profile::Game::HasInit) {
    ImGui::Separator();
    ImGui::Text("%s", GetGameDisplayName(selectedGame).c_str());
    ShowPlayTime("Current Session Play Time", ScrWork[SW_PLAYTIME]);
    ShowPlayTime("Total Play Time", ScrWork[SW_TOTALPLAYTIME]);
  }
}

}  // namespace Impacto::Overlay
