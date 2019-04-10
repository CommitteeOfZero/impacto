#include "titlemenu.h"
#include "../profile_internal.h"
#include "../games/rne/tilebackground.h"
#include "../games/rne/titlemenu.h"
#include "../games/dash/titlemenu.h"
#include "../../log.h"
#include "../../window.h"

namespace Impacto {
namespace Profile {
namespace TitleMenu {

using namespace Impacto::TitleMenu;

TitleMenuType Type = TitleMenuType::None;

Sprite PressToStartSprite;

float PressToStartAnimDurationIn;
float PressToStartAnimDurationOut;

float PressToStartX;
float PressToStartY;

void Configure() {
  if (TryPushMember("TitleMenu")) {
    AssertIs(kObjectType);

    Type = TitleMenuType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

    if (Type == +TitleMenuType::RNE) {
      Implementation = RNE::TitleMenu::Configure();
    } else if (Type == +TitleMenuType::Dash) {
      Implementation = Dash::TitleMenu::Configure();
    }

    PressToStartSprite = EnsureGetMemberSprite("PressToStartSprite");

    PressToStartAnimDurationIn =
        EnsureGetMemberFloat("PressToStartAnimDurationIn");
    PressToStartAnimDurationOut =
        EnsureGetMemberFloat("PressToStartAnimDurationOut");

    Implementation->PressToStartAnimation.DurationIn =
        PressToStartAnimDurationIn;
    Implementation->PressToStartAnimation.DurationOut =
        PressToStartAnimDurationOut;
    Implementation->PressToStartAnimation.LoopMode = ALM_ReverseDirection;

    PressToStartX = EnsureGetMemberFloat("PressToStartX");
    PressToStartY = EnsureGetMemberFloat("PressToStartY");
    Pop();
  }
}

}  // namespace TitleMenu
}  // namespace Profile
}  // namespace Impacto