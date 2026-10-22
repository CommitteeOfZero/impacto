#include "trophymenuentry.h"

#include <cassert>

#include "../../../data/achievementsystem.h"
#include "../../../data/achievementsystemcommon.h"
#include "../../../profile/data/achievementsystem.h"
#include "../../../profile/games/chlcc/trophymenu.h"

namespace CHLCCTrophyProfile = Impacto::Profile::CHLCC::TrophyMenu;

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::TrophyMenu;
using namespace Impacto::AchievementSystem;

TrophyMenuEntry::TrophyMenuEntry(int achievementId)
    : AchievementId(achievementId) {
  Bounds.SetPos(CHLCCTrophyProfile::FirstEntryPos +
                glm::vec2(0.0f, CHLCCTrophyProfile::EntryHeight *
                                    (AchievementId %
                                     CHLCCTrophyProfile::EntriesPerPage)));

  const auto* ach = GetAchievement(AchievementId);
  assert(ach != nullptr);
  const bool unlocked = IsAchievementUnlocked(AchievementId);
  const bool revealed = unlocked || !ach->Hidden();
  const size_t textColorIndex =
      unlocked ? 0 : CHLCCTrophyProfile::EntryLockedTextColorIndex;
  if (!revealed) {
    NameLabel = Label(Vm::ScriptGetTextTableStrAddress(
                          CHLCCTrophyProfile::EntryDefaultNameTextTableId,
                          CHLCCTrophyProfile::EntryDefaultNameStringNum),
                      Bounds.GetPos() + CHLCCTrophyProfile::EntryNameOffset,
                      CHLCCTrophyProfile::EntryNameFontSize,
                      RendererOutlineMode::BottomRight, textColorIndex);
    DescriptionLabel =
        Label("", Bounds.GetPos() + CHLCCTrophyProfile::EntryDescriptionOffset,
              CHLCCTrophyProfile::EntryDescriptionFontSize,
              RendererOutlineMode::BottomRight, textColorIndex);
    Icon = CHLCCTrophyProfile::DefaultTrophyIconSprite;
  } else {
    NameLabel = Label(ach->Name(),
                      Bounds.GetPos() + CHLCCTrophyProfile::EntryNameOffset,
                      CHLCCTrophyProfile::EntryNameFontSize,
                      RendererOutlineMode::BottomRight, textColorIndex);
    DescriptionLabel =
        Label(ach->Description(),
              Bounds.GetPos() + CHLCCTrophyProfile::EntryDescriptionOffset,
              CHLCCTrophyProfile::EntryDescriptionFontSize,
              RendererOutlineMode::BottomRight, textColorIndex);
    Icon = unlocked ? ach->Icon() : CHLCCTrophyProfile::DefaultTrophyIconSprite;
  }
  IconDest = RectF{Bounds.X, Bounds.Y,
                   CHLCCTrophyProfile::DefaultTrophyIconSprite.ScaledWidth(),
                   CHLCCTrophyProfile::DefaultTrophyIconSprite.ScaledHeight()} +
             CHLCCTrophyProfile::EntryIconOffset;

  AchievementRarity rarity = AchievementRarity::Bronze;
  if (Profile::AchievementSystem::Type == AchievementDataType::Common) {
    if (const CommonAchievement* commonAch =
            static_cast<AchievementSystemCommon*>(
                AchievementSystem::Implementation)
                ->GetAchievement(AchievementId)) {
      rarity = commonAch->GetRarity();
    }
  }
  switch (rarity) {
    case AchievementRarity::Platinum:
      RarityIcon = CHLCCTrophyProfile::PlatinumTrophySprite;
      break;
    case AchievementRarity::Gold:
      RarityIcon = CHLCCTrophyProfile::GoldTrophySprite;
      break;
    case AchievementRarity::Silver:
      RarityIcon = CHLCCTrophyProfile::SilverTrophySprite;
      break;
    case AchievementRarity::Bronze:
    default:
      RarityIcon = CHLCCTrophyProfile::BronzeTrophySprite;
      break;
  }
  RarityIconDest = RectF{Bounds.X, Bounds.Y, RarityIcon.ScaledWidth(),
                         RarityIcon.ScaledHeight()} +
                   CHLCCTrophyProfile::EntryRarityIconOffset;
  RarityIconTint = revealed ? glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}
                            : glm::vec4{0.0f, 0.0f, 0.0f, 1.0f};
};

void TrophyMenuEntry::Render() {
  Renderer->DrawSprite(TrophyEntryCardSprite,
                       Bounds.GetPos() + EntryCardOffset);
  Renderer->DrawSprite(Icon, IconDest);
  Renderer->DrawSprite(RarityIcon, RarityIconDest, RarityIconTint);

  NameLabel.Render();
  DescriptionLabel.Render();
}

void TrophyMenuEntry::Move(glm::vec2 relativePosition) {
  Widget::Move(relativePosition);

  NameLabel.Move(relativePosition);
  DescriptionLabel.Move(relativePosition);
  IconDest += relativePosition;
  RarityIconDest += relativePosition;
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto