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

TrophyMenuEntry::TrophyMenuEntry(int achievementId, float heightOffset)
    : AchievementId(achievementId) {
  Position = glm::vec2(0.0f, 74.0f * (AchievementId % 6) + heightOffset + 130);

  const auto* ach = AchievementSystem::GetAchievement(AchievementId);
  if (ach == nullptr) {
    NameLabel = Label(Vm::ScriptGetTextTableStrAddress(0, 19),
                      Position + glm::vec2{218.0f, 13.0f}, 26, RO_None, 0);
    DescriptionLabel =
        Label("", Position + glm::vec2{218.0f, 43.0f}, 18, RO_None, 0);
    Icon = DefaultTrophyIconSprite;
  } else {
    NameLabel =
        Label(ach->Name(), Position + glm::vec2{218.0f, 13.0f}, 26, RO_None, 0);
    DescriptionLabel =
        Label(ach->Description(), Position + glm::vec2{218.0f, 43.0f}, 18,
              RO_None, 0);
    Icon = ach->Icon();
  }
  iconDest = {Position.x + 112.0f, Position.y + 4.0f, 64.0f, 64.0f};
};

void TrophyMenuEntry::Update(float dt) { Widget::Update(dt); }

void TrophyMenuEntry::Render() {
  Renderer->DrawSprite(Icon, iconDest);
  NameLabel.Render();
  DescriptionLabel.Render();
}

void TrophyMenuEntry::Move(glm::vec2 relativePosition) {
  Widget::Move(relativePosition);
}

void TrophyMenuEntry::MoveTo(glm::vec2 pos) { Widget::MoveTo(pos); }

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto