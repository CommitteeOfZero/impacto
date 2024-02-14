#include "tipsmenu.h"
#include "../../profile_internal.h"
#include "../../../log.h"
#include "../../../text.h"

#include "../../ui/tipsmenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/cclcc/tipsmenu.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace TipsMenu {

Sprite TipsBookLayerSprite;
Sprite TipsGuideSprite;
Sprite TipsMaskSprite;

int TipsGuideX;
int TipsGuideY;

void Configure() {
  TipsBookLayerSprite = EnsureGetMemberSprite("TipsBookLayerSprite");
  TipsGuideSprite = EnsureGetMemberSprite("TipsGuideSprite");
  TipsMaskSprite = EnsureGetMemberSprite("TipsMaskSprite");

  TipsGuideX = EnsureGetMemberInt("TipsGuideX");
  TipsGuideY = EnsureGetMemberInt("TipsGuideY");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::TipsMenuPtr = new UI::CCLCC::TipsMenu();
  UI::Menus[drawType].push_back(UI::TipsMenuPtr);
}

}  // namespace TipsMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto
