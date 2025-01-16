#include "optionsmenu.h"
#include "../profile_internal.h"
#include "../games/mo6tw/optionsmenu.h"
#include "../games/mo8/optionsmenu.h"
#include "../games/chlcc/optionsmenu.h"
#include "../games/cclcc/optionsmenu.h"
#include "../../ui/ui.h"
#include "../../log.h"

namespace Impacto {
namespace Profile {
namespace OptionsMenu {

using namespace Impacto::UI;

void Configure() {
  if (TryPushMember("OptionsMenu")) {
    AssertIs(LUA_TTABLE);

    Type =
        OptionsMenuType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

    MinButtonHoldTime = EnsureGetMemberFloat("MinButtonHoldTime");
    ButtonHoldTimeInterval = EnsureGetMemberFloat("ButtonHoldTimeInterval");

    FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
    FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

    if (Type != +OptionsMenuType::CHLCC) {
      BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");
    };

    if (Type == +OptionsMenuType::MO6TW) {
      MO6TW::OptionsMenu::Configure();
    } else if (Type == +OptionsMenuType::CHLCC) {
      CHLCC::OptionsMenu::Configure();
    } else if (Type == +OptionsMenuType::CCLCC) {
      CCLCC::OptionsMenu::Configure();
    } else if (Type == +OptionsMenuType::MO8) {
      MO8::OptionsMenu::Configure();
    }

    Pop();
  }
}

}  // namespace OptionsMenu
}  // namespace Profile
}  // namespace Impacto