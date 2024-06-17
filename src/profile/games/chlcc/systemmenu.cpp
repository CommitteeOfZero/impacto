#include "systemmenu.h"
#include "../../profile_internal.h"
#include "../../../renderer/renderer.h"
#include "../../../log.h"

#include "../../../games/chlcc/systemmenu.h"
#include "../../ui/systemmenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace SystemMenu {

void Configure() {
  BackgroundColor = EnsureGetMemberUint("BackgroundColor");
  BackgroundFilter = EnsureGetMemberSprite("BackgroundFilter");
  CircleStartPosition = EnsureGetMemberVec2("CircleStartPosition");
  CircleSprite = EnsureGetMemberSprite("CircleSprite");
  CircleOffset = EnsureGetMemberFloat("CircleOffset");
  ErinPosition = EnsureGetMemberVec2("ErinPosition");
  ErinSprite = EnsureGetMemberSprite("ErinSprite");
  TitleFadeInDuration = EnsureGetMemberFloat("TitleFadeInDuration");
  TitleFadeOutDuration = EnsureGetMemberFloat("TitleFadeOutDuration");
  FocusTint = EnsureGetMemberUint("FocusTint");
  GetMemberVec2Array(MenuEntriesPositions, Profile::SystemMenu::MenuEntriesNum,
                     "MenuEntriesPositions");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::SystemMenuPtr = new UI::CHLCC::SystemMenu();
  UI::Menus[drawType].push_back(UI::SystemMenuPtr);
}

}  // namespace SystemMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto