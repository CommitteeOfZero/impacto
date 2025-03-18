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

void Configure() {
  MenuEntriesHighlightedSprite =
      EnsureGetMember<Sprite>("MenuEntriesHighlightedSprite");
  SystemMenuBackgroundSprite =
      EnsureGetMember<Sprite>("SystemMenuBackgroundSprite");
  MenuEntriesTargetWidth = EnsureGetMember<float>("MenuEntriesTargetWidth");
  SystemMenuX = EnsureGetMember<float>("SystemMenuX");
  SystemMenuY = EnsureGetMember<float>("SystemMenuY");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  UI::SystemMenuPtr = new UI::MO6TW::SystemMenu();
  UI::Menus[drawType].push_back(UI::SystemMenuPtr);
}

}  // namespace SystemMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto
