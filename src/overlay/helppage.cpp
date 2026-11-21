#include "helppage.h"

#include <imgui.h>
#include <string>

#include "../profile/ui/helpmenu.h"

namespace Impacto::Overlay {

void ShowHelpPage() {
  using Impacto::Profile::HelpMenu::Actions;

  if (ImGui::BeginChild("##HelpContent")) {
    constexpr ImGuiTableFlags flags = ImGuiTableFlags_Borders |
                                      ImGuiTableFlags_RowBg |
                                      ImGuiTableFlags_SizingStretchProp;

    std::string currentSection;
    bool tableOpen = false;
    int sectionIndex = 0;

    for (auto const& entry : Actions) {
      if (entry.Section != currentSection) {
        if (tableOpen) {
          ImGui::EndTable();
          tableOpen = false;
          ImGui::Spacing();
        }
        currentSection = entry.Section;
        ImGui::SeparatorText(currentSection.c_str());

        std::string tableId = "##HelpTable" + std::to_string(sectionIndex++);
        tableOpen = ImGui::BeginTable(tableId.c_str(), 3, flags);
        if (tableOpen) {
          ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthStretch,
                                  1.3f);
          ImGui::TableSetupColumn("Keyboard & Mouse",
                                  ImGuiTableColumnFlags_WidthStretch, 1.1f);
          ImGui::TableSetupColumn("Controller",
                                  ImGuiTableColumnFlags_WidthStretch, 1.0f);
          ImGui::TableHeadersRow();
        }
      }

      if (tableOpen) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextUnformatted(entry.Action.c_str());
        ImGui::TableNextColumn();
        ImGui::TextUnformatted(entry.KeyboardMouse.c_str());
        ImGui::TableNextColumn();
        ImGui::TextUnformatted(entry.Controller.c_str());
      }
    }

    if (tableOpen) ImGui::EndTable();
  }
  ImGui::EndChild();
}

}  // namespace Impacto::Overlay
