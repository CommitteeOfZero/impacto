#include "debugmenu.h"

#include <imgui.h>
#include <sstream>
#include <limits>

#include "game.h"
#include "mem.h"
#include "inputsystem.h"
#include "vm/vm.h"
#include "io/vfs.h"
#include "background2d.h"
#include "character2d.h"
#include "profile/sprites.h"
#include "profile/vm.h"

namespace Impacto {
namespace DebugMenu {

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

static void ImageTooltip(ImVec2 pos, ImTextureID textureId, float texWidth,
                         float texHeight) {
  if (ImGui::BeginItemTooltip()) {
    float regionSz = 32.0f;
    float regionX = ImGui::GetIO().MousePos.x - pos.x - regionSz * 0.5f;
    float regionY = ImGui::GetIO().MousePos.y - pos.y - regionSz * 0.5f;
    float zoom = 6.0f;
    if (regionX < 0.0f) {
      regionX = 0.0f;
    } else if (regionX > texWidth - regionSz) {
      regionX = texWidth - regionSz;
    }
    if (regionY < 0.0f) {
      regionY = 0.0f;
    } else if (regionY > texHeight - regionSz) {
      regionY = texHeight - regionSz;
    }
    ImGui::Text("Min: (%.2f, %.2f)", regionX, regionY);
    ImGui::Text("Max: (%.2f, %.2f)", regionX + regionSz, regionY + regionSz);
    ImVec2 uv0 = ImVec2((regionX) / texWidth, (regionY) / texHeight);
    ImVec2 uv1 = ImVec2((regionX + regionSz) / texWidth,
                        (regionY + regionSz) / texHeight);
    ImGui::Image(textureId, ImVec2(regionSz * zoom, regionSz * zoom), uv0, uv1);
    ImGui::EndTooltip();
  }
}

static void ParseScriptDebugData(uint32_t scriptId) {
  if (ScriptDebugSource.find(scriptId) != ScriptDebugSource.end()) return;

  Io::Stream* stream;
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
    ImGui::Text("Cursor Pos: (%.1f,%.1f)", ImGui::GetIO().MousePos.x,
                ImGui::GetIO().MousePos.y);

    if (ImGui::BeginTabBar("DebugTabBar", ImGuiTabBarFlags_None)) {
      if (ImGui::BeginTabItem("\"Debug Editer\"")) {
        ShowScriptVariablesEditor();
        ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("Objects")) {
        ShowObjects();
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
        ScriptVariablesEditorShown) {
      ShowScriptVariablesEditor();
    }
    ImGui::End();
  }

  if (ObjectViewerShown) {
    if (ImGui::Begin("Objects##ObjectViewerWindow"), ObjectViewerShown) {
      ShowObjects();
    }
    ImGui::End();
  }

  if (UiViewerShown) {
    if (ImGui::Begin("UI##UIViewerWindow"), UiViewerShown) {
      ImGui::Text("Not available");
    }
    ImGui::End();
  }

  if (ScriptDebuggerShown) {
    if (ImGui::Begin("Script Debugger##ScriptDebuggerWindow"),
        ScriptDebuggerShown) {
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
    Vm::DebugThreadId = std::numeric_limits<uint32_t>::max();
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
    if (ImGui::Button("+1##IncreaseFlagWorkStartIndex"))
      FlagWorkIndexStart += 1;
    ImGui::SameLine();
    if (ImGui::Button("-1##DecreaseFlagWorkStartIndex"))
      FlagWorkIndexStart -= 1;
    ImGui::SameLine();
    if (ImGui::Button("+100##IncreaseFlagWorkStartIndex1"))
      FlagWorkIndexStart += 100;
    ImGui::SameLine();
    if (ImGui::Button("-100##DecreaseFlagWorkStartIndex1"))
      FlagWorkIndexStart -= 100;
    ImGui::SameLine();
    if (ImGui::Button("+1000##IncreaseFlagWorkStartIndex2"))
      FlagWorkIndexStart += 1000;
    ImGui::SameLine();
    if (ImGui::Button("-1000##DecreaseFlagWorkStartIndex2"))
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
static int ScriptDebuggerSelectedScriptId = -1;
static int ThreadVarsNumberFormat = 0;
static bool AutoScrollSourceView = true;
static std::map<uint32_t, std::string> ScriptFilesListing;

void ShowScriptDebugger() {
  if (ScriptDebuggerSelectedThreadId == -1 ||
      Vm::ThreadPool[ScriptDebuggerSelectedThreadId].IpOffset == 0) {
    int firstThreadId = 0;
    for (int i = 0; i < Vm::MaxThreads; i++) {
      if (Vm::ThreadPool[i].IpOffset != 0) {
        firstThreadId = i;
        break;
      }
    }
    ScriptDebuggerSelectedThreadId = firstThreadId;
    ScriptDebuggerSelectedScriptId =
        Vm::LoadedScriptMetas[Vm::ThreadPool[ScriptDebuggerSelectedThreadId]
                                  .ScriptBufferId]
            .Id;
  }

  char comboPreviewValue[128];
  auto groupType = ThreadGroupType::_from_integral_nothrow(
      (uint8_t)Vm::ThreadPool[ScriptDebuggerSelectedThreadId].GroupId);
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
      if (Vm::ThreadPool[i].IpOffset != 0) {
        const bool isSelected = (ScriptDebuggerSelectedThreadId == i);
        groupType = ThreadGroupType::_from_integral_nothrow(
            (uint8_t)Vm::ThreadPool[i].GroupId);
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
        if (ImGui::Selectable(comboPreviewValue, isSelected)) {
          ScriptDebuggerSelectedThreadId = i;
          AutoScrollSourceView = true;
        }
        if (isSelected) ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndCombo();
  }

  ImGui::SameLine();
  if (ScriptFilesListing.size() == 0) {
    IoError err = Io::VfsListFiles("script", ScriptFilesListing);
    if (err != IoError_OK) {
      ImpLog(LogLevel::Warning, LogChannel::General,
             "Failed to open script archive!\n");
      return;
    }
  }

  snprintf(comboPreviewValue, 128, "[%d] %s", ScriptDebuggerSelectedScriptId,
           ScriptFilesListing[ScriptDebuggerSelectedScriptId].c_str());
  if (ImGui::BeginCombo("Script##vmScriptCombo", comboPreviewValue,
                        ImGuiComboFlags_WidthFitPreview)) {
    for (auto const& file : ScriptFilesListing) {
      const bool isSelected =
          (static_cast<uint32_t>(ScriptDebuggerSelectedScriptId) == file.first);
      snprintf(comboPreviewValue, 128, "[%d] %s", file.first,
               file.second.c_str());
      if (ImGui::Selectable(comboPreviewValue, isSelected)) {
        ScriptDebuggerSelectedScriptId = file.first;
        if (static_cast<uint32_t>(ScriptDebuggerSelectedScriptId) !=
            Vm::LoadedScriptMetas[Vm::ThreadPool[ScriptDebuggerSelectedThreadId]
                                      .ScriptBufferId]
                .Id) {
          AutoScrollSourceView = false;
        }
      }
      if (isSelected) {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndCombo();
  }

  if (AutoScrollSourceView) {
    ScriptDebuggerSelectedScriptId =
        Vm::LoadedScriptMetas[Vm::ThreadPool[ScriptDebuggerSelectedThreadId]
                                  .ScriptBufferId]
            .Id;
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
                         (unsigned int)Vm::TF_Message);
    ImGui::EndTable();
  }

  ImGui::SeparatorText("Thread data:");
  if (ImGui::BeginTable("tableThreadData", 3, ImGuiTableFlags_None)) {
    ImGui::TableNextColumn();
    ImGui::Text("ID: %d", Vm::ThreadPool[ScriptDebuggerSelectedThreadId].Id);
    ImGui::Text("IP: %d",
                Vm::ThreadPool[ScriptDebuggerSelectedThreadId].IpOffset);
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
    groupType = ThreadGroupType::_from_integral_nothrow(
        (uint8_t)Vm::ThreadPool[ScriptDebuggerSelectedThreadId].GroupId);
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
    uint32_t scriptId = ScriptDebuggerSelectedScriptId;
    uint32_t scriptIp = Vm::ThreadPool[ScriptDebuggerSelectedThreadId].IpOffset;

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
        if (ImGui::Button("Continue")) {
          AutoScrollSourceView = true;
          Vm::DebuggerContinueRequest = true;
        }
      } else {
        if (ImGui::Button("Break")) {
          AutoScrollSourceView = true;
          Vm::DebuggerBreak = true;
        }
      }
      ImGui::SameLine();
      ImGui::BeginDisabled(!Vm::DebuggerBreak);
      if (ImGui::Button("Step")) {
        AutoScrollSourceView = true;
        Vm::DebuggerStepRequest = true;
      }
      ImGui::EndDisabled();
      ImGui::SameLine();
      if (ImGui::Button("Clear breakpoints")) {
        Vm::DebuggerBreakpoints.clear();
      }
      ImGui::SameLine();
      ImGui::Checkbox("Always block thread", &Vm::DebuggerAlwaysBlock);

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
            fmt::format_to_n(buf, 256, "%08X - %d - %s - %s", it->second.second,
                             it->first, scriptMeta.FileName,
                             ScriptDebugSource[it->second.first][it->first]);
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
        clipper.Begin((int)ScriptDebugSource[scriptId].size());
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
              Vm::DebuggerBreakpoints[row] =
                  std::make_pair(scriptId, lineToByteCodePosTable[row]);
            } else if (Vm::DebuggerBreakpoints.find(row) !=
                           Vm::DebuggerBreakpoints.end() &&
                       Vm::DebuggerBreakpoints.find(row)->second.first ==
                           scriptId) {
              Vm::DebuggerBreakpoints.erase(row);
            }
            ImGui::PopStyleColor();
            if (scriptId == Vm::LoadedScriptMetas
                                [Vm::ThreadPool[ScriptDebuggerSelectedThreadId]
                                     .ScriptBufferId]
                                    .Id &&
                currentLineNum == row) {
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
      auto& thd = Vm::ThreadPool[ScriptDebuggerSelectedThreadId];
      uint32_t returnAddress =
          (Profile::Vm::UseReturnIds)
              ? Vm::ScriptGetRetAddress(thd.ReturnScriptBufferIds[i],
                                        thd.ReturnIds[i])
              : thd.ReturnAddresses[i];

      ImGui::Text(
          "%s - %08X",
          Vm::LoadedScriptMetas[thd.ReturnScriptBufferIds[i]].FileName.c_str(),
          returnAddress);
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

static ankerl::unordered_dense::map<uint32_t, std::vector<std::string>>
    SpritesBySpriteSheet;

static void ShowSprite(const Sprite* sprite) {
  if (Profile::ActiveRenderer == +RendererType::OpenGL) {
    float texWidth = sprite->Sheet.DesignWidth;
    float texHeight = sprite->Sheet.DesignHeight;
    ImGui::Image(
        (ImTextureID)(intptr_t)sprite->Sheet.Texture,
        ImVec2(sprite->Bounds.Width, sprite->Bounds.Height),
        ImVec2(sprite->Bounds.X / texWidth, sprite->Bounds.Y / texHeight),
        ImVec2((sprite->Bounds.X + sprite->Bounds.Width) / texWidth,
               (sprite->Bounds.Y + sprite->Bounds.Height) / texHeight));
  }
}

void ShowObjects() {
  ImGui::ShowDemoWindow();
  ImGui::PushItemWidth(10.0f * ImGui::GetFontSize());

  if (SpritesBySpriteSheet.size() == 0) {
    for (const auto& sprite : Profile::Sprites) {
      SpritesBySpriteSheet[sprite.second.Sheet.Texture].push_back(sprite.first);
    }
  }

  if (ImGui::TreeNode("SpriteSheets")) {
    for (const auto& spriteSheet : Profile::SpriteSheets) {
      if (ImGui::TreeNode(spriteSheet.first.c_str())) {
        ImGui::PushID(spriteSheet.second.Texture);
        float texWidth = spriteSheet.second.DesignWidth * 0.4f;
        float texHeight = spriteSheet.second.DesignHeight * 0.4f;
        // Only OpenGL for now
        if (Profile::ActiveRenderer == +RendererType::OpenGL) {
          ImVec2 pos = ImGui::GetCursorScreenPos();
          ImGui::Image((ImTextureID)(intptr_t)spriteSheet.second.Texture,
                       ImVec2(texWidth, texHeight));
          ImageTooltip(pos, (ImTextureID)(intptr_t)spriteSheet.second.Texture,
                       texWidth, texHeight);
        }

        ImGui::Spacing();
        ImGui::BulletText("Texture: (width: %f, height: %f)",
                          spriteSheet.second.DesignWidth,
                          spriteSheet.second.DesignHeight);

        if (ImGui::TreeNode("Sprites")) {
          for (const auto& spriteName :
               SpritesBySpriteSheet[spriteSheet.second.Texture]) {
            const auto& sprite = Profile::Sprites[spriteName];
            if (ImGui::TreeNode(spriteName.c_str())) {
              ShowSprite(&sprite);
              ImGui::Spacing();
              ImGui::BulletText("Bounds: (x: %f, y: %f, width: %f, height: %f)",
                                sprite.Bounds.X, sprite.Bounds.Y,
                                sprite.Bounds.Width, sprite.Bounds.Height);

              ImGui::TreePop();
            }
          }
          ImGui::TreePop();
        }

        ImGui::PopID();

        ImGui::TreePop();
      }
    }
    ImGui::TreePop();
  }

  HelpMarker(
      "These values are read-only as everything here is controlled by "
      "scripts.");
  if (ImGui::TreeNode("Backgrounds")) {
    for (size_t i = 0; i < Backgrounds.size(); i++) {
      ImGui::PushID(i);
      if (ImGui::TreeNode("Background", "Background %zu", i)) {
        if (Backgrounds[i].Status == LoadStatus::Loaded) {
          float texWidth = Backgrounds[i].BgSprite.Sheet.DesignWidth * 0.4f;
          float texHeight = Backgrounds[i].BgSprite.Sheet.DesignHeight * 0.4f;
          // Only OpenGL for now
          if (Profile::ActiveRenderer == +RendererType::OpenGL) {
            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImGui::Image(
                (ImTextureID)(intptr_t)Backgrounds[i].BgSprite.Sheet.Texture,
                ImVec2(texWidth, texHeight));
            ImageTooltip(
                pos,
                (ImTextureID)(intptr_t)Backgrounds[i].BgSprite.Sheet.Texture,
                texWidth, texHeight);
          }
        }

        ImGui::Spacing();
        ImGui::BulletText("Status: %d", (int)Backgrounds[i].Status);
        ImGui::Spacing();
        ImGui::BulletText("Texture: (width: %f, height: %f)",
                          Backgrounds[i].BgSprite.Sheet.DesignWidth,
                          Backgrounds[i].BgSprite.Sheet.DesignHeight);
        ImGui::Spacing();
        ImGui::BulletText("IsShown: %d", Backgrounds[i].Show);
        ImGui::Spacing();
        ImGui::BulletText("Layers: %d, %d", Backgrounds[i].Layers[0],
                          Backgrounds[i].Layers[1]);
        ImGui::Spacing();
        ImGui::BulletText("Display coords: (x: %f, y: %f)",
                          Backgrounds[i].Position.x, Backgrounds[i].Position.y);
        ImGui::Spacing();
        ImGui::BulletText("Sprite: (x: %f, y: %f, width: %f, height: %f)",
                          Backgrounds[i].BgSprite.Bounds.X,
                          Backgrounds[i].BgSprite.Bounds.Y,
                          Backgrounds[i].BgSprite.Bounds.Width,
                          Backgrounds[i].BgSprite.Bounds.Height);
        ImGui::Spacing();
        ImGui::BulletText("Scale: (x: %f, y: %f)",
                          Backgrounds[i].BgSprite.BaseScale.x,
                          Backgrounds[i].BgSprite.BaseScale.y);

        ImGui::TreePop();
      }
      ImGui::PopID();
    }
    ImGui::TreePop();
  }

  if (ImGui::TreeNode("Characters")) {
    for (size_t i = 0; i < Characters2D.size(); i++) {
      ImGui::PushID(i);
      if (ImGui::TreeNode("Character", "Character %zu", i)) {
        if (Characters2D[i].Status == LoadStatus::Loaded) {
          float texWidth = Characters2D[i].CharaSprite.Sheet.DesignWidth * 0.4f;
          float texHeight =
              Characters2D[i].CharaSprite.Sheet.DesignHeight * 0.4f;
          // Only OpenGL for now
          if (Profile::ActiveRenderer == +RendererType::OpenGL) {
            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImGui::Image((ImTextureID)(intptr_t)Characters2D[i]
                             .CharaSprite.Sheet.Texture,
                         ImVec2(texWidth, texHeight));
            ImageTooltip(pos,
                         (ImTextureID)(intptr_t)Characters2D[i]
                             .CharaSprite.Sheet.Texture,
                         texWidth, texHeight);
          }
        }

        ImGui::Spacing();
        ImGui::BulletText("Status: %d", (int)Characters2D[i].Status);
        ImGui::Spacing();
        ImGui::BulletText("Texture: (width: %f, height: %f)",
                          Characters2D[i].CharaSprite.Sheet.DesignWidth,
                          Characters2D[i].CharaSprite.Sheet.DesignHeight);
        ImGui::Spacing();
        ImGui::BulletText("IsShown: %d", Characters2D[i].Show);
        ImGui::Spacing();
        ImGui::BulletText("Layers: %d, %d", Characters2D[i].Layers[0],
                          Characters2D[i].Layers[1]);
        ImGui::Spacing();
        ImGui::BulletText("Display coords: (x: %f, y: %f)",
                          Characters2D[i].Position.x,
                          Characters2D[i].Position.y);
        ImGui::Spacing();
        ImGui::BulletText("Scale: (x: %f, y: %f)", Characters2D[i].Scale.x,
                          Characters2D[i].Scale.y);
        ImGui::Spacing();
        ImGui::BulletText("Face: %d", Characters2D[i].Face);
        ImGui::Spacing();

        ImGui::TreePop();
      }
      ImGui::PopID();
    }
    ImGui::TreePop();
  }

  ImGui::PopItemWidth();
}

}  // namespace DebugMenu
}  // namespace Impacto
