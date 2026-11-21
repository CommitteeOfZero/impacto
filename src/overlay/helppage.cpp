#include "helppage.h"

#include <imgui.h>
#include <string>

#include "../profile/overlay/helpmenu.h"

namespace Impacto::Overlay {

void ShowHelpPage() {
  using Impacto::Profile::Overlay::HelpMenu::Sections;

  if (ImGui::BeginChild("##HelpContent", ImVec2(0, 0),
                        ImGuiChildFlags_NavFlattened)) {
    constexpr ImGuiTableFlags flags =
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
        ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Hideable;

    int sectionIndex = 0;
    for (auto const& section : Sections) {
      ImGui::SeparatorText(section.Name.c_str());

      std::string tableId = "##HelpTable" + std::to_string(sectionIndex++);
      if (ImGui::BeginTable(tableId.c_str(), 4, flags)) {
        ImGui::TableSetupColumn(
            "Action",
            ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoHide,
            1.3f);
        ImGui::TableSetupColumn("Keyboard & Mouse",
                                ImGuiTableColumnFlags_WidthStretch, 1.1f);
        ImGui::TableSetupColumn("Controller",
                                ImGuiTableColumnFlags_WidthStretch, 1.0f);
        ImGui::TableSetupColumn("Touch", ImGuiTableColumnFlags_WidthStretch,
                                1.0f);
        ImGui::TableHeadersRow();

        for (auto const& action : section.Actions) {
          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::TextUnformatted(action.Action.c_str());
          ImGui::TableNextColumn();
          ImGui::TextUnformatted(action.KeyboardMouse.c_str());
          ImGui::TableNextColumn();
          ImGui::TextUnformatted(action.Controller.c_str());
          ImGui::TableNextColumn();
          ImGui::TextUnformatted(action.Touch.c_str());
        }

        ImGui::EndTable();
      }

      ImGui::Spacing();
    }
  }
  ImGui::EndChild();
}

}  // namespace Impacto::Overlay
