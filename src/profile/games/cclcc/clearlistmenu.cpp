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
  ClearListBookLayerSprite =
      EnsureGetMember<Sprite>("ClearListBookLayerSprite");
  ClearListGuideSprite = EnsureGetMember<Sprite>("ClearListGuideSprite");
  LibraryMaskSprite = EnsureGetMember<Sprite>("LibraryMaskSprite");

  FadeInDuration = EnsureGetMember<float>("FadeInDuration");
  FadeOutDuration = EnsureGetMember<float>("FadeOutDuration");
  ClearListGuideX = EnsureGetMember<int>("ClearListGuideX");
  ClearListGuideY = EnsureGetMember<int>("ClearListGuideY");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  auto clearList = new UI::CCLCC::ClearListMenu();
  UI::Menus[drawType].push_back(clearList);
}

}  // namespace ClearListMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto