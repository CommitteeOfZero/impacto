#include "backlogmenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"
#include "../../../games/chlcc/backlogmenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace BacklogMenu {

void Configure() {
  BackgroundColor = EnsureGetMember<uint32_t>("BackgroundColor");
  CircleSprite = EnsureGetMember<Sprite>("CircleSprite");
  MenuTitleTextRightPosition =
      EnsureGetMember<glm::vec2>("MenuTitleTextRightPosition");
  MenuTitleTextLeftPosition =
      EnsureGetMember<glm::vec2>("MenuTitleTextLeftPosition");
  MenuTitleTextAngle = EnsureGetMember<float>("MenuTitleTextAngle");
  MenuTitleText = EnsureGetMember<Sprite>("MenuTitleText");

  ButtonPromptPosition = EnsureGetMember<glm::vec2>("ButtonPromptPosition");
  ButtonPromptSprite = EnsureGetMember<Sprite>("BacklogButtonPromptSprite");

  auto drawType = EnsureGetMember<Game::DrawComponentType>("DrawType");

  UI::BacklogMenuPtr = new UI::CHLCC::BacklogMenu();
  UI::Menus[drawType].push_back(UI::BacklogMenuPtr);
}

}  // namespace BacklogMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto