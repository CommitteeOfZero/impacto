#include "tipsmenu.h"
#include "../profile_internal.h"
#include "../games/mo6tw/tipsmenu.h"
#include "../games/chlcc/tipsmenu.h"
#include "../games/cclcc/tipsmenu.h"
#include "../../ui/ui.h"
#include "../../log.h"

namespace Impacto {
namespace Profile {
namespace TipsMenu {

using namespace Impacto::UI;

void Configure() {
  if (TryPushMember("TipsMenu")) {
    AssertIs(LUA_TTABLE);

    Type = TipsMenuType::_from_integral_unchecked(EnsureGetMember<int>("Type"));

    if (Type != +TipsMenuType::None && Type != +TipsMenuType::CHLCC) {
      FadeInDuration = EnsureGetMember<float>("FadeInDuration");
      FadeOutDuration = EnsureGetMember<float>("FadeOutDuration");

      BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");
    }

    if (Type == +TipsMenuType::MO6TW) {
      MO6TW::TipsMenu::Configure();
    } else if (Type == +TipsMenuType::CHLCC) {
      CHLCC::TipsMenu::Configure();
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