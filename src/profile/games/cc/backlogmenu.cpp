#include "backlogmenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"

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

  BacklogAlphaMask = EnsureGetMemberSprite("BacklogAlphaMask");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::BacklogMenuPtr = new UI::BacklogMenu();
  UI::Menus[drawType].push_back(UI::BacklogMenuPtr);
}

}  // namespace BacklogMenu
}  // namespace CC
}  // namespace Profile
}  // namespace Impacto