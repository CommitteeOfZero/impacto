#include "savemenu.h"
#include "../profile_internal.h"
#include "../games/mo6tw/savemenu.h"
#include "../games/mo8/savemenu.h"
#include "../games/chlcc/savemenu.h"
#include "../games/cclcc/savemenu.h"
#include "../../ui/ui.h"
#include "../../log.h"
//#include "../../ui/nullmenu.h"

namespace Impacto {
namespace Profile {
namespace SaveMenu {

using namespace Impacto::UI;

SaveMenuType Type = SaveMenuType::None;

Sprite SaveMenuBackgroundSprite;
Sprite EmptyThumbnailSprite;

float EntryStartX;
float EntryXPadding;
float EntryStartY;
float EntryYPadding;

float FadeInDuration;
float FadeOutDuration;

void Configure() {
  if (TryPushMember("SaveMenu")) {
    AssertIs(LUA_TTABLE);

    Type = SaveMenuType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

    SaveMenuBackgroundSprite =
        EnsureGetMemberSprite("SaveMenuBackgroundSprite");
    EmptyThumbnailSprite = EnsureGetMemberSprite("EmptyThumbnailSprite");

    EntryStartX = EnsureGetMemberFloat("EntryStartX");
    EntryXPadding = EnsureGetMemberFloat("EntryXPadding");
    EntryStartY = EnsureGetMemberFloat("EntryStartY");
    EntryYPadding = EnsureGetMemberFloat("EntryYPadding");

    FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
    FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

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