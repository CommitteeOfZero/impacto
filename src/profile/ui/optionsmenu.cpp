#include "optionsmenu.h"
#include "../profile_internal.h"
#include "../games/mo6tw/optionsmenu.h"
#include "../games/chlcc/optionsmenu.h"
#include "../games/cclcc/optionsmenu.h"
#include "../../ui/ui.h"
#include "../../log.h"

namespace Impacto {
namespace Profile {
namespace OptionsMenu {

using namespace Impacto::UI;

OptionsMenuType Type = OptionsMenuType::None;

Sprite BackgroundSprite;
Sprite SliderTrackSprite;
Sprite SliderFillSprite;
Sprite SliderThumbSprite;

float FadeInDuration;
float FadeOutDuration;

void Configure() {
  if (TryPushMember("OptionsMenu")) {
    AssertIs(LUA_TTABLE);

    Type =
        OptionsMenuType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

    if (Type != +OptionsMenuType::CHLCC) {
      FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
      FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

      BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");
      SliderTrackSprite = EnsureGetMemberSprite("SliderTrackSprite");
      SliderFillSprite = EnsureGetMemberSprite("SliderFillSprite");
      SliderThumbSprite = EnsureGetMemberSprite("SliderThumbSprite");
    };

    if (Type == +OptionsMenuType::MO6TW) {
      MO6TW::OptionsMenu::Configure();
    } else if (Type == +OptionsMenuType::CHLCC) {
      CHLCC::OptionsMenu::Configure();
    } else if (Type == +OptionsMenuType::CCLCC) {
      CCLCC::OptionsMenu::Configure();
    }

    Pop();
  }
}

}  // namespace OptionsMenu
}  // namespace Profile
}  // namespace Impacto