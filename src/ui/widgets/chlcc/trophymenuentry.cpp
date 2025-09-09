#include "trophymenuentry.h"

#include "../../../data/achievementsystem.h"
#include "../../../games/chlcc/trophymenu.h"
#include "../../../profile/games/chlcc/trophymenu.h"
#include "../../../profile/game.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::TrophyMenu;
using namespace Impacto::AchievementSystem;

TrophyMenuEntry::TrophyMenuEntry(int achievementId)
    : AchievementId(achievementId) {
  Bounds.SetPos(
      FirstEntryPos +
      glm::vec2(0.0f, EntryHeight * (AchievementId % EntriesPerPage)));

  const auto* ach = AchievementSystem::GetAchievement(AchievementId);
  if (ach == nullptr) {
    NameLabel =
        Label(Vm::ScriptGetTextTableStrAddress(EntryDefaultNameTextTableId,
                                               EntryDefaultNameStringNum),
              Bounds.GetPos() + EntryNameOffset, EntryNameFontSize,
              RendererOutlineMode::BottomRight, 0);
    DescriptionLabel =
        Label("", Bounds.GetPos() + EntryDescriptionOffset,
              EntryDescriptionFontSize, RendererOutlineMode::BottomRight, 0);
    Icon = DefaultTrophyIconSprite;
  } else {
    NameLabel = Label(ach->Name(), Bounds.GetPos() + EntryNameOffset,
                      EntryNameFontSize, RendererOutlineMode::BottomRight, 0);
    DescriptionLabel =
        Label(ach->Description(), Bounds.GetPos() + EntryDescriptionOffset,
              EntryDescriptionFontSize, RendererOutlineMode::BottomRight, 0);
    Icon = ach->Icon();
  }
  IconDest = RectF{Bounds.X, Bounds.Y, DefaultTrophyIconSprite.ScaledWidth(),
                   DefaultTrophyIconSprite.ScaledHeight()} +
             EntryIconOffset;
};

void TrophyMenuEntry::Render() {
  Renderer->DrawSprite(TrophyEntryCardSprite,
                       Bounds.GetPos() + EntryCardOffset);
  Renderer->DrawSprite(Icon, IconDest);

  NameLabel.Render();
  DescriptionLabel.Render();
}

void TrophyMenuEntry::Move(glm::vec2 relativePosition) {
  Widget::Move(relativePosition);

  NameLabel.Move(relativePosition);
  DescriptionLabel.Move(relativePosition);
  IconDest += relativePosition;
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto