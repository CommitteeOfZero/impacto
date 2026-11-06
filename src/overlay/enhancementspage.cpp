#include "enhancementspage.h"

#include <imgui.h>
#include <imgui_scroll_drag.h>

#include "../userconfig.h"

namespace Impacto::Overlay {

void ShowEnhancementsPage(std::string const& selectedGame) {
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
  ImGui::ScrollWhenDraggingOnVoid(ImVec2{0.0f, -ImGui::GetIO().MouseDelta.y},
                                  ImGuiMouseButton_Left);
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

}  // namespace Impacto::Overlay
