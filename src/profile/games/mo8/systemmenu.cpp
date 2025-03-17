#include "systemmenu.h"
#include "../../profile_internal.h"
#include "../../../log.h"

#include "../../ui/systemmenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/mo8/systemmenu.h"

namespace Impacto {
namespace Profile {
namespace MO8 {
namespace SystemMenu {

void Configure() {
  MenuEntriesLNum = EnsureGetMember<int>("MenuEntriesLNum");
  if (MenuEntriesLNum > 0) {
    GetMemberArray<Sprite>(MenuEntriesLSprites, MenuEntriesLNum,
                           "MenuEntriesLockedSprites");
  }
  ExitMenuButtonId = EnsureGetMember<int>("ExitMenuButtonId");
  SystemMenuBackgroundSprite =
      EnsureGetMember<Sprite>("SystemMenuBackgroundSprite");
  MenuEntriesTargetWidth = EnsureGetMember<float>("MenuEntriesTargetWidth");
  SystemMenuX = EnsureGetMember<float>("SystemMenuX");
  SystemMenuY = EnsureGetMember<float>("SystemMenuY");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  UI::SystemMenuPtr = new UI::MO8::SystemMenu();
  UI::Menus[drawType].push_back(UI::SystemMenuPtr);
}

}  // namespace SystemMenu
}  // namespace MO8
}  // namespace Profile
}  // namespace Impacto
