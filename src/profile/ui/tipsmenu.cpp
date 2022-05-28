#include "tipsmenu.h"
#include "../profile_internal.h"
#include "../games/mo6tw/tipsmenu.h"
#include "../games/cclcc/tipsmenu.h"
#include "../../ui/ui.h"
#include "../../log.h"
#include "../../window.h"

namespace Impacto {
namespace Profile {
namespace TipsMenu {

using namespace Impacto::UI;

TipsMenuType Type = TipsMenuType::None;

Sprite BackgroundSprite;

float FadeInDuration;
float FadeOutDuration;

void Configure() {
  if (TryPushMember("TipsMenu")) {
    AssertIs(kObjectType);

    Type = TipsMenuType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

    if (Type != +TipsMenuType::None) {
      FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
      FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

      BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");
    }

    if (Type == +TipsMenuType::MO6TW) {
      MO6TW::TipsMenu::Configure();
    } else if (Type == +TipsMenuType::CCLCC) {
      CCLCC::TipsMenu::Configure();
    } else {
      UI::TipsMenuPtr = new UI::TipsMenu();
    }

    Pop();
  }
}

}  // namespace TipsMenu
}  // namespace Profile
}  // namespace Impacto