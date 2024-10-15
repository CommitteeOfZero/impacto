#include "systemmenu.h"
#include "../../profile_internal.h"
#include "../../../renderer/renderer.h"
#include "../../../log.h"

#include "../../../games/cclcc/systemmenu.h"
#include "../../ui/systemmenu.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace SystemMenu {

void Configure() {
  MoveInDuration = EnsureGetMemberFloat("MoveInDuration");
  MoveOutDuration = EnsureGetMemberFloat("MoveOutDuration");
  ItemsFadeInDuration = EnsureGetMemberFloat("ItemsFadeInDuration");
  ItemsFadeOutDuration = EnsureGetMemberFloat("ItemsFadeOutDuration");

  GetMemberVec2Array(MenuEntriesPositions, Profile::SystemMenu::MenuEntriesNum,
                     "MenuEntriesPositions");

  GetMemberRectFArray(MenuEntriesButtonBounds,
                      Profile::SystemMenu::MenuEntriesNum,
                      "MenuEntriesButtonBounds");

  SystemMenuBG = EnsureGetMemberSprite("SystemMenuBG");
  SystemMenuFrame = EnsureGetMemberSprite("SystemMenuFrame");
  MenuButtonGuide = EnsureGetMemberSprite("MenuButtonGuide");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::SystemMenuPtr = new UI::CCLCC::SystemMenu();
  UI::Menus[drawType].push_back(UI::SystemMenuPtr);
}

}  // namespace SystemMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto