#include "debugmenu.h"

#include "imgui.h"

#include "mem.h"
#include "inputsystem.h"

namespace Impacto {
namespace DebugMenu {

bool DebugMenuShown = false;
bool DebugMenuMinimized = true;

static int ScrWorkNumberFormat = 0;
static int ScrWorkIndexStart = 0;
static int ScrWorkIndexEnd = 0;
static int FlagWorkIndexStart = 0;
static int FlagWorkIndexEnd = 0;

void Show() {
  if (((Input::KeyboardButtonIsDown[SDL_SCANCODE_LCTRL] &&
        Input::KeyboardButtonWentDown[SDL_SCANCODE_D]) ||
       (Input::ControllerButtonWentDown[SDL_CONTROLLER_BUTTON_RIGHTSHOULDER] &&
        Input::ControllerButtonWentDown[SDL_CONTROLLER_BUTTON_Y])) &&
      !DebugMenuShown)
    DebugMenuShown = true;

  DebugMenuMinimized = true;
  if (DebugMenuShown) {
    if (ImGui::Begin("Debug Menu", &DebugMenuShown)) {
      DebugMenuMinimized = false;

      ImGui::Text("%.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

      if (ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_None)) {
        if (ImGui::BeginTabItem("\"Debug Editer\"")) {
          ShowScriptVariablesEditor();
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Objects (BG, Models, etc.)")) {
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("UI")) {
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Script Debugger (?)")) {
          ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
      }
    }

    ImGui::End();
  }
}

void ShowScriptVariablesEditor() {
  ImGui::PushItemWidth(10.0f * ImGui::GetFontSize());

  if (ImGui::TreeNode("ScrWork Config")) {
    ImGui::Text("Display number format");
    ImGui::SameLine();
    ImGui::RadioButton("DEC", &ScrWorkNumberFormat, 0);
    ImGui::SameLine();
    ImGui::RadioButton("HEX", &ScrWorkNumberFormat, 1);

    ImGui::PushButtonRepeat(true);
    ImGui::Spacing();
    ImGui::Text("Start index");
    ImGui::DragInt("##ScrWorkStartIndex", &ScrWorkIndexStart, 0.5f, 0, 8000,
                   "%04d", ImGuiSliderFlags_AlwaysClamp);
    ImGui::SameLine();
    if (ImGui::Button("+1##IncreaseScrWorkStartIndex")) ScrWorkIndexStart += 1;
    ImGui::SameLine();
    if (ImGui::Button("-1##DecreaseScrWorkStartIndex")) ScrWorkIndexStart -= 1;
    ImGui::SameLine();
    if (ImGui::Button("+100##IncreaseScrWorkStartIndex1"))
      ScrWorkIndexStart += 100;
    ImGui::SameLine();
    if (ImGui::Button("-100##DecreaseScrWorkStartIndex1"))
      ScrWorkIndexStart -= 100;
    ImGui::SameLine();
    if (ImGui::Button("+1000##IncreaseScrWorkStartIndex2"))
      ScrWorkIndexStart += 1000;
    ImGui::SameLine();
    if (ImGui::Button("-1000##DecreaseScrWorkStartIndex2"))
      ScrWorkIndexStart -= 1000;
    ImGui::PopButtonRepeat();

    ImGui::Spacing();
    ImGui::Text("End index");
    ImGui::DragInt("##ScrWorkEndIndex", &ScrWorkIndexEnd, 0.5f, 0, 8000, "%04d",
                   ImGuiSliderFlags_AlwaysClamp);

    ImGui::PushButtonRepeat(true);
    ImGui::SameLine();
    if (ImGui::Button("+1##IncreaseScrWorkEndIndex")) ScrWorkIndexEnd += 1;
    ImGui::SameLine();
    if (ImGui::Button("-1##DecreaseScrWorkEndIndex")) ScrWorkIndexEnd -= 1;
    ImGui::SameLine();
    if (ImGui::Button("+100##IncreaseScrWorkEndIndex1")) ScrWorkIndexEnd += 100;
    ImGui::SameLine();
    if (ImGui::Button("-100##DecreaseScrWorkEndIndex1")) ScrWorkIndexEnd -= 100;
    ImGui::SameLine();
    if (ImGui::Button("+1000##IncreaseScrWorkEndIndex2"))
      ScrWorkIndexEnd += 1000;
    ImGui::SameLine();
    if (ImGui::Button("-1000##DecreaseScrWorkEndIndex2"))
      ScrWorkIndexEnd -= 1000;
    ImGui::PopButtonRepeat();

    if (ScrWorkIndexEnd < ScrWorkIndexStart)
      ScrWorkIndexEnd = ScrWorkIndexStart;

    ImGui::TreePop();
  }

  ImGui::Spacing();

  if (ImGui::TreeNode("FlagWork Config")) {
    ImGui::Text("Start index");
    ImGui::DragInt("##FlagWorkStartIndex", &FlagWorkIndexStart, 0.5f, 0, 7000,
                   "%04d", ImGuiSliderFlags_AlwaysClamp);

    ImGui::PushButtonRepeat(true);
    ImGui::SameLine();
    if (ImGui::Button("+1##IncreaseFlagWorkEndIndex")) FlagWorkIndexStart += 1;
    ImGui::SameLine();
    if (ImGui::Button("-1##DecreaseFlagWorkEndIndex")) FlagWorkIndexStart -= 1;
    ImGui::SameLine();
    if (ImGui::Button("+100##IncreaseFlagWorkEndIndex1"))
      FlagWorkIndexStart += 100;
    ImGui::SameLine();
    if (ImGui::Button("-100##DecreaseFlagWorkEndIndex1"))
      FlagWorkIndexStart -= 100;
    ImGui::SameLine();
    if (ImGui::Button("+1000##IncreaseFlagWorkEndIndex2"))
      FlagWorkIndexStart += 1000;
    ImGui::SameLine();
    if (ImGui::Button("-1000##DecreaseFlagWorkEndIndex2"))
      FlagWorkIndexStart -= 1000;
    ImGui::PopButtonRepeat();

    ImGui::Spacing();
    ImGui::Text("End index");
    ImGui::DragInt("##FlagWorkEndIndex", &FlagWorkIndexEnd, 0.5f, 0, 7000,
                   "%04d", ImGuiSliderFlags_AlwaysClamp);

    ImGui::PushButtonRepeat(true);
    ImGui::SameLine();
    if (ImGui::Button("+1##IncreaseFlagWorkEndIndex")) FlagWorkIndexEnd += 1;
    ImGui::SameLine();
    if (ImGui::Button("-1##DecreaseFlagWorkEndIndex")) FlagWorkIndexEnd -= 1;
    ImGui::SameLine();
    if (ImGui::Button("+100##IncreaseFlagWorkEndIndex1"))
      FlagWorkIndexEnd += 100;
    ImGui::SameLine();
    if (ImGui::Button("-100##DecreaseFlagWorkEndIndex1"))
      FlagWorkIndexEnd -= 100;
    ImGui::SameLine();
    if (ImGui::Button("+1000##IncreaseFlagWorkEndIndex2"))
      FlagWorkIndexEnd += 1000;
    ImGui::SameLine();
    if (ImGui::Button("-1000##DecreaseFlagWorkEndIndex2"))
      FlagWorkIndexEnd -= 1000;
    ImGui::PopButtonRepeat();

    if (FlagWorkIndexEnd < FlagWorkIndexStart)
      FlagWorkIndexEnd = FlagWorkIndexStart;

    ImGui::TreePop();
  }

  ImGuiStyle& style = ImGui::GetStyle();
  float windowVisibleX2 =
      ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

  if (ImGui::CollapsingHeader("ScrWork Editor")) {
    bool sameLine = false;
    for (int i = ScrWorkIndexStart; i <= ScrWorkIndexEnd; i++) {
      if (sameLine) ImGui::SameLine();
      ImGui::PushID(i);
      float width = 0.0f;
      char buf[32];
      snprintf(buf, 32, "ScrWork[%04d]", i);
      int one = 1;
      int ten = 10;
      ImGui::InputScalar(buf, ImGuiDataType_S32, &ScrWork[i], &one, &ten,
                         ScrWorkNumberFormat == 1 ? "%08X" : "%d");
      float lastX2 = ImGui::GetItemRectMax().x;
      float nextButtonX2 =
          lastX2 + style.ItemSpacing.x + ImGui::GetItemRectSize().x;
      if (i + 1 <= ScrWorkIndexEnd && nextButtonX2 < windowVisibleX2)
        ImGui::SameLine();
      ImGui::PopID();
    }
  }

  if (ImGui::CollapsingHeader("FlagWork Editor")) {
    for (int i = FlagWorkIndexStart; i <= FlagWorkIndexEnd; i++) {
      ImGui::PushID(i);
      char buf[32];
      snprintf(buf, 32, "Flags[%04d]", i);
      bool flagVal = GetFlag(i);
      if (ImGui::Checkbox(buf, &flagVal)) SetFlag(i, flagVal);
      float lastX2 = ImGui::GetItemRectMax().x;
      float nextButtonX2 =
          lastX2 + style.ItemSpacing.x + ImGui::GetItemRectSize().x;
      if (i + 1 <= FlagWorkIndexEnd && nextButtonX2 < windowVisibleX2)
        ImGui::SameLine();
      ImGui::PopID();
    }
  }
}

}  // namespace DebugMenu
}  // namespace Impacto
