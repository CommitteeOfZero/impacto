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
    ButtonHoldFireInterval = EnsureGetMemberFloat("ButtonHoldFireInterval");

    FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
    FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

    switch (Type) {
      case OptionsMenuType::MO6TW:
        MO6TW::OptionsMenu::Configure();
        break;
      case OptionsMenuType::MO8:
        MO8::OptionsMenu::Configure();
        break;
      case OptionsMenuType::CHLCC:
        CHLCC::OptionsMenu::Configure();
        break;
      case OptionsMenuType::CCLCC:
        CCLCC::OptionsMenu::Configure();
        break;
    }

    Pop();
  }
}

}  // namespace OptionsMenu
}  // namespace Profile
}  // namespace Impacto