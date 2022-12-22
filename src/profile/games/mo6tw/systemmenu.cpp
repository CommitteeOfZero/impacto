#include "systemmenu.h"
#include "../../profile_internal.h"
#include "../../../log.h"

#include "../../ui/systemmenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/mo6tw/systemmenu.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace SystemMenu {

Sprite SystemMenuBackgroundSprite;
Sprite MenuEntriesHighlightedSprite;

float MenuEntriesTargetWidth;
float SystemMenuX;
float SystemMenuY;

void Configure() {
  MenuEntriesHighlightedSprite =
      EnsureGetMemberSprite("MenuEntriesHighlightedSprite");
  SystemMenuBackgroundSprite =
      EnsureGetMemberSprite("SystemMenuBackgroundSprite");
  MenuEntriesTargetWidth = EnsureGetMemberFloat("MenuEntriesTargetWidth");
  SystemMenuX = EnsureGetMemberFloat("SystemMenuX");
  SystemMenuY = EnsureGetMemberFloat("SystemMenuY");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::SystemMenuPtr = new UI::MO6TW::SystemMenu();
  UI::Menus[drawType].push_back(UI::SystemMenuPtr);
}

}  // namespace SystemMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto
