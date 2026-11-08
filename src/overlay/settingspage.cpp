#include "settingspage.h"

#include <imgui.h>
#include <imgui_scroll_drag.h>

#include "../userconfig.h"
#include "../profile/game.h"
#include "../profile/gamedefinitions.h"

namespace Impacto::Overlay {

static void ShowCommonSettings() {
  const float comboWidth = 200.0f * Window->DpiScale;

  if (ImGui::CollapsingHeader("General Settings",
                              ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Spacing();

    auto& commonSettings = UserConfig::CommonSettings;
    ImGui::Checkbox("Swap Confirm/Cancel Buttons",
                    &commonSettings.SwapConfirmCancelButtons);
    ImGui::Checkbox("Log to Console", &commonSettings.LoggingToConsole);
    if (ImGui::Checkbox("Log to File", &commonSettings.LoggingToFile)) {
      LogInitFile();
    }
    if (ImGui::TreeNode("Log Channels")) {
      for (auto&& [channelValue, channelName] :
           magic_enum::enum_entries<LogChannel>()) {
        bool isSelected = magic_enum::enum_flags_test(
            commonSettings.LogChannels, channelValue);
        ImGui::Checkbox(channelName.data(), &isSelected);
        if (isSelected) commonSettings.LogChannels |= channelValue;
        if (!isSelected) commonSettings.LogChannels &= ~channelValue;
      }
      ImGui::TreePop();
    }

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Log Level");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(comboWidth);
    if (ImGui::BeginCombo(
            "##LogLevel",
            magic_enum::enum_name(commonSettings.LogLvl).data())) {
      for (auto&& logLevel : magic_enum::enum_values<LogLevel>()) {
        const bool isSelected = logLevel == commonSettings.LogLvl;
        if (ImGui::Selectable(magic_enum::enum_name(logLevel).data(),
                              isSelected)) {
          commonSettings.LogLvl = logLevel;
        }
        if (isSelected) ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }
  }
}

static void ShowControllerAudioSettings() {
  if (ImGui::CollapsingHeader("DualShock 4 Audio",
                              ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::SameLine();
    ImGui::Text(
        "(Played on headphones plugged into a DualShock 4 connected by USB)");
    ImGui::Spacing();

    auto& controllerAudio = UserConfig::CommonSettings.ControllerAudio;
    ImGui::Checkbox("Music", &controllerAudio.BGM);
    ImGui::Checkbox("Voices", &controllerAudio.Voice);
    ImGui::Checkbox("Sound Effects", &controllerAudio.SE);
    ImGui::Checkbox("System Sound Effects", &controllerAudio.SSE);
    ImGui::Checkbox("Movies", &controllerAudio.Movie);
  }
}

[[maybe_unused]] static bool ShowDisplaySettings(
    std::string const& selectedGame) {
  auto& gameSettings = UserConfig::GameSettings.at(selectedGame);

  const float comboWidth = 200.0f * Window->DpiScale;

  constexpr static auto resolutionOptions = std::to_array({
      std::pair{"1024x576", glm::ivec2{1024, 576}},
      std::pair{"1280x720", glm::ivec2{1280, 720}},
      std::pair{"1366x768", glm::ivec2{1366, 768}},
      std::pair{"1600x900", glm::ivec2{1600, 900}},
      std::pair{"1920x1080", glm::ivec2{1920, 1080}},
      std::pair{"2560x1440", glm::ivec2{2560, 1440}},
      std::pair{"3840x2160", glm::ivec2{3840, 2160}},
  });

  bool wasUpdated = false;
  ImGui::Spacing();
  if (ImGui::CollapsingHeader("Display Settings",
                              ImGuiTreeNodeFlags_DefaultOpen)) {
    static std::string currentResolution;

    static std::optional<int> lastResWidth;
    static std::optional<int> lastResHeight;

    if (currentResolution.empty() ||
        lastResWidth != gameSettings.ResolutionWidth ||
        lastResHeight != gameSettings.ResolutionHeight) {
      if (gameSettings.ResolutionWidth && gameSettings.ResolutionHeight) {
        currentResolution = fmt::format("{}x{}", *gameSettings.ResolutionWidth,
                                        *gameSettings.ResolutionHeight);
      } else {
        currentResolution = "Native Game Res";
      }
      wasUpdated = true;
      lastResWidth = gameSettings.ResolutionWidth;
      lastResHeight = gameSettings.ResolutionHeight;
    }

    SDL_DisplayMode maxRes{};
    if (const SDL_DisplayMode* desktopMode =
            SDL_GetDesktopDisplayMode(SDL_GetPrimaryDisplay())) {
      maxRes = *desktopMode;
    }

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Game Resolution");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(comboWidth);
    if (ImGui::BeginCombo("##ChooseResolution", currentResolution.c_str())) {
      bool isNativeRes =
          !gameSettings.ResolutionWidth || !gameSettings.ResolutionHeight;
      if (ImGui::Selectable("Native Game Res", isNativeRes)) {
        gameSettings.ResolutionWidth.reset();
        gameSettings.ResolutionHeight.reset();
      }
      if (isNativeRes) ImGui::SetItemDefaultFocus();
      for (auto&& [display, value] : resolutionOptions) {
        if (value.x > maxRes.w || value.y > maxRes.h) continue;
        bool isSelected = gameSettings.ResolutionWidth == value.x &&
                          gameSettings.ResolutionHeight == value.y;
        if (ImGui::Selectable(display, isSelected)) {
          gameSettings.ResolutionWidth = value.x;
          gameSettings.ResolutionHeight = value.y;
        }
        if (isSelected) ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }

    ImGui::SameLine();

    auto dispModeRadio = [&](char const* const label, DisplayMode mode) {
      if (ImGui::RadioButton(label, gameSettings.Display == mode)) {
        wasUpdated = true;
        gameSettings.Display = mode;
      }
    };

    dispModeRadio("Windowed", DisplayMode::Windowed);
    ImGui::SameLine();
    dispModeRadio("Fullscreen", DisplayMode::Fullscreen);
    ImGui::SameLine();
    dispModeRadio("Borderless", DisplayMode::Borderless);

    if (ImGui::Checkbox("Vertical Synchronization", &gameSettings.UseVSync)) {
      wasUpdated = true;
    }
  }

  return wasUpdated;
}

static void ShowPatchSettings(std::string const& selectedGame) {
  const float comboWidth = 200.0f * Window->DpiScale;

  auto& gameSettings = UserConfig::GameSettings.at(selectedGame);
  auto const& gameDef = Profile::GameDefinitions.at(selectedGame);

  if (ImGui::CollapsingHeader("Patch Settings",
                              ImGuiTreeNodeFlags_DefaultOpen)) {
    if (Profile::Game::HasInit) {
      ImGui::SameLine();
      ImGui::Text("(Will apply on next launch)");
    }
    ImGui::Spacing();

    ImGui::Checkbox("Use Patch", &gameSettings.UsePatch);

    ImGui::SameLine();
    ImGui::SetNextItemWidth(comboWidth);

    ImGui::BeginDisabled(!gameSettings.UsePatch);
    if (ImGui::BeginCombo("Patch Profile", gameSettings.PatchProfile.c_str())) {
      for (auto&& [patchProfile, _] : gameDef.Patch) {
        const bool isSelected = patchProfile == gameSettings.PatchProfile;
        if (ImGui::Selectable(patchProfile.c_str(), isSelected)) {
          gameSettings.PatchProfile = patchProfile;
        }
        if (isSelected) ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }
    ImGui::EndDisabled();
  }
}

void ShowSettingsPage(std::string const& selectedGame) {
  const float footerHeight = ImGui::GetStyle().ItemSpacing.y +
                             ImGui::GetStyle().SeparatorSize +
                             ImGui::GetFrameHeightWithSpacing();

  static bool displayChanged = false;
  if (ImGui::BeginChild("##SettingsContent", ImVec2(0, -footerHeight))) {
    ShowCommonSettings();
    ImGui::Spacing();
    ShowControllerAudioSettings();
    ImGui::Spacing();
    if (!selectedGame.empty()) {
#if !defined(__ANDROID__) && !defined(__SWITCH__)
      displayChanged |= ShowDisplaySettings(selectedGame);
      ImGui::Spacing();
#endif

      ShowPatchSettings(selectedGame);
    }
    ImGui::ScrollWhenDraggingOnVoid(ImVec2{0.0f, -ImGui::GetIO().MouseDelta.y},
                                    ImGuiMouseButton_Left);
  }
  ImGui::EndChild();

  constexpr auto restoreDefaultsLabel = "Restore Defaults";
  constexpr auto saveConfigLabel = "Save Config";
  constexpr auto applyDisplayLabel = "Apply Display Changes";
  float buttonsWidth = ImGui::CalcTextSize(restoreDefaultsLabel).x +
                       ImGui::CalcTextSize(saveConfigLabel).x +
                       ImGui::GetStyle().FramePadding.x * 4 +
                       ImGui::GetStyle().ItemSpacing.x;

  if (displayChanged && Profile::Game::HasInit) {
    buttonsWidth += ImGui::CalcTextSize(applyDisplayLabel).x +
                    ImGui::GetStyle().FramePadding.x * 2 +
                    ImGui::GetStyle().ItemSpacing.x;
  }

  ImGui::SetCursorPosX(ImGui::GetCursorPosX() +
                       ImGui::GetContentRegionAvail().x - buttonsWidth);
  if (displayChanged && Profile::Game::HasInit) {
    if (ImGui::Button(applyDisplayLabel)) {
      Window->ApplyWindowSettings();
      displayChanged = false;
    }
    ImGui::SameLine();
  }
  if (ImGui::Button(restoreDefaultsLabel)) {
    UserConfig::RestoreSettingsDefaults();
  }
  ImGui::SameLine();
  if (ImGui::Button(saveConfigLabel)) {
    UserConfig::WriteUserConfig();
  }
}

}  // namespace Impacto::Overlay
