#include "trophymenuentry.h"
#include "achievementsystem.h"
#include "trophymenu.h"
#include "../../profile/games/chlcc/trophymenu.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::TrophyMenu;
using namespace Impacto::AchievementSystem;

TrophyMenuEntry::TrophyMenuEntry(int achievementId)
    : AchievementId(achievementId), Offset(0.0f, -720.0f) {
  Position = glm::vec2(0.0f, 74.0f * (AchievementId % 6) + 130) + Offset;

  const auto* ach = AchievementSystem::GetAchievement(AchievementId);
  if (ach == nullptr) {
    NameLabel =
        Label(Vm::ScriptGetTextTableStrAddress(0, 19),
              Position + glm::vec2{218.0f, 13.0f}, 26, RO_BottomRight, 0);
    DescriptionLabel =
        Label("", Position + glm::vec2{218.0f, 43.0f}, 18, RO_BottomRight, 0);
    Icon = DefaultTrophyIconSprite;
  } else {
    NameLabel = Label(ach->Name(), Position + glm::vec2{218.0f, 13.0f}, 26,
                      RO_BottomRight, 0);
    DescriptionLabel =
        Label(ach->Description(), Position + glm::vec2{218.0f, 43.0f}, 18,
              RO_BottomRight, 0);
    Icon = ach->Icon();
  }
  iconDest = {Position.x + 112.0f, Position.y + 4.0f, 64.0f, 64.0f};
};

void TrophyMenuEntry::UpdateOffset(glm::vec2 offset) {
  glm::vec2 relativePosition = offset - Offset;
  Move(relativePosition);
  Offset = offset;
}

void TrophyMenuEntry::Update(float dt) { Widget::Update(dt); }

void TrophyMenuEntry::Render() {
  Renderer->DrawSprite(Icon, iconDest);
  NameLabel.Render();
  DescriptionLabel.Render();
}

void TrophyMenuEntry::Move(glm::vec2 relativePosition) {
  Widget::Move(relativePosition);
  NameLabel.Move(relativePosition);
  DescriptionLabel.Move(relativePosition);
  iconDest.X += relativePosition.x;
  iconDest.Y += relativePosition.y;
}

void TrophyMenuEntry::MoveTo(glm::vec2 pos) {
  Widget::MoveTo(pos);
  NameLabel.MoveTo(pos + glm::vec2{218.0f, 13.0f});
  DescriptionLabel.MoveTo(pos + glm::vec2{218.0f, 43.0f});
  iconDest.X = pos.x + 112.0f;
  iconDest.Y = pos.y + 4.0f;
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto