#include "overlay.h"

#include <imgui.h>
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <ankerl/unordered_dense.h>
#include <magic_enum/magic_enum_containers.hpp>

#include "game.h"
#include "log.h"
#include "inputsystem.h"
#include "mem.h"
#include "profile/game.h"
#include "profile/gamedefinitions.h"
#include "profile/basepaths.h"
#include "profile/scriptvars.h"
#include "userconfig.h"
#include "version.h"
#include "io/physicalfilestream.h"
#include "data/achievementsystem.h"

using namespace Impacto::Profile::ScriptVars;

namespace Impacto::Overlay {

static bool HasInit = false;
static std::optional<OverlayTab> ActiveTab;

struct ImgData {
  uint32_t Texture;
};
static ankerl::unordered_dense::map<std::string, ImgData> iconTextureMap;

static std::string GetGameDisplayName(std::string const& gameKey) {
  auto const& name = Profile::GameDefinitions.at(gameKey).Name;
  return name.empty() ? gameKey : name;
}

static void ShowPlayTime(char const* label, int totalSeconds) {
  int hours = totalSeconds / 3600;
  int minutes = (totalSeconds % 3600) / 60;
  int seconds = totalSeconds % 60;
  ImGui::Text("%s: %02d:%02d:%02d", label, hours, minutes, seconds);
}

static ImVec4 GetAccentColor(std::string const& gameKey) {
  auto gameDefIt = Profile::GameDefinitions.find(gameKey);
  uint32_t theme = gameDefIt != Profile::GameDefinitions.end()
                       ? gameDefIt->second.LauncherTheme
                       : 0xFFFFFF;
  glm::vec4 rgb = RgbIntToFloat(theme);
  return ImVec4{rgb.r, rgb.g, rgb.b, rgb.a};
}

static int PushAccentColors(std::string const& gameKey) {
  static constexpr ImGuiCol_ accentSlots[] = {
      ImGuiCol_Border,
      ImGuiCol_CheckMark,
      ImGuiCol_SliderGrab,
      ImGuiCol_Separator,
      ImGuiCol_SeparatorHovered,
      ImGuiCol_SeparatorActive,
      ImGuiCol_ResizeGrip,
      ImGuiCol_ScrollbarGrab,
      ImGuiCol_InputTextCursor,
      ImGuiCol_TabSelectedOverline,
      ImGuiCol_PlotLines,
      ImGuiCol_PlotHistogram,
      ImGuiCol_TableBorderStrong,
      ImGuiCol_DragDropTarget,
      ImGuiCol_UnsavedMarker,
      ImGuiCol_NavCursor,
  };

  ImVec4 accent = GetAccentColor(gameKey);
  for (auto slot : accentSlots) ImGui::PushStyleColor(slot, accent);
  return std::size(accentSlots);
}

void SetupStyle() {
  ImGuiStyle& style = ImGui::GetStyle();

  style.WindowRounding = 0.0f;
  style.ChildRounding = 0.0f;
  style.PopupRounding = 0.0f;
  style.FrameRounding = 0.0f;
  style.ScrollbarRounding = 0.0f;
  style.GrabRounding = 0.0f;
  style.TabRounding = 0.0f;
  style.DragDropTargetRounding = 0.0f;

  style.WindowBorderSize = 0.0f;
  style.ChildBorderSize = 1.0f;
  style.PopupBorderSize = 1.0f;
  style.FrameBorderSize = 1.0f;
  style.TabBorderSize = 1.0f;
  style.SeparatorTextBorderSize = 1.0f;

  constexpr ImVec4 white{1.0f, 1.0f, 1.0f, 1.0f};
  constexpr ImVec4 black{0.0f, 0.0f, 0.0f, 1.0f};
  constexpr ImVec4 transparent{0.0f, 0.0f, 0.0f, 0.0f};
  constexpr ImVec4 blackTranslucent{0.0f, 0.0f, 0.0f, 0.85f};
  constexpr ImVec4 grayLow{0.16f, 0.16f, 0.16f, 1.0f};
  constexpr ImVec4 grayMid{0.32f, 0.32f, 0.32f, 1.0f};
  constexpr ImVec4 grayHigh{0.55f, 0.55f, 0.55f, 1.0f};
  constexpr ImVec4 whiteFaint{1.0f, 1.0f, 1.0f, 0.35f};

  ImVec4* colors = style.Colors;
  colors[ImGuiCol_Text] = white;
  colors[ImGuiCol_TextDisabled] = grayHigh;
  colors[ImGuiCol_TextLink] = white;
  colors[ImGuiCol_TextSelectedBg] = whiteFaint;
  colors[ImGuiCol_WindowBg] = blackTranslucent;
  colors[ImGuiCol_ChildBg] = transparent;
  colors[ImGuiCol_PopupBg] = blackTranslucent;
  colors[ImGuiCol_Border] = white;
  colors[ImGuiCol_BorderShadow] = transparent;
  colors[ImGuiCol_FrameBg] = black;
  colors[ImGuiCol_FrameBgHovered] = grayLow;
  colors[ImGuiCol_FrameBgActive] = grayMid;
  colors[ImGuiCol_TitleBg] = black;
  colors[ImGuiCol_TitleBgActive] = black;
  colors[ImGuiCol_TitleBgCollapsed] = black;
  colors[ImGuiCol_MenuBarBg] = black;
  colors[ImGuiCol_ScrollbarBg] = black;
  colors[ImGuiCol_ScrollbarGrab] = white;
  colors[ImGuiCol_ScrollbarGrabHovered] = grayHigh;
  colors[ImGuiCol_ScrollbarGrabActive] = grayMid;
  colors[ImGuiCol_CheckMark] = white;
  colors[ImGuiCol_CheckboxSelectedBg] = grayLow;
  colors[ImGuiCol_SliderGrab] = white;
  colors[ImGuiCol_SliderGrabActive] = grayHigh;
  colors[ImGuiCol_Button] = black;
  colors[ImGuiCol_ButtonHovered] = grayLow;
  colors[ImGuiCol_ButtonActive] = grayMid;
  colors[ImGuiCol_Header] = grayLow;
  colors[ImGuiCol_HeaderHovered] = grayMid;
  colors[ImGuiCol_HeaderActive] = grayHigh;
  colors[ImGuiCol_Separator] = white;
  colors[ImGuiCol_SeparatorHovered] = white;
  colors[ImGuiCol_SeparatorActive] = white;
  colors[ImGuiCol_ResizeGrip] = white;
  colors[ImGuiCol_ResizeGripHovered] = grayHigh;
  colors[ImGuiCol_ResizeGripActive] = grayMid;
  colors[ImGuiCol_InputTextCursor] = white;
  colors[ImGuiCol_Tab] = black;
  colors[ImGuiCol_TabHovered] = grayMid;
  colors[ImGuiCol_TabSelected] = grayLow;
  colors[ImGuiCol_TabSelectedOverline] = white;
  colors[ImGuiCol_TabDimmed] = black;
  colors[ImGuiCol_TabDimmedSelected] = grayLow;
  colors[ImGuiCol_TabDimmedSelectedOverline] = grayHigh;
  colors[ImGuiCol_DockingPreview] = whiteFaint;
  colors[ImGuiCol_DockingEmptyBg] = black;
  colors[ImGuiCol_PlotLines] = white;
  colors[ImGuiCol_PlotLinesHovered] = grayHigh;
  colors[ImGuiCol_PlotHistogram] = white;
  colors[ImGuiCol_PlotHistogramHovered] = grayHigh;
  colors[ImGuiCol_TableHeaderBg] = black;
  colors[ImGuiCol_TableBorderStrong] = white;
  colors[ImGuiCol_TableBorderLight] = grayHigh;
  colors[ImGuiCol_TableRowBg] = black;
  colors[ImGuiCol_TableRowBgAlt] = grayLow;
  colors[ImGuiCol_TreeLines] = grayHigh;
  colors[ImGuiCol_DragDropTarget] = white;
  colors[ImGuiCol_DragDropTargetBg] = grayLow;
  colors[ImGuiCol_UnsavedMarker] = white;
  colors[ImGuiCol_NavCursor] = white;
  colors[ImGuiCol_NavWindowingHighlight] = whiteFaint;
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4{1.0f, 1.0f, 1.0f, 0.2f};
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4{1.0f, 1.0f, 1.0f, 0.2f};
}

void SetupFonts() {
  ImGuiIO& io = ImGui::GetIO();
  constexpr const char* fontPath =
      "resources/common/font/NotoSansCJKjp-Bold.otf";
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
    auto iconPath = (path("resources") / gameKey / "icondata" / "icon.png");
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
    };
    iconTextureMap.try_emplace(gameKey, img);
    delete stream;
  }
}

void Init() {
  if (HasInit) return;

  SetupStyle();
  SetupFonts();
  SetupIcons();
  HasInit = true;
}

static void ShowGamePicker(std::string& selectedGame) {
  constexpr float comboWidth = 340.0f;
  constexpr float iconSize = 36.0f;
  constexpr auto label = "Choose Game";

  std::vector<std::string> gameKeys;
  for (auto&& [key, def] : Profile::GameDefinitions) {
    using std::filesystem::path;
    if (def.Hidden) continue;
    if (Io::PathExists(
            (path(Profile::BasePaths::RootGamedataDir) / key).string()) ==
        IoError_NotFound)
      continue;
    gameKeys.push_back(key);
  }
  std::ranges::sort(gameKeys, std::ranges::less{}, [](auto const& key) {
    return Profile::GameDefinitions.at(key).LauncherOrderId;
  });

  if (ImGui::BeginTable("##ChooseGameTable", 3)) {
    ImGui::TableSetupColumn("##left", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("##mid", ImGuiTableColumnFlags_WidthFixed,
                            ImGui::CalcTextSize(label).x +
                                ImGui::GetStyle().ItemSpacing.x + comboWidth +
                                iconSize);
    ImGui::TableSetupColumn("##right", ImGuiTableColumnFlags_WidthStretch);

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(1);

    auto showIcon = [](std::string const& game) {
      if (auto iconTxtItr = iconTextureMap.find(game);
          iconTxtItr != iconTextureMap.end()) {
        auto const& img = iconTxtItr->second;
        ImGui::Image((ImTextureID)(intptr_t)img.Texture,
                     ImVec2{iconSize, iconSize});
        ImGui::SameLine();
      }
    };
    showIcon(selectedGame);
    ImGui::SetNextItemWidth(comboWidth);
    std::string previewName =
        selectedGame.empty() ? std::string() : GetGameDisplayName(selectedGame);
    if (ImGui::BeginCombo("Choose Game", previewName.c_str())) {
      for (auto&& key : gameKeys) {
        const bool isSelected = selectedGame == key;
        showIcon(key);
        if (ImGui::Selectable(GetGameDisplayName(key).c_str(), isSelected, 0,
                              ImVec2{0, iconSize})) {
          selectedGame = key;
        }
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
    UserConfig::RestoreSettingsDefaults();
  }
  ImGui::SameLine();
  if (ImGui::Button(saveConfigLabel)) {
    UserConfig::WriteUserConfig();
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
      if (ImGui::Checkbox("Karaoke", &karaOn)) {
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
    UserConfig::EnhancementsSettings =
        decltype(UserConfig::EnhancementsSettings){};
  }
  ImGui::SameLine();
  if (ImGui::Button(saveConfigLabel)) {
    UserConfig::WriteUserConfig();
  }
}

static void ShowCloseButton() {
  ImVec2 closeButtonSize(27, 27);

  ImGui::SetCursorPos(
      ImVec2(ImGui::GetContentRegionAvail().x - closeButtonSize.x, 0.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.0f));
  if (ImGui::Button("×", closeButtonSize)) {
    OverlayShown = false;
  }

  ImGui::PopStyleVar();
  ImGui::PopStyleVar();
}

static void ShowAchievementsPage() {
  using namespace Impacto::AchievementSystem;

  if (Implementation == nullptr) {
    ImGui::Spacing();
    ImGui::TextWrapped(
        "Achievements will be shown here once a game is loaded.");
    return;
  }

  const float lineHeight = ImGui::GetTextLineHeightWithSpacing();
  const float iconSize = lineHeight * 2.0f;
  const float cardHeight = iconSize + ImGui::GetStyle().WindowPadding.y * 2.0f;

  static std::unordered_set<int> peekedAchievements;

  auto showCard = [&](int id, const Achievement* ach, bool unlocked) {
    const bool hidden = ach->Hidden() && !unlocked;
    const bool peeked = peekedAchievements.contains(id);
    const bool revealed = !hidden || peeked;

    ImGui::PushID(id);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{1.0f, 1.0f, 1.0f, 1.0f});

    const ImVec2 cardMin = ImGui::GetCursorScreenPos();
    const float cardWidth = ImGui::GetContentRegionAvail().x;
    const ImVec2 cardMax{cardMin.x + cardWidth, cardMin.y + cardHeight};
    const bool cardHovered = ImGui::IsMouseHoveringRect(cardMin, cardMax);

    if (ImGui::BeginChild("##AchievementCard", ImVec2(cardWidth, cardHeight),
                          ImGuiChildFlags_Borders,
                          ImGuiWindowFlags_NoScrollbar |
                              ImGuiWindowFlags_NoScrollWithMouse)) {
      Sprite const& icon = ach->Icon();
      if (revealed && icon.Sheet.Texture != 0) {
        RectF uv = icon.NormalizedBounds();
        ImVec4 tint = unlocked ? ImVec4{1.0f, 1.0f, 1.0f, 1.0f}
                               : ImVec4{0.25f, 0.25f, 0.25f, 1.0f};
        ImGui::Image((ImTextureID)(intptr_t)icon.Sheet.Texture,
                     ImVec2{iconSize, iconSize}, ImVec2{uv.X, uv.Y},
                     ImVec2{uv.Right(), uv.Bottom()}, tint,
                     ImVec4{0.0f, 0.0f, 0.0f, 0.0f});
      } else {
        ImGui::Dummy(ImVec2{iconSize, iconSize});
      }
      ImGui::SameLine();

      ImGui::BeginGroup();
      ImGui::TextUnformatted(revealed ? ach->Name().c_str()
                                      : "Hidden achievement");

      if (revealed) {
        if (!ach->Description().empty()) {
          ImGui::TextWrapped("%s", ach->Description().c_str());
        }
      } else if (cardHovered) {
        ImGui::TextWrapped("Click to show spoiler.");
      } else {
        ImGui::TextWrapped(
            "Details for this achievement will be revealed once unlocked.");
      }
      ImGui::EndGroup();
    }
    ImGui::EndChild();

    if (hidden && cardHovered) {
      RequestCursor(CursorType::Pointer);
      if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        if (peeked) {
          peekedAchievements.erase(id);
        } else {
          peekedAchievements.insert(id);
        }
      }
    }

    ImGui::PopStyleColor();
    ImGui::PopID();

    ImGui::Spacing();
  };

  if (ImGui::BeginChild("##AchievementsContent")) {
    const size_t count = GetAchievementCount();

    for (size_t id = 0; id < count; id++) {
      const Achievement* ach = GetAchievement((int)id);
      if (ach == nullptr || !IsAchievementUnlocked((int)id)) continue;
      showCard((int)id, ach, true);
    }

    bool printedLockedHeader = false;
    for (size_t id = 0; id < count; id++) {
      const Achievement* ach = GetAchievement((int)id);
      if (ach == nullptr || IsAchievementUnlocked((int)id)) continue;
      if (!printedLockedHeader) {
        ImGui::SeparatorText("Locked Achievements");
        printedLockedHeader = true;
      }
      showCard((int)id, ach, false);
    }
  }
  ImGui::EndChild();
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
    int accentColorCount = PushAccentColors(selectedGame);

    float footerHeight = ImGui::GetStyle().ItemSpacing.y +
                         ImGui::GetStyle().SeparatorSize +
                         ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("TabRegion", ImVec2(0, -footerHeight), 0);
    ImGui::SetNextItemAllowOverlap();
    if (ImGui::BeginTabBar("MainTabs")) {
      magic_enum::containers::array<OverlayTab, ImGuiTabItemFlags> tabFlags{};

      if (RequestedTab != ActiveTab) {
        ActiveTab = RequestedTab;
        if (ActiveTab.has_value())
          tabFlags[*ActiveTab] |= ImGuiTabItemFlags_SetSelected;
      }

      if (ImGui::BeginTabItem("Settings", nullptr,
                              tabFlags[OverlayTab::Settings])) {
        ShowSettingsPage(selectedGame);
        ImGui::EndTabItem();
      }

      if (ImGui::BeginTabItem("Enhancements", nullptr,
                              tabFlags[OverlayTab::Enhancements])) {
        ShowEnhancementsPage(selectedGame);
        ImGui::EndTabItem();
      }

      if (ImGui::BeginTabItem("Achievements", nullptr,
                              tabFlags[OverlayTab::Achievements])) {
        ShowAchievementsPage();
        ImGui::EndTabItem();
      }

      if (ImGui::BeginTabItem("About", nullptr, tabFlags[OverlayTab::About])) {
        ImGui::Text("Committee of Zero");
        ImGui::SameLine();
        ImGui::TextLinkOpenURL("Technical Support",
                               "https://discord.gg/hRtvaYawg6");
        ImGui::Text("Impacto Version %d.%d.%d", VERSION_MAJOR, VERSION_MINOR,
                    VERSION_PATCH);
        ImGui::Text("OS: %s", SDL_GetPlatform());
        ImGui::Text("%.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
        if (Profile::Game::HasInit) {
          ImGui::Separator();
          ImGui::Text("%s", GetGameDisplayName(selectedGame).c_str());
          ShowPlayTime("Current Session Play Time", ScrWork[SW_PLAYTIME]);
          ShowPlayTime("Total Play Time", ScrWork[SW_TOTALPLAYTIME]);
        }
        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
    }
    if (Profile::Game::HasInit) ShowCloseButton();
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

#ifndef NDEBUG
    static bool showDemo = false;
    if (ImGui::Button("Toggle Demo")) {
      showDemo = !showDemo;
    }
    if (showDemo) ImGui::ShowDemoWindow();
#endif

    ImVec2 mouseDelta = ImGui::GetIO().MouseDelta;
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) &&
        (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)) {
      Input::CurrentInputDevice = Input::Device::Mouse;
    }

    if (ImGui::IsAnyItemHovered()) {
      RequestCursor(CursorType::Pointer);
    }

    ImGui::PopStyleColor(accentColorCount);
  }
  ImGui::End();
}

void Show() {
  if (Profile::Game::HasInit) {
    if (!OverlayShown &&
        ((Input::KeyboardButtonWentDown[SDL_SCANCODE_0] &&
          (SDL_GetModState() & SDL_KMOD_SHIFT)) ||
         Input::ControllerButtonWentDown[SDL_GAMEPAD_BUTTON_LEFT_STICK])) {
      OverlayShown = true;
    } else if (OverlayShown &&
               (ImGui::IsKeyChordPressed(ImGuiMod_Shift | ImGuiKey_0) ||
                ImGui::IsKeyPressed(ImGuiKey_GamepadL3, false))) {
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
