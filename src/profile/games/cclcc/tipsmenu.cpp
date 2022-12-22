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

static void GetMemberSpriteArray(Sprite* arr, uint32_t count,
                                 char const* name) {
  EnsurePushMemberOfType(name, kArrayType);

  if (TopVal().Size() != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d sprites for %s\n", count,
           name);
    Renderer->Window->Shutdown();
  }

  for (uint32_t i = 0; i < count; i++) {
    arr[i] = EnsureGetArrayElementSprite(i);
  }

  Pop();
}

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
