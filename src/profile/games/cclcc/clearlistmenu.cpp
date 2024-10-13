#include "clearlistmenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"
#include "../../../games/cclcc/clearlistmenu.h"
#include "../../../text.h"
#include "../../../game.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace ClearListMenu {

void Configure() {
  ClearListBookLayerSprite = EnsureGetMemberSprite("ClearListBookLayerSprite");
  ClearListGuideSprite = EnsureGetMemberSprite("ClearListGuideSprite");
  LibraryMaskSprite = EnsureGetMemberSprite("LibraryMaskSprite");

  FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
  FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");
  ClearListGuideX = EnsureGetMemberInt("ClearListGuideX");
  ClearListGuideY = EnsureGetMemberInt("ClearListGuideY");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  auto clearList = new UI::CCLCC::ClearListMenu();
  UI::Menus[drawType].push_back(clearList);
}

}  // namespace ClearListMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto