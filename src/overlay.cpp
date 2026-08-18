#include "overlay.h"

#include <imgui.h>
#include <charconv>
#include <ankerl/unordered_dense.h>

#include "game.h"
#include "log.h"
#include "inputsystem.h"
#include "profile/game.h"
#include "profile/gamedefinitions.h"
#include "profile/basepaths.h"
#include "userconfig.h"
#include "version.h"

namespace Impacto::Overlay {

static bool HasInit = false;

struct ImgData {
  uint32_t Texture;
  uint32_t Width;
  uint32_t Height;
};
static ankerl::unordered_dense::map<std::string, ImgData> iconTextureMap;

void SetupFonts() {
  ImGuiIO& io = ImGui::GetIO();
  constexpr const char* fontPath =
      "resources/common/font/NotoSansCJKjp-Regular.otf";
  ImFont* font = io.Fonts->AddFontFromFileTTF(
      fontPath, 24.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
  if (font == nullptr) {
    ImpLog(LogLevel::Error, LogChannel::Overlay, "Failed to load font: {}",
           fontPath);
  }
}

void SetupIcons() {
  for (auto&& [gameKey, gameDef] : Profile::GameDefinitions) {
    using std::filesystem::path;
    auto iconPath = (path("resources") / gameKey / "icons" / "icon.png");
    if (Io::PathExists(iconPath.string()) == IoError_NotFound) continue;

    Io::Stream* stream;
    if (Io::PhysicalFileStream::Create(iconPath.string(), &stream) !=
        IoError_OK) {
      ImpLog(LogLevel::Error, LogChannel::Overlay, "Failed to open icon: {}",
             iconPath);
      continue;
    };
    Texture t;
    t.Load(stream);
    ImgData img{
        .Texture = t.Submit(),
        .Width = t.Width,
        .Height = t.Height,
    };
    iconTextureMap.try_emplace(gameKey, img);
    delete stream;
  }
}

void Init() {
  if (HasInit) return;

  SetupFonts();
  SetupIcons();
  HasInit = true;
}

static void ShowGamePicker(std::string& selectedGame) {
  constexpr float comboWidth = 200.0f;
  constexpr auto label = "Choose Game";
  if (ImGui::BeginTable("##ChooseGameTable", 3)) {
    ImGui::TableSetupColumn("##left", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("##mid", ImGuiTableColumnFlags_WidthFixed,
                            ImGui::CalcTextSize(label).x +
                                ImGui::GetStyle().ItemSpacing.x + comboWidth);
    ImGui::TableSetupColumn("##right", ImGuiTableColumnFlags_WidthStretch);

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(1);

    ImGui::AlignTextToFramePadding();
    ImGui::Text(label);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(comboWidth);

    bool isSelected = false;
    bool isOpening = ImGui::IsWindowAppearing();

    if (ImGui::BeginCombo("##ChooseGame", selectedGame.c_str())) {
      for (auto&& [gameKey, gameDef] : Profile::GameDefinitions) {
        using std::filesystem::path;
        if (gameDef.Hidden) continue;
        if (Io::PathExists((path(Profile::BasePaths::RootGamedataDir) / gameKey)
                               .string()) == IoError_NotFound)
          continue;
        if (isOpening) {
          selectedGame = gameKey;
          isOpening = false;
        }
        isSelected = selectedGame == gameKey;

        if (auto iconTxtItr = iconTextureMap.find(gameKey);
            iconTxtItr != iconTextureMap.end()) {
          ImGui::Image(
              (ImTextureID)(intptr_t)iconTxtItr->second.Texture,
              ImVec2{iconTxtItr->second.Width, iconTxtItr->second.Height});
          ImGui::SameLine();
        }
        if (ImGui::Selectable(gameKey.c_str(), isSelected))
          selectedGame = gameKey;
        if (isSelected) ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }

    ImGui::EndTable();
  }

  ImGui::Separator();
  ImGui::Spacing();
}

static bool ShowDisplaySettings(std::string const& selectedGame) {
  auto& gameSettings = UserConfig::GameSettings.at(selectedGame);

  constexpr float comboWidth = 200.0f;

  constexpr static auto resolutionOptions = std::to_array({
      std::pair{"1024x576", glm::ivec2{1024, 576}},
      std::pair{"1280x720", glm::ivec2{1280, 720}},
      std::pair{"1366x768", glm::ivec2{1366, 768}},
      std::pair{"1600x900", glm::ivec2{1600, 900}},
      std::pair{"1920x1080", glm::ivec2{1920, 1080}},
      std::pair{"2560x1440", glm::ivec2{2560, 1440}},
      std::pair{"3840x2160", glm::ivec2{3840, 2160}},
  });

  static bool updatedResolution = false;
  bool wasUpdated = false;
  ImGui::Spacing();
  if (ImGui::CollapsingHeader("Display Settings",
                              ImGuiTreeNodeFlags_DefaultOpen)) {
    static std::string currentResolution;

    if (currentResolution.empty() || updatedResolution) {
      if (gameSettings.ResolutionWidth && gameSettings.ResolutionHeight) {
        currentResolution = fmt::format("{}x{}", *gameSettings.ResolutionWidth,
                                        *gameSettings.ResolutionHeight);
      } else {
        currentResolution = "Native Game Res";
      }
      wasUpdated = true;
      updatedResolution = false;
    }

    SDL_DisplayMode maxRes{};
    SDL_GetDisplayMode(0, 0, &maxRes);

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Game Resolution");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(comboWidth);
    if (ImGui::BeginCombo("##ChooseResolution", currentResolution.c_str())) {
      ImGui::Selectable("Native Game Res", !gameSettings.ResolutionWidth ||
                                               !gameSettings.ResolutionHeight);
      for (auto&& [display, value] : resolutionOptions) {
        if (value.x > maxRes.w || value.y > maxRes.h) continue;
        bool isSelected = gameSettings.ResolutionWidth == value.x &&
                          gameSettings.ResolutionHeight == value.y;
        if (ImGui::Selectable(display, isSelected)) {
          updatedResolution = true;
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
  }

  return wasUpdated;
}

static void ShowPatchSettings(std::string const& selectedGame) {
  constexpr float comboWidth = 200.0f;

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

static void ShowCommonSettings() {
  constexpr float comboWidth = 200.0f;

  if (ImGui::CollapsingHeader("General Settings",
                              ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::Spacing();

    auto& commonSettings = UserConfig::CommonSettings;
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

static void ShowSettingsPage(std::string const& selectedGame) {
  const float footerHeight = ImGui::GetStyle().ItemSpacing.y +
                             ImGui::GetStyle().SeparatorSize +
                             ImGui::GetFrameHeightWithSpacing();

  static bool displayChanged = false;
  if (ImGui::BeginChild("##SettingsContent", ImVec2(0, -footerHeight))) {
    ShowCommonSettings();
    ImGui::Spacing();
    if (!selectedGame.empty()) {
      displayChanged |= ShowDisplaySettings(selectedGame);

      ImGui::Spacing();

      ShowPatchSettings(selectedGame);
    }
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
  }
  ImGui::SameLine();
  if (ImGui::Button(saveConfigLabel)) {
  }
}

static void ShowEnhancementsPage(std::string const& selectedGame) {
  const float footerHeight = ImGui::GetStyle().ItemSpacing.y +
                             ImGui::GetStyle().SeparatorSize +
                             ImGui::GetFrameHeightWithSpacing();
  auto& enhancements = UserConfig::EnhancementsSettings;

  if (ImGui::BeginChild("##EnhancementsContent", ImVec2(0, -footerHeight))) {
    ImGui::Spacing();

    if (ImGui::TreeNode("Subtitle Config")) {
      using enum Profile::Subtitle::SubtitleConfigType;
      bool tlOn =
          magic_enum::enum_flags_test(enhancements.SubtitleConfig, Translation);
      if (ImGui::Checkbox("Translation", &tlOn)) {
        if (tlOn) enhancements.SubtitleConfig |= Translation;
        if (!tlOn) enhancements.SubtitleConfig &= ~Translation;
      }

      bool karaOn =
          magic_enum::enum_flags_test(enhancements.SubtitleConfig, Karaoke);
      if (ImGui::Checkbox("Karaoke", &tlOn)) {
        if (karaOn) enhancements.SubtitleConfig |= Karaoke;
        if (!karaOn) enhancements.SubtitleConfig &= ~Karaoke;
      }

      ImGui::TreePop();
    }

    ImGui::Checkbox("Close Backlog on Scroll End",
                    &enhancements.CloseBacklogWhenReachedEnd);

    if (selectedGame == "cclcc") {
      ImGui::Checkbox("Delusion Trigger Mouse Support",
                      &enhancements.CCLCC.DelusionMousePatch);
    } else if (selectedGame == "chlcc") {
      ImGui::Checkbox("Delusion Trigger Mouse Support",
                      &enhancements.CHLCC.DelusionMousePatch);
    }
  }
  ImGui::EndChild();

  constexpr auto restoreDefaultsLabel = "Restore Defaults";
  constexpr auto saveConfigLabel = "Save Config";
  float buttonsWidth = ImGui::CalcTextSize(restoreDefaultsLabel).x +
                       ImGui::CalcTextSize(saveConfigLabel).x +
                       ImGui::GetStyle().FramePadding.x * 4 +
                       ImGui::GetStyle().ItemSpacing.x;

  ImGui::SetCursorPosX(ImGui::GetCursorPosX() +
                       ImGui::GetContentRegionAvail().x - buttonsWidth);
  if (ImGui::Button(restoreDefaultsLabel)) {
  }
  ImGui::SameLine();
  if (ImGui::Button(saveConfigLabel)) {
  }
}

void ShowOverlay() {
  constexpr ImGuiWindowFlags windowFlags =
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoScrollbar;
  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);

  static std::string selectedGame;
  if (ImGui::Begin("Overlay##DockArea", &OverlayShown, windowFlags)) {
    if (!Profile::Game::HasInit) {  // Game selection
      ShowGamePicker(selectedGame);
    } else {
      selectedGame = UserConfig::GetActiveGame();
    }

    const bool showStart = !Profile::Game::HasInit && !selectedGame.empty();

    float footerHeight = ImGui::GetStyle().ItemSpacing.y +
                         ImGui::GetStyle().SeparatorSize +
                         ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("TabRegion",
                      ImVec2(0, -footerHeight * (showStart ? 2 : 1)), 0);
    if (ImGui::BeginTabBar("MainTabs")) {
      if (ImGui::BeginTabItem("About")) {
        ImGui::Text("Committee of Zero");
        ImGui::SameLine();
        ImGui::TextLinkOpenURL("Technical Support",
                               "https://discord.com/invite/rq4GGCh");
        ImGui::Text("Impacto Version %d.%d.%d", VERSION_MAJOR, VERSION_MINOR,
                    VERSION_PATCH);
        ImGui::Text("OS: %s", SDL_GetPlatform());
        ImGui::Text("%.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
        ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("Settings")) {
        ShowSettingsPage(selectedGame);
        ImGui::EndTabItem();
      }

      if (ImGui::BeginTabItem("Enhancements")) {
        ShowEnhancementsPage(selectedGame);
        ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("Achievements")) {
        ImGui::TextWrapped("List achievements here.");
        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
    }
    ImGui::EndChild();

    if (!Profile::Game::HasInit && !selectedGame.empty()) {
      ImGui::Separator();
      float buttonWidth = 120.0f;
      if (ImGui::BeginTable("StartButtonCenterTable", 3)) {
        ImGui::TableSetupColumn("##left", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("##mid", ImGuiTableColumnFlags_WidthFixed,
                                buttonWidth);
        ImGui::TableSetupColumn("##right", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(1);
        if (ImGui::Button("Start Game", ImVec2(buttonWidth, 0))) {
          UserConfig::SetActiveGame(selectedGame);
          OverlayShown = false;
        }

        ImGui::EndTable();
      }
    }

    static bool showDemo = false;
    if (ImGui::Button("Toggle Demo")) {
      showDemo = !showDemo;
    }
    if (showDemo) ImGui::ShowDemoWindow();
  }
  ImGui::End();
}

void Show() {
  if (Profile::Game::HasInit) {
    if ((!OverlayShown && (Input::KeyboardButtonWentDown[SDL_SCANCODE_TAB] &&
                           (SDL_GetModState() & KMOD_SHIFT))) ||
        (Input::ControllerButtonWentDown[SDL_CONTROLLER_BUTTON_GUIDE])) {
      OverlayShown = true;
    } else if (OverlayShown &&
               ImGui::IsKeyChordPressed(ImGuiMod_Shift | ImGuiKey_Tab)) {
      OverlayShown = false;
    }
  } else {
    OverlayShown = true;
  }

  if (OverlayShown) {
    ShowOverlay();
  }
}

}  // namespace Impacto::Overlay
