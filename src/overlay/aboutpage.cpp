#include "aboutpage.h"

#include <imgui.h>
#include <SDL3/SDL_process.h>

#include <version.h>
#include "utils.h"
#include "updater.h"
#include "../mem.h"
#include "../userconfig.h"
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

static void ShowCheckUpdateButton() {
  const std::optional<CheckUpdateProcess::UpdateCheckResult> updateCheckResult =
      CheckUpdateProcess::CheckUpdateStatus();
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
}

static void ShowUpdatePrompt() {
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
      const static std::string uninstallerPath = GetUninstallerPath()->string();
      const char* const args[]{uninstallerPath.c_str(), nullptr};
      // Start the process but don't monitor it
      SDL_DestroyProcess(SDL_CreateProcess(args, true));

      Game::Shutdown();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::SetItemDefaultFocus();

    ImGui::EndPopup();
  }

  ImGui::SameLine();
  ImGui::Text("Update(s) available");
}

void ShowCopyablePath(std::string const& path, const char* label,
                      const char* id) {
  std::string fixedPath = std::filesystem::path(path).make_preferred().string();
  bool& cursorBlink = ImGui::GetIO().ConfigInputTextCursorBlink;
  const bool oldCursorBlink = cursorBlink;
  cursorBlink = false;
  ImGui::TextDisabled("%s", label);
  ImGui::InputText(
      (std::string("##") + id).c_str(), const_cast<char*>(fixedPath.c_str()),
      fixedPath.size() + 1,
      ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_CharsNoBlank);
  ImGui::SameLine();
  if (ImGui::SmallButton((std::string("Copy##Copy") + id).c_str())) {
    ImGui::SetClipboardText(fixedPath.c_str());
  }
  ImGui::SameLine();

  if (ImGui::SmallButton((std::string("Open##Open") + id).c_str())) {
    SDL_OpenURL((std::string("file:///") + fixedPath).c_str());
  }
  cursorBlink = oldCursorBlink;
}

void ShowAboutPage(
    std::string const& selectedGame,
    ankerl::unordered_dense::map<std::string, uint32_t> const& iconTextureMap,
    const float imguiScale) {
  if (auto cozTxtItr = iconTextureMap.find("_coz");
      cozTxtItr != iconTextureMap.end()) {
    auto const& img = cozTxtItr->second;
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    if (ImGui::ImageButton("##coz_logo", (ImTextureID)(intptr_t)img,
                           ImVec2{48 * imguiScale, 48 * imguiScale})) {
      SDL_OpenURL("https://sonome.dareno.me");
    }
    ImGui::SetItemTooltip("Open 'https://sonome.dareno.me'");
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(3);
    ImGui::SameLine();
  }
  ImGui::BeginGroup();
  ImGui::TextUnformatted("Committee of Zero");
  ImGui::TextLinkOpenURL("Technical Support", "https://discord.gg/U2GtuBJyD2");
  ImGui::EndGroup();

  ImGui::NewLine();

  ImGui::Text("Impacto Version: %d.%d.%d", VERSION_MAJOR, VERSION_MINOR,
              VERSION_PATCH);

  if (GetUninstallerPath().has_value()) {
    if (CheckUpdateProcess::CheckUpdateStatus() !=
        CheckUpdateProcess::UpdateCheckResult::UpdatesAvailable) {
      ShowCheckUpdateButton();
    } else {
      ShowUpdatePrompt();
    }
  }

  ImGui::Text("OS: %s", SDL_GetPlatform());
  ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
              ImGui::GetIO().Framerate);

  ImGui::NewLine();

  if (ImGui::TreeNode("Files and Directories")) {
    ShowCopyablePath(Profile::BasePaths::RootInstallDir, "Install Directory",
                     "InstallDir");
    ShowCopyablePath(Io::GetPlatformConfigDir(), "Config Directory",
                     "ConfigDir");
    if (UserConfig::CommonSettings.LoggingToFile) {
      ShowCopyablePath(UserConfig::CommonSettings.LogFile, "Log File",
                       "LogFile");
    }
    ImGui::TreePop();
  }
  ImGui::NewLine();

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
