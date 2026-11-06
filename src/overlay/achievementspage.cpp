#include "achievementspage.h"

#include <imgui.h>
#include <ankerl/unordered_dense.h>

#include "../data/achievementsystem.h"
#include "../profile/patch.h"

namespace Impacto::Overlay {

void ShowAchievementsPage() {
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

}  // namespace Impacto::Overlay
