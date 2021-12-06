#include "musicmenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"

#include "../../../games/mo6tw/musicmenu.h"
#include "../../../text.h"
#include "../../../game.h"
#include "../../../window.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace MusicMenu {

Sprite BackgroundSprite;

float FadeInDuration;
float FadeOutDuration;

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
  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");

  FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
  FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::Menus[drawType].push_back(new UI::MO6TW::MusicMenu());
}

}  // namespace MusicMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto
