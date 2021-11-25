#include "tipsmenu.h"
#include "../../profile_internal.h"
#include "../../../window.h"
#include "../../../log.h"

#include "../../ui/tipsmenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/mo6tw/tipsmenu.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace TipsMenu {

Sprite TipThumbnails[ThumbnailCount];
Sprite TipTextOnlyThumbnail;

static void GetMemberSpriteArray(Sprite* arr, uint32_t count,
                                 char const* name) {
  EnsurePushMemberOfType(name, kArrayType);

  if (TopVal().Size() != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d sprites for %s\n", count,
           name);
    Window::Shutdown();
  }

  for (uint32_t i = 0; i < count; i++) {
    arr[i] = EnsureGetArrayElementSprite(i);
  }

  Pop();
}

void Configure() {
  GetMemberSpriteArray(TipThumbnails, 37, "Thumbnails");
  TipTextOnlyThumbnail = EnsureGetMemberSprite("TextOnlyThumbnail");

  UI::TipsMenuPtr = new UI::MO6TW::TipsMenu();
}

}  // namespace TipsMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto
