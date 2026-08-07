#include "overlay.h"

#include <imgui.h>

#include "game.h"
#include "log.h"
#include "inputsystem.h"

namespace Impacto::Overlay {

void SetupFonts() {
  ImGuiIO& io = ImGui::GetIO();
  constexpr const char* fontPath = "assets/NotoSansCJKjp-Regular.otf";
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
  if (ImGui::Begin("Overlay##DockArea", &OverlayShown, windowFlags)) {
    if (ImGui::BeginTabBar("MainTabs")) {
      if (ImGui::BeginTabItem("Settings")) {
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

    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
  }
  ImGui::End();

  if (!OverlayShown) {
    // Hide other tabs.
  }
}

void Show() {
  if ((!OverlayShown && (Input::KeyboardButtonWentDown[SDL_SCANCODE_TAB] &&
                         (SDL_GetModState() & KMOD_SHIFT))) ||
      (Input::ControllerButtonWentDown[SDL_CONTROLLER_BUTTON_GUIDE])) {
    OverlayShown = true;
  }
  if (OverlayShown && ImGui::IsKeyChordPressed(ImGuiMod_Shift | ImGuiKey_Tab)) {
    OverlayShown = false;
  }

  if (OverlayShown) {
    ShowOverlay();
  }
}

}  // namespace Impacto::Overlay
