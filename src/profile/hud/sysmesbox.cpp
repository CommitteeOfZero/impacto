#include "sysmesbox.h"
#include "../games/rne/sysmesbox.h"
#include "../games/chlcc/sysmesbox.h"
#include "../games/mo6tw/sysmesbox.h"
#include "../games/darling/sysmesbox.h"

#include "../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace SysMesBox {

using namespace Impacto::SysMesBox;

SysMesBoxType Type = SysMesBoxType::None;

float TextFontSize;
float TextMiddleY;
float TextX;
float TextLineHeight;
float TextMarginY;
float AnimationSpeed;
float FadeInDuration;
float FadeOutDuration;

void Configure() {
  if (TryPushMember("SysMesBoxDisplay")) {
    AssertIs(kObjectType);

    Type = SysMesBoxType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

    if (Type == +SysMesBoxType::RNE || Type == +SysMesBoxType::Dash) {
      Implementation = RNE::SysMesBox::Configure();
    } else if (Type == +SysMesBoxType::CHLCC) {
      Implementation = CHLCC::SysMesBox::Configure();
    } else if (Type == +SysMesBoxType::MO6TW) {
      Implementation = MO6TW::SysMesBox::Configure();
    } else if (Type == +SysMesBoxType::Darling) {
      Implementation = Darling::SysMesBox::Configure();
    }

    TextFontSize = EnsureGetMemberFloat("TextFontSize");
    TextMiddleY = EnsureGetMemberFloat("TextMiddleY");
    TextX = EnsureGetMemberFloat("TextX");
    TextLineHeight = EnsureGetMemberFloat("TextLineHeight");
    TextMarginY = EnsureGetMemberFloat("TextMarginY");
    AnimationSpeed = EnsureGetMemberFloat("AnimationSpeed");
    FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
    FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

    Implementation->FadeAnimation.DurationIn = FadeInDuration;
    Implementation->FadeAnimation.DurationOut = FadeOutDuration;

    Pop();
  }
}

}  // namespace SysMesBox
}  // namespace Profile
}  // namespace Impacto