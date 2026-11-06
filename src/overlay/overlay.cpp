#include "overlay.h"

#include <imgui.h>
#include <algorithm>
#include <vector>
#include <ankerl/unordered_dense.h>
#include <magic_enum/magic_enum_containers.hpp>

#ifdef __ANDROID__
#include <jni.h>

#include "../profile/profile.h"
#endif

#include "enhancementspage.h"
#include "settingspage.h"

#include "../game.h"
#include "../log.h"
#include "../inputsystem.h"
#include "../mem.h"
#include "../profile/game.h"
#include "../profile/gamedefinitions.h"
#include "../profile/basepaths.h"
#include "../profile/scriptvars.h"
#include "../profile/patch.h"
#include "../userconfig.h"
#include "version.h"
#include "../io/physicalfilestream.h"
#include "../io/filemeta.h"
#include "../data/achievementsystem.h"

using namespace Impacto::Profile::ScriptVars;

namespace Impacto::Overlay {

static bool HasInit = false;
static std::optional<OverlayTab> ActiveTab;

struct ImgData {
  uint32_t Texture;
};
static ankerl::unordered_dense::map<std::string, ImgData> iconTextureMap;

#ifdef __ANDROID__
static void ShowDirectoryPicker() {
  JNIEnv* env = (JNIEnv*)SDL_GetAndroidJNIEnv();
  jobject activity = (jobject)SDL_GetAndroidActivity();
  jclass clazz = env->GetObjectClass(activity);

  jmethodID chooseDir = env->GetMethodID(clazz, "chooseDirectory", "()V");
  if (chooseDir) {
    env->CallVoidMethod(activity, chooseDir);
  } else {
    env->ExceptionDescribe();
    env->ExceptionClear();
    throw std::runtime_error("Failed to call JNI method chooseDirectory");
  }

  env->DeleteLocalRef(activity);
  env->DeleteLocalRef(clazz);
}

static void ShowDirectoryPickerButton() {
  static std::string lastKnownDir;
  auto const& directory = Io::GetAndroidChosenDir();

  if (directory != lastKnownDir) {
    lastKnownDir = directory;
    Profile::Configure();
    UserConfig::CommonSettings.LogFile = GetDefaultLogFile();
    LogInitFile();

  } else if (lastKnownDir.empty()) {
    lastKnownDir = directory;
  }

  if (ImGui::Button("Default Directory")) {
    Io::ResetAndroidChosenDir();
  }
  ImGui::SameLine();
  if (ImGui::Button("Choose Directory")) {
    ShowDirectoryPicker();
  }
  ImGui::SameLine();
  ImGui::Text("%s", directory.c_str());

  ImGui::Separator();
  ImGui::Spacing();
}
#endif

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
  style.ScaleAllSizes(Window->DpiScale);

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
  ImFont* font =
      io.Fonts->AddFontFromFileTTF(fontPath, 24.0f * Window->DpiScale, nullptr,
                                   io.Fonts->GetGlyphRangesJapanese());
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
  const float comboWidth = 340.0f * Window->DpiScale;
  const float iconSize = 36.0f * Window->DpiScale;
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

    auto showIcon = [iconSize](std::string const& game) {
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

static void ShowCloseButton() {
  ImVec2 closeButtonSize(27 * Window->DpiScale, 27 * Window->DpiScale);

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

  const float lineHeight = ImGui::GetTextLineHeightWithSpacing();
  const float iconSize = lineHeight * 2.0f;
  const float cardHeight = iconSize + ImGui::GetStyle().WindowPadding.y * 2.0f;

  static ankerl::unordered_dense::set<int> peekedAchievements;

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
      Sprite const& icon = unlocked ? ach->Icon() : ach->LockedIcon();
      if (icon.Sheet.Texture != 0) {
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

    if (unlocked) {
      tm const unlockDate = GetAchievementUnlockDate(id);
      std::string dateText = fmt::format(
          "{} {:%H:%M:%S}",
          fmt::format(
              fmt::runtime(Profile::Patch::DateFormat.FormattedString()),
              unlockDate),
          unlockDate);
      const ImVec2 padding = ImGui::GetStyle().WindowPadding;
      const ImVec2 dateSize = ImGui::CalcTextSize(dateText.c_str());
      const ImVec2 datePos{cardMax.x - dateSize.x - padding.x,
                           cardMin.y + padding.y};
      ImGui::GetWindowDrawList()->AddText(
          datePos, ImGui::GetColorU32(ImGuiCol_Text), dateText.c_str());
    }

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
  constexpr ImGuiWindowFlags wrapperWindowFlags =
      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration |
      ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoSavedSettings;
  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  if (ImGui::Begin("WindowWrapper", &OverlayShown, wrapperWindowFlags)) {
    constexpr ImGuiWindowFlags innerWindowFlags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;

    ImVec2 windowPos = viewport->WorkPos;
    ImVec2 windowSize = viewport->WorkSize;

    if (SDL_Rect windowSafeBounds;
        SDL_GetWindowSafeArea(Window->SDLWindow, &windowSafeBounds)) {
      windowPos.x = static_cast<float>(windowSafeBounds.x);
      windowPos.y = static_cast<float>(windowSafeBounds.y);
      windowSize.x = static_cast<float>(windowSafeBounds.w);
      windowSize.y = static_cast<float>(windowSafeBounds.h);
    } else {
      ImpLog(LogLevel::Error, LogChannel::Overlay,
             "Failed to get window safe area, {}", SDL_GetError());
    }
    ImGui::SetCursorScreenPos(windowPos);

    static std::string selectedGame;
    if (ImGui::BeginChild("Overlay##DockArea", windowSize, ImGuiChildFlags_None,
                          innerWindowFlags)) {
      bool isReady = true;
#ifdef __ANDROID__
      if (!Profile::Game::HasInit) ShowDirectoryPickerButton();
      isReady = !Io::GetAndroidChosenDir().empty();
#endif

      if (isReady) {
        if (!Profile::Game::HasInit) {  // Game selection
          ShowGamePicker(selectedGame);
        } else {
          selectedGame = UserConfig::GetActiveGame();
        }
      }
      int accentColorCount = PushAccentColors(selectedGame);

      static float cachedFooterHeight = ImGui::GetStyle().ItemSpacing.y +
                                        ImGui::GetStyle().SeparatorSize +
                                        ImGui::GetFrameHeightWithSpacing();
      ImGui::BeginChild("TabRegion", ImVec2(0, -cachedFooterHeight), 0);
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

        if (!selectedGame.empty() &&
            Impacto::AchievementSystem::Implementation != nullptr &&
            ImGui::BeginTabItem("Achievements", nullptr,
                                tabFlags[OverlayTab::Achievements])) {
          ShowAchievementsPage();
          ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("About", nullptr,
                                tabFlags[OverlayTab::About])) {
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

      const float footerStartY = ImGui::GetCursorScreenPos().y;
      if (!Profile::Game::HasInit && !selectedGame.empty()) {
        ImGui::Separator();
        float buttonWidth = 120.0f * Window->DpiScale;
        if (ImGui::BeginTable("StartButtonCenterTable", 3)) {
          ImGui::TableSetupColumn("##left", ImGuiTableColumnFlags_WidthStretch);
          ImGui::TableSetupColumn("##mid", ImGuiTableColumnFlags_WidthFixed,
                                  buttonWidth);
          ImGui::TableSetupColumn("##right",
                                  ImGuiTableColumnFlags_WidthStretch);

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
      const float footerEndY = ImGui::GetCursorScreenPos().y;
      cachedFooterHeight = footerEndY - footerStartY;

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
    ImGui::EndChild();
  }
  ImGui::PopStyleVar();
  ImGui::End();
}

void Show() {
  ImGui::GetIO().ConfigNavSwapGamepadButtons = Input::FaceButtonsFlipped();

  if (Profile::Game::HasInit) {
    if (!OverlayShown &&
        ((Input::KeyboardButtonWentDown[SDL_SCANCODE_0] &&
          (SDL_GetModState() & SDL_KMOD_SHIFT)) ||
         Input::ControllerButtonWentDown[SDL_GAMEPAD_BUTTON_LEFT_STICK] ||
         Input::TouchTapCount == 3)) {
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
