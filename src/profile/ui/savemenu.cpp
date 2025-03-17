#include "savemenu.h"
#include "../profile_internal.h"
#include "../games/mo6tw/savemenu.h"
#include "../games/mo8/savemenu.h"
#include "../games/chlcc/savemenu.h"
#include "../games/cclcc/savemenu.h"
#include "../../ui/ui.h"

namespace Impacto {
namespace Profile {
namespace SaveMenu {

using namespace Impacto::UI;

void Configure() {
  if (TryPushMember("SaveMenu")) {
    AssertIs(LUA_TTABLE);

    Type = SaveMenuType::_from_integral_unchecked(EnsureGetMember<int>("Type"));

    EmptyThumbnailSprite = EnsureGetMember<Sprite>("EmptyThumbnailSprite");

    FadeInDuration = EnsureGetMember<float>("FadeInDuration");
    FadeOutDuration = EnsureGetMember<float>("FadeOutDuration");

    if (Type == +SaveMenuType::MO6TW) {
      MO6TW::SaveMenu::Configure();
    } else if (Type == +SaveMenuType::CHLCC) {
      CHLCC::SaveMenu::Configure();
    } else if (Type == +SaveMenuType::CCLCC) {
      CCLCC::SaveMenu::Configure();
    } else if (Type == +SaveMenuType::MO8) {
      MO8::SaveMenu::Configure();
    }

    Pop();
  }
}

}  // namespace SaveMenu
}  // namespace Profile
}  // namespace Impacto