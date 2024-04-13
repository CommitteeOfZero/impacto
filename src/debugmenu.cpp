#include "debugmenu.h"

#include "imgui.h"
#include <sstream>

#include "game.h"
#include "mem.h"
#include "inputsystem.h"
#include "vm/vm.h"
#include "io/vfs.h"

namespace Impacto {
namespace DebugMenu {

bool DebugMenuShown = false;

static int ScrWorkNumberFormat = 0;
static int ScrWorkIndexStart = 0;
static int ScrWorkIndexEnd = 0;
static int FlagWorkIndexStart = 0;
static int FlagWorkIndexEnd = 0;

static std::map<uint32_t, std::map<int, int>> ScriptDebugByteCodePosToLine;
static std::map<uint32_t, std::map<int, int>> ScriptDebugLineToByteCodePos;
static std::map<uint32_t, std::vector<std::string>> ScriptDebugSource;

static bool ScriptVariablesEditorShown = false;
static bool ObjectViewerShown = false;
static bool UiViewerShown = false;
static bool ScriptDebuggerShown = false;

static void HelpMarker(const char* desc) {
  ImGui::TextDisabled("(?)");
  if (ImGui::BeginItemTooltip()) {
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

static void ParseScriptDebugData(uint32_t scriptId) {
  if (ScriptDebugSource.find(scriptId) != ScriptDebugSource.end()) return;

  Io::InputStream* stream;
  if (Io::VfsOpen("scriptdbg", scriptId, &stream) != IoError_OK) return;

  std::map<int, int> byteCodePosToLine;
  std::map<int, int> lineToByteCodePos;
  std::vector<std::string> sourceLines;

  std::string content;
  content.resize(stream->Meta.Size);
  int64_t size = stream->Read(&content[0], stream->Meta.Size);
  content.resize(size);

  std::istringstream ss = std::istringstream(content, std::ios::in);
  int lineId = 0;
  for (std::string line; std::getline(ss, line); lineId++) {
    if (line.empty()) continue;

    if (line[line.size() - 1] == '\r') line.pop_back();
    size_t firstColLength = line.find(',');

    if (firstColLength == std::string::npos ||
        firstColLength == line.length() - 1)
      continue;

    size_t secondColLength = line.find(',', firstColLength + 1);
    if (secondColLength == std::string::npos ||
        secondColLength == line.length() - 1)
      continue;

    uint32_t byteCodePos = std::atoi(line.substr(0, firstColLength).c_str());

    std::string sourceLine =
        line.substr(secondColLength + 2, line.length() - secondColLength);

    sourceLines.push_back(sourceLine);
    byteCodePosToLine[byteCodePos] = lineId;
    lineToByteCodePos[lineId] = byteCodePos;
  }

  ScriptDebugByteCodePosToLine[scriptId] = byteCodePosToLine;
  ScriptDebugLineToByteCodePos[scriptId] = lineToByteCodePos;
  ScriptDebugSource[scriptId] = sourceLines;

  delete stream;
}

void ShowSingleWindow() {
  if (ImGui::Begin("Debug Menu", &DebugMenuShown)) {
    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);

    if (ImGui::BeginTabBar("DebugTabBar", ImGuiTabBarFlags_None)) {
      if (ImGui::BeginTabItem("\"Debug Editer\"")) {
        ShowScriptVariablesEditor();
        ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("Objects")) {
        ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("UI")) {
        ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("Script Debugger")) {
        ShowScriptDebugger();
        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
    }
  }

  ImGui::End();
}

void ShowDockableArea() {
  if (ImGui::Begin("Debug Menu##DebugMenuDockArea", &DebugMenuShown,
                   ImGuiWindowFlags_MenuBar)) {
    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("Tools")) {
        ImGui::MenuItem("\"Debug Editer\"", NULL, &ScriptVariablesEditorShown);
        ImGui::MenuItem("Objects", NULL, &ObjectViewerShown);
        ImGui::MenuItem("UI", NULL, &UiViewerShown);
        ImGui::MenuItem("Script Debugger", NULL, &ScriptDebuggerShown);
        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }

    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
  }
  ImGui::End();

  if (ScriptVariablesEditorShown) {
    if (ImGui::Begin("\"Debug Editer\"##ScriptVarEditorWindow"),
        &ScriptVariablesEditorShown) {
      ShowScriptVariablesEditor();
    }
    ImGui::End();
  }

  if (ObjectViewerShown) {
    if (ImGui::Begin("Objects##ObjectViewerWindow"), &ObjectViewerShown) {
      ImGui::Text("Not available");
    }
    ImGui::End();
  }

  if (UiViewerShown) {
    if (ImGui::Begin("UI##UIViewerWindow"), &UiViewerShown) {
      ImGui::Text("Not available");
    }
    ImGui::End();
  }

  if (ScriptDebuggerShown) {
    if (ImGui::Begin("Script Debugger##ScriptDebuggerWindow"),
        &ScriptDebuggerShown) {
      ShowScriptDebugger();
    }
    ImGui::End();
  }

  if (!DebugMenuShown) {
    ScriptVariablesEditorShown = false;
    ObjectViewerShown = false;
    UiViewerShown = false;
    ScriptDebuggerShown = false;
  }
}

void Show() {
  if (((Input::KeyboardButtonIsDown[SDL_SCANCODE_LALT] &&
        Input::KeyboardButtonWentDown[SDL_SCANCODE_D]) ||
       (Input::ControllerButtonWentDown[SDL_CONTROLLER_BUTTON_RIGHTSHOULDER] &&
        Input::ControllerButtonWentDown[SDL_CONTROLLER_BUTTON_Y])) &&
      !DebugMenuShown)
    DebugMenuShown = true;

  if (DebugMenuShown) {
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable) {
      ShowDockableArea();
    } else {
      ShowSingleWindow();
    }
  } else {
    Vm::DebugThreadId = -1;
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
    for (int i = ScrWorkIndexStart; i <= ScrWorkIndexEnd; i++) {
      ImGui::PushID(i);
      float width = 0.0f;
      char buf[32];
      snprintf(buf, 32, "ScrWork[%04d]", i);
      int one = 1;
      int ten = 10;
      ImGui::InputScalar(buf, ImGuiDataType_S32, &ScrWork[i], &one, &ten,
                         ScrWorkNumberFormat == 1 ? "%08lX" : "%d");
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

  ImGui::PopItemWidth();
}

static int ScriptDebuggerSelectedThreadId = -1;
static int ThreadVarsNumberFormat = 0;
static bool AutoScrollSourceView = true;

void ShowScriptDebugger() {
  if (ScriptDebuggerSelectedThreadId == -1) {
    int firstThreadId = 0;
    for (int i = 0; i < Vm::MaxThreads; i++) {
      if (Vm::ThreadPool[i].Ip != 0) {
        firstThreadId = i;
        break;
      }
    }
    ScriptDebuggerSelectedThreadId = firstThreadId;
  }

  char comboPreviewValue[128];
  auto groupType = ThreadGroupType::_from_integral_nothrow(
      Vm::ThreadPool[ScriptDebuggerSelectedThreadId].GroupId);
  if (groupType) {
    snprintf(
        comboPreviewValue, 128, "[%s][%d] %s", groupType.value()._to_string(),
        Vm::ThreadPool[ScriptDebuggerSelectedThreadId].Id,
        Vm::LoadedScriptMetas[Vm::ThreadPool[ScriptDebuggerSelectedThreadId]
                                  .ScriptBufferId]
            .FileName.c_str());
  } else {
    snprintf(
        comboPreviewValue, 128, "[%d][%d] %s",
        Vm::ThreadPool[ScriptDebuggerSelectedThreadId].GroupId,
        Vm::ThreadPool[ScriptDebuggerSelectedThreadId].Id,
        Vm::LoadedScriptMetas[Vm::ThreadPool[ScriptDebuggerSelectedThreadId]
                                  .ScriptBufferId]
            .FileName.c_str());
  }

  if (ImGui::BeginCombo("Thread##vmThreadCombo", comboPreviewValue,
                        ImGuiComboFlags_WidthFitPreview)) {
    for (int i = 0; i < Vm::MaxThreads; i++) {
      if (Vm::ThreadPool[i].Ip != 0) {
        const bool isSelected = (ScriptDebuggerSelectedThreadId == i);
        auto groupType =
            ThreadGroupType::_from_integral_nothrow(Vm::ThreadPool[i].GroupId);
        if (groupType) {
          snprintf(comboPreviewValue, 128, "[%s][%d] %s",
                   groupType.value()._to_string(), Vm::ThreadPool[i].Id,
                   Vm::LoadedScriptMetas[Vm::ThreadPool[i].ScriptBufferId]
                       .FileName.c_str());
        } else {
          snprintf(comboPreviewValue, 128, "[%d][%d] %s",
                   Vm::ThreadPool[i].GroupId, Vm::ThreadPool[i].Id,
                   Vm::LoadedScriptMetas[Vm::ThreadPool[i].ScriptBufferId]
                       .FileName.c_str());
        }
        if (ImGui::Selectable(comboPreviewValue, isSelected))
          ScriptDebuggerSelectedThreadId = i;
        if (isSelected) ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndCombo();
  }

  ImGui::SeparatorText("Thread flags:");
  if (ImGui::BeginTable("tableThreadFlags", 3, ImGuiTableFlags_None)) {
    ImGui::TableNextColumn();
    if (ImGui::CheckboxFlags(
            "Destroy", &Vm::ThreadPool[ScriptDebuggerSelectedThreadId].Flags,
            Vm::TF_Destroy))
      ScriptDebuggerSelectedThreadId = 0;
    ImGui::SameLine();
    HelpMarker("Setting this will DESTROY the thread IMMEDIATELY!");
    ImGui::CheckboxFlags("Animate",
                         &Vm::ThreadPool[ScriptDebuggerSelectedThreadId].Flags,
                         Vm::TF_Animate);
    ImGui::TableNextColumn();
    ImGui::CheckboxFlags("Display",
                         &Vm::ThreadPool[ScriptDebuggerSelectedThreadId].Flags,
                         Vm::TF_Display);
    ImGui::CheckboxFlags("Pause",
                         &Vm::ThreadPool[ScriptDebuggerSelectedThreadId].Flags,
                         Vm::TF_Pause);
    ImGui::TableNextColumn();
    ImGui::CheckboxFlags("Message",
                         &Vm::ThreadPool[ScriptDebuggerSelectedThreadId].Flags,
                         Vm::TF_Message);
    ImGui::EndTable();
  }

  ImGui::SeparatorText("Thread data:");
  if (ImGui::BeginTable("tableThreadData", 3, ImGuiTableFlags_None)) {
    ImGui::TableNextColumn();
    ImGui::Text("ID: %d", Vm::ThreadPool[ScriptDebuggerSelectedThreadId].Id);
    ImGui::Text(
        "IP: %08lX",
        Vm::ThreadPool[ScriptDebuggerSelectedThreadId].Ip -
            Vm::ScriptBuffers[Vm::ThreadPool[ScriptDebuggerSelectedThreadId]
                                  .ScriptBufferId]);
    ImGui::Text("ExecPriority: %d",
                Vm::ThreadPool[ScriptDebuggerSelectedThreadId].ExecPriority);

    ImGui::TableNextColumn();
    auto drawType = Game::DrawComponentType::_from_integral_nothrow(
        Vm::ThreadPool[ScriptDebuggerSelectedThreadId].DrawType);
    if (drawType) {
      ImGui::Text("DrawType: %s", drawType.value()._to_string());
    } else {
      ImGui::Text("DrawType: %d",
                  Vm::ThreadPool[ScriptDebuggerSelectedThreadId].DrawType);
    }
    ImGui::Text("DrawPriority: %d",
                Vm::ThreadPool[ScriptDebuggerSelectedThreadId].DrawPriority);
    ImGui::Text("Alpha: %d",
                Vm::ThreadPool[ScriptDebuggerSelectedThreadId].Alpha);

    ImGui::TableNextColumn();
    auto groupType = ThreadGroupType::_from_integral_nothrow(
        Vm::ThreadPool[ScriptDebuggerSelectedThreadId].GroupId);
    if (groupType) {
      ImGui::Text("Group: %s", groupType.value()._to_string());
    } else {
      ImGui::Text("Group: %d",
                  Vm::ThreadPool[ScriptDebuggerSelectedThreadId].GroupId);
    }
    ImGui::Text("DialoguePageId: %d",
                Vm::ThreadPool[ScriptDebuggerSelectedThreadId].DialoguePageId);
    ImGui::Text("WaitCounter: %d",
                Vm::ThreadPool[ScriptDebuggerSelectedThreadId].WaitCounter);

    ImGui::EndTable();
  }

  ImGui::Spacing();
  if (ImGui::TreeNode("Source View")) {
    uint32_t scriptId =
        Vm::LoadedScriptMetas[Vm::ThreadPool[ScriptDebuggerSelectedThreadId]
                                  .ScriptBufferId]
            .Id;
    uint32_t scriptIp =
        (uint32_t)(Vm::ThreadPool[ScriptDebuggerSelectedThreadId].Ip -
                   Vm::ScriptBuffers
                       [Vm::ThreadPool[ScriptDebuggerSelectedThreadId]
                            .ScriptBufferId]);

    ParseScriptDebugData(scriptId);
    if (ScriptDebugSource.find(scriptId) != ScriptDebugSource.end()) {
      auto byteCodePosTable = ScriptDebugByteCodePosToLine[scriptId];
      auto lineToByteCodePosTable = ScriptDebugLineToByteCodePos[scriptId];
      auto currentLine = byteCodePosTable.lower_bound(scriptIp);
      int currentLineNum = -1;
      if (currentLine != byteCodePosTable.end()) {
        currentLineNum = currentLine->second;
      }

      Vm::DebugThreadId = ScriptDebuggerSelectedThreadId;
      ImGui::Checkbox("Auto scroll source view", &AutoScrollSourceView);
      ImGui::SameLine();
      if (Vm::DebuggerBreak) {
        if (ImGui::Button("Continue")) Vm::DebuggerContinueRequest = true;
      } else {
        if (ImGui::Button("Break")) Vm::DebuggerBreak = true;
      }
      ImGui::SameLine();
      ImGui::BeginDisabled(!Vm::DebuggerBreak);
      if (ImGui::Button("Step"))
        Vm::DebuggerStepRequest = true;
      else
        Vm::DebuggerStepRequest = false;
      ImGui::EndDisabled();
      ImGui::SameLine();
      if (ImGui::Button("Clear breakpoints")) {
        Vm::DebuggerBreakpoints.clear();
      }

      if (ImGui::TreeNode("Breakpoint List")) {
        if (ImGui::BeginTable(
                "BreakpointList", 1,
                ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders)) {
          char buf[256];
          for (auto it = Vm::DebuggerBreakpoints.cbegin(), nextIt = it;
               it != Vm::DebuggerBreakpoints.cend(); it = nextIt) {
            ++nextIt;
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            Io::FileMeta scriptMeta;
            Io::VfsGetMeta("script", it->second.first, &scriptMeta);
            bool isBreakpoint = true;
            snprintf(buf, 256, "%08lX - %d - %s - %s", it->second.second,
                     it->first, scriptMeta.FileName.c_str(),
                     ScriptDebugSource[it->second.first][it->first].c_str());
            ImGui::Selectable(buf, &isBreakpoint,
                              ImGuiSelectableFlags_SpanAllColumns);
            if (!isBreakpoint) {
              Vm::DebuggerBreakpoints.erase(it);
            }
          }
          ImGui::EndTable();
        }

        ImGui::TreePop();
      }

      if (ImGui::BeginTable(
              "Source", 2,
              ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders |
                  ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY,
              ImVec2(ImGui::GetContentRegionAvail().x,
                     ImGui::GetContentRegionAvail().y * 0.9f))) {
        ImGuiListClipper clipper;
        clipper.Begin(ScriptDebugSource[scriptId].size());
        while (clipper.Step()) {
          for (int row = clipper.DisplayStart; row < clipper.DisplayEnd;
               row++) {
            ImGui::PushID(row);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%d", row + 1);
            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Header,
                                  ImVec4(1.0f, 0.0f, 0.0f, 0.65f));
            bool isBreakpoint = Vm::DebuggerBreakpoints.find(row) !=
                                Vm::DebuggerBreakpoints.end();
            isBreakpoint =
                isBreakpoint &&
                Vm::DebuggerBreakpoints.find(row)->second.first == scriptId;
            ImGui::Selectable(ScriptDebugSource[scriptId][row].c_str(),
                              &isBreakpoint,
                              ImGuiSelectableFlags_SpanAllColumns);
            if (isBreakpoint) {
              Vm::DebuggerBreakpoints[row] = std::make_pair(
                  Vm::LoadedScriptMetas
                      [Vm::ThreadPool[ScriptDebuggerSelectedThreadId]
                           .ScriptBufferId]
                          .Id,
                  lineToByteCodePosTable[row]);
            } else if (Vm::DebuggerBreakpoints.find(row) !=
                           Vm::DebuggerBreakpoints.end() &&
                       Vm::DebuggerBreakpoints.find(row)->second.first ==
                           scriptId) {
              Vm::DebuggerBreakpoints.erase(row);
            }
            ImGui::PopStyleColor();
            if (currentLineNum == row) {
              ImGui::TableSetBgColor(
                  ImGuiTableBgTarget_RowBg0,
                  ImGui::GetColorU32(ImVec4(0.0f, 0.7f, 0.0f, 0.65f)));
            }
            ImGui::PopID();
          }
        }
        if (AutoScrollSourceView)
          ImGui::SetScrollY((currentLineNum - 5) *
                            ImGui::GetTextLineHeightWithSpacing());
        ImGui::EndTable();
      }
    }
    ImGui::TreePop();
  }

  ImGui::Spacing();
  if (ImGui::TreeNode("Call Stack")) {
    for (int i =
             Vm::ThreadPool[ScriptDebuggerSelectedThreadId].CallStackDepth - 1;
         i >= 0; i--) {
      ImGui::PushID(i);
      ImGui::Text(
          "%s - %08X",
          Vm::LoadedScriptMetas[Vm::ThreadPool[ScriptDebuggerSelectedThreadId]
                                    .ReturnScriptBufferIds[i]]
              .FileName.c_str(),
          Vm::ThreadPool[ScriptDebuggerSelectedThreadId].ReturnAdresses[i] -
              Vm::ScriptBuffers[Vm::ThreadPool[ScriptDebuggerSelectedThreadId]
                                    .ReturnScriptBufferIds[i]]);
      ImGui::PopID();
    }

    ImGui::TreePop();
  }
  ImGui::Spacing();

  ImGuiStyle& style = ImGui::GetStyle();
  float windowVisibleX2 =
      ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
  ImGui::PushItemWidth(10.0f * ImGui::GetFontSize());

  if (ImGui::TreeNode("Variables")) {
    ImGui::Text("Display number format");
    ImGui::SameLine();
    ImGui::RadioButton("DEC", &ThreadVarsNumberFormat, 0);
    ImGui::SameLine();
    ImGui::RadioButton("HEX", &ThreadVarsNumberFormat, 1);

    for (int i = 0; i < Vm::MaxThreadVars; i++) {
      ImGui::PushID(i);
      char buf[32];
      snprintf(buf, 32, "Vars[%02d]", i);
      int one = 1;
      ImGui::InputScalar(
          buf, ImGuiDataType_S32,
          &Vm::ThreadPool[ScriptDebuggerSelectedThreadId].Variables[i], &one, 0,
          ThreadVarsNumberFormat == 1 ? "%08X" : "%d");
      float lastX2 = ImGui::GetItemRectMax().x;
      float nextButtonX2 =
          lastX2 + style.ItemSpacing.x + ImGui::GetItemRectSize().x;
      if (i + 1 < Vm::MaxThreadVars && nextButtonX2 < windowVisibleX2)
        ImGui::SameLine();
      ImGui::PopID();
    }

    ImGui::TreePop();
  }

  ImGui::PopItemWidth();
}

}  // namespace DebugMenu
}  // namespace Impacto
