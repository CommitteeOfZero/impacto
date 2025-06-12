#include "backlogmenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"
#include "../../../games/cc/backlogmenu.h"

namespace Impacto {
namespace Profile {
namespace CC {
namespace BacklogMenu {

void Configure() {
  BacklogBackgroundRepeatHeight =
      EnsureGetMember<int>("BacklogBackgroundRepeatHeight");

  BacklogHeaderSprite = EnsureGetMember<Sprite>("BacklogHeaderSprite");
  BacklogHeaderPosition = EnsureGetMember<glm::vec2>("BacklogHeaderPosition");

  BacklogControlsSprite = EnsureGetMember<Sprite>("BacklogControlsSprite");
  BacklogControlsPosition =
      EnsureGetMember<glm::vec2>("BacklogControlsPosition");

  MenuMaskSprite = EnsureGetMember<Sprite>("MenuMask");
  BacklogMaskSheet = EnsureGetMember<SpriteSheet>("BacklogMask");

  FadeInDirectDuration = EnsureGetMember<float>("FadeInDirectDuration");
  FadeOutDirectDuration = EnsureGetMember<float>("FadeOutDirectDuration");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  UI::BacklogMenuPtr = new UI::CC::BacklogMenu();
  UI::Menus[drawType].push_back(UI::BacklogMenuPtr);
}

}  // namespace BacklogMenu
}  // namespace CC
}  // namespace Profile
}  // namespace Impacto