#include "trophymenuentry.h"
#include "../../data/achievementsystem.h"
#include "trophymenu.h"
#include "../../profile/games/chlcc/trophymenu.h"
#include "../../profile/game.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::TrophyMenu;
using namespace Impacto::AchievementSystem;

TrophyMenuEntry::TrophyMenuEntry(int achievementId)
    : AchievementId(achievementId), Offset(0.0f, -Profile::DesignHeight) {
  Bounds.SetPos(glm::vec2(0.0f, 74.0f * (AchievementId % 6) + 130) + Offset);

  const auto* ach = AchievementSystem::GetAchievement(AchievementId);
  if (ach == nullptr) {
    NameLabel = Label(Vm::ScriptGetTextTableStrAddress(0, 19),
                      Bounds.GetPos() + glm::vec2{218.0f, 13.0f}, 26,
                      RendererOutlineMode::BottomRight, 0);
    DescriptionLabel = Label("", Bounds.GetPos() + glm::vec2{218.0f, 43.0f}, 18,
                             RendererOutlineMode::BottomRight, 0);
    Icon = DefaultTrophyIconSprite;
  } else {
    NameLabel = Label(ach->Name(), Bounds.GetPos() + glm::vec2{218.0f, 13.0f},
                      26, RendererOutlineMode::BottomRight, 0);
    DescriptionLabel =
        Label(ach->Description(), Bounds.GetPos() + glm::vec2{218.0f, 43.0f},
              18, RendererOutlineMode::BottomRight, 0);
    Icon = ach->Icon();
  }
  iconDest = {Bounds.X + 112.0f, Bounds.Y + 4.0f, 64.0f, 64.0f};
};

void TrophyMenuEntry::UpdateOffset(glm::vec2 offset) {
  glm::vec2 relativePosition = offset - Offset;
  Move(relativePosition);
  Offset = offset;
}

void TrophyMenuEntry::Update(float dt) { Widget::Update(dt); }

void TrophyMenuEntry::Render() {
  Renderer->DrawSprite(TrophyEntryCardSprite,
                       Bounds.GetPos() + glm::vec2{91.0f, 0.0f});
  Renderer->DrawSprite(Icon, iconDest);
  NameLabel.Render();
  DescriptionLabel.Render();
}

void TrophyMenuEntry::Move(glm::vec2 relativePosition) {
  Widget::Move(relativePosition);

  NameLabel.Move(relativePosition);
  DescriptionLabel.Move(relativePosition);
  iconDest += relativePosition;
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto