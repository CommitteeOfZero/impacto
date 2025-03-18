#include "sysmesbox.h"
#include "../games/rne/sysmesbox.h"
#include "../games/chlcc/sysmesbox.h"
#include "../games/mo6tw/sysmesbox.h"
#include "../games/darling/sysmesbox.h"
#include "../games/cc/sysmesbox.h"

#include "../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace SysMesBox {

using namespace Impacto::UI;

void Configure() {
  if (TryPushMember("SysMesBoxDisplay")) {
    AssertIs(LUA_TTABLE);

    Type =
        SysMesBoxType::_from_integral_unchecked(EnsureGetMember<int>("Type"));

    if (Type == +SysMesBoxType::RNE || Type == +SysMesBoxType::Dash) {
      RNE::SysMesBox::Configure();
    } else if (Type == +SysMesBoxType::CHLCC) {
      CHLCC::SysMesBox::Configure();
    } else if (Type == +SysMesBoxType::MO6TW) {
      MO6TW::SysMesBox::Configure();
    } else if (Type == +SysMesBoxType::Darling) {
      Darling::SysMesBox::Configure();
    } else if (Type == +SysMesBoxType::CC) {
      CC::SysMesBox::Configure();
    }

    TextFontSize = EnsureGetMember<float>("TextFontSize");
    TextMiddleY = EnsureGetMember<float>("TextMiddleY");
    TextX = EnsureGetMember<float>("TextX");
    TextLineHeight = EnsureGetMember<float>("TextLineHeight");
    TextMarginY = EnsureGetMember<float>("TextMarginY");
    AnimationSpeed = EnsureGetMember<float>("AnimationSpeed");
    FadeInDuration = EnsureGetMember<float>("FadeInDuration");
    FadeOutDuration = EnsureGetMember<float>("FadeOutDuration");

    // Implementation->FadeAnimation.DurationIn = FadeInDuration;
    // Implementation->FadeAnimation.DurationOut = FadeOutDuration;

    Pop();
  }
}

}  // namespace SysMesBox
}  // namespace Profile
}  // namespace Impacto