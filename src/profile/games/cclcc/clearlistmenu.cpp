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

Sprite BackgroundSprite;
Sprite ClearListBookLayerSprite;
Sprite ClearListGuideSprite;
Sprite ClearListMaskSprite;

float FadeInDuration;
float FadeOutDuration;
int ClearListGuideX;
int ClearListGuideY;

void Configure() {
  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");
  ClearListBookLayerSprite = EnsureGetMemberSprite("ClearListBookLayerSprite");
  ClearListGuideSprite = EnsureGetMemberSprite("ClearListGuideSprite");
  ClearListMaskSprite = EnsureGetMemberSprite("ClearListMaskSprite");

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