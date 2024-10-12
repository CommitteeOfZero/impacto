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
      EnsureGetMemberInt("BacklogBackgroundRepeatHeight");

  BacklogHeaderSprite = EnsureGetMemberSprite("BacklogHeaderSprite");
  BacklogHeaderPosition = EnsureGetMemberVec2("BacklogHeaderPosition");

  BacklogControlsSprite = EnsureGetMemberSprite("BacklogControlsSprite");
  BacklogControlsPosition = EnsureGetMemberVec2("BacklogControlsPosition");

  MenuMaskSprite = EnsureGetMemberSprite("MenuMask");
  BacklogMaskSheet = EnsureGetMemberSpriteSheet("BacklogMask");

  FadeInDirectDuration = EnsureGetMemberFloat("FadeInDirectDuration");
  FadeOutDirectDuration = EnsureGetMemberFloat("FadeOutDirectDuration");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::BacklogMenuPtr = new UI::CC::BacklogMenu();
  UI::Menus[drawType].push_back(UI::BacklogMenuPtr);
}

}  // namespace BacklogMenu
}  // namespace CC
}  // namespace Profile
}  // namespace Impacto