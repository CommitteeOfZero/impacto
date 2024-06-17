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
  MenuEntriesLNum = EnsureGetMemberInt("MenuEntriesLNum");
  if (MenuEntriesLNum > 0) {
    GetMemberSpriteArray(MenuEntriesLSprites, MenuEntriesLNum,
                         "MenuEntriesLockedSprites");
  }
  ExitMenuButtonId = EnsureGetMemberInt("ExitMenuButtonId");
  SystemMenuBackgroundSprite =
      EnsureGetMemberSprite("SystemMenuBackgroundSprite");
  MenuEntriesTargetWidth = EnsureGetMemberFloat("MenuEntriesTargetWidth");
  SystemMenuX = EnsureGetMemberFloat("SystemMenuX");
  SystemMenuY = EnsureGetMemberFloat("SystemMenuY");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::SystemMenuPtr = new UI::MO8::SystemMenu();
  UI::Menus[drawType].push_back(UI::SystemMenuPtr);
}

}  // namespace SystemMenu
}  // namespace MO8
}  // namespace Profile
}  // namespace Impacto
