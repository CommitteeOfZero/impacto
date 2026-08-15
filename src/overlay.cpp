#include "overlay.h"

#include <imgui.h>

#include "game.h"
#include "log.h"
#include "inputsystem.h"
#include "profile/game.h"
#include "profile/gamedefinitions.h"
#include "userconfig.h"

namespace Impacto::Overlay {

void SetupFonts() {
  ImGuiIO& io = ImGui::GetIO();
  constexpr const char* fontPath =
      "resources/common/font/NotoSansCJKjp-Regular.otf";
  ImFont* font = io.Fonts->AddFontFromFileTTF(
      fontPath, 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
  if (font == nullptr) {
    ImpLog(LogLevel::Error, LogChannel::Overlay, "Failed to load font: {}",
           fontPath);
  }
}

void Init() { SetupFonts(); }

void ShowOverlay() {
  constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar |
                                           ImGuiWindowFlags_NoMove |
                                           ImGuiWindowFlags_NoCollapse;
  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);
  static const char* selectedGame = nullptr;
  if (ImGui::Begin("Overlay##DockArea", &OverlayShown, windowFlags)) {
    if (!Profile::Game::HasInit) {  // Game selection
      ImGui::BeginGroup();
      bool isSelected = false;
      bool isOpening = ImGui::IsWindowAppearing();

      {
        float windowWidth = ImGui::GetContentRegionAvail().x;
        constexpr static float comboWidth = 200.0f;

        ImGui::SetCursorPosX((windowWidth - comboWidth) * 0.5f);
        ImGui::SetNextItemWidth(comboWidth);
      }

      if (ImGui::BeginCombo("Choose Game", selectedGame)) {
        for (auto&& [gameKey, gameDef] : Profile::GameDefinitions) {
          if (gameDef.Hidden) continue;
          if (isOpening) {
            selectedGame = gameKey.c_str();
            isOpening = false;
          }
          isSelected = selectedGame == gameKey.c_str();
          if (ImGui::Selectable(gameKey.c_str(), isSelected))
            selectedGame = gameKey.c_str();
          if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }

      if (selectedGame) {
        float windowWidth = ImGui::GetContentRegionAvail().x;
        constexpr static float btnWidth = 200.0f;

        ImGui::SetCursorPosX((windowWidth - btnWidth) * 0.5f);
        ImGui::SetNextItemWidth(btnWidth);

        if (ImGui::Button("Start Game", ImVec2(80, 20))) {
          UserConfig::SetActiveGame(selectedGame);
          OverlayShown = false;
        }
      }
      ImGui::EndGroup();
    } else {
      selectedGame = UserConfig::GetActiveGame().c_str();
    }

    if (ImGui::BeginTabBar("MainTabs")) {
      if (ImGui::BeginTabItem("About")) {
        ImGui::Text("%.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
        ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("Settings")) {
        if (selectedGame == nullptr) {
          ImGui::Text("Please select a game first.");
        }
        ImGui::EndTabItem();
      }

      if (ImGui::BeginTabItem("Enhancements")) {
        ImGui::EndTabItem();
      }

      if (ImGui::BeginTabItem("Achievements")) {
        ImGui::EndTabItem();
      }

      ImGui::EndTabBar();
    }
  }
  ImGui::End();
}

void Show() {
  if (Profile::Game::HasInit) {
    if ((!OverlayShown && (Input::KeyboardButtonWentDown[SDL_SCANCODE_TAB] &&
                           (SDL_GetModState() & KMOD_SHIFT))) ||
        (Input::ControllerButtonWentDown[SDL_CONTROLLER_BUTTON_GUIDE])) {
      OverlayShown = true;
    }
    if (OverlayShown &&
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
