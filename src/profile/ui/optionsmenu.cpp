#include "optionsmenu.h"
#include "../profile_internal.h"
#include "../games/mo6tw/optionsmenu.h"
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
    AssertIs(kObjectType);

    Type =
        OptionsMenuType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

    FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
    FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

    BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");
    SliderTrackSprite = EnsureGetMemberSprite("SliderTrackSprite");
    SliderFillSprite = EnsureGetMemberSprite("SliderFillSprite");
    SliderThumbSprite = EnsureGetMemberSprite("SliderThumbSprite");

    if (Type == +OptionsMenuType::MO6TW) {
      MO6TW::OptionsMenu::Configure();
    }

    Pop();
  }
}

}  // namespace OptionsMenu
}  // namespace Profile
}  // namespace Impacto