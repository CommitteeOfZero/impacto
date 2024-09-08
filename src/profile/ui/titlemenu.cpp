#include "titlemenu.h"
#include "../profile_internal.h"
#include "../games/rne/tilebackground.h"
#include "../games/rne/titlemenu.h"
#include "../games/dash/titlemenu.h"
#include "../games/chlcc/titlemenu.h"
#include "../games/mo6tw/titlemenu.h"
#include "../games/mo8/titlemenu.h"
#include "../games/cc/titlemenu.h"
#include "../games/cclcc/titlemenu.h"
#include "../games/sg/titlemenu.h"
#include "../../log.h"

namespace Impacto {
namespace Profile {
namespace TitleMenu {

using namespace Impacto::UI;

void Configure() {
  if (TryPushMember("TitleMenu")) {
    AssertIs(LUA_TTABLE);

    Type = TitleMenuType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

    MenuEntriesNum = EnsureGetMemberInt("MenuEntriesNum");
    if (MenuEntriesNum > 0) {
      GetMemberSpriteArray(MenuEntriesSprites, MenuEntriesNum,
                           "MenuEntriesSprites");
      GetMemberSpriteArray(MenuEntriesHSprites, MenuEntriesNum,
                           "MenuEntriesHighlightedSprites");
    }

    PressToStartSprite = EnsureGetMemberSprite("PressToStartSprite");

    PressToStartAnimDurationIn =
        EnsureGetMemberFloat("PressToStartAnimDurationIn");
    PressToStartAnimDurationOut =
        EnsureGetMemberFloat("PressToStartAnimDurationOut");

    PressToStartX = EnsureGetMemberFloat("PressToStartX");
    PressToStartY = EnsureGetMemberFloat("PressToStartY");

    if (Type == +TitleMenuType::RNE) {
      RNE::TitleMenu::Configure();
    } else if (Type == +TitleMenuType::Dash) {
      Dash::TitleMenu::Configure();
    } else if (Type == +TitleMenuType::CHLCC) {
      CHLCC::TitleMenu::Configure();
    } else if (Type == +TitleMenuType::MO6TW) {
      MO6TW::TitleMenu::Configure();
    } else if (Type == +TitleMenuType::MO8) {
      MO8::TitleMenu::Configure();
    } else if (Type == +TitleMenuType::CC) {
      CC::TitleMenu::Configure();
    } else if (Type == +TitleMenuType::CCLCC) {
      CCLCC::TitleMenu::Configure();
    } else if (Type == +TitleMenuType::SG) {
      SG::TitleMenu::Configure();
    } else {
      ImpLog(LL_Fatal, LC_Profile, "Unknown title menu type in profile!\n");
    }

    // if (Implementation != 0) {
    //  Implementation->PressToStartAnimation.DurationIn =
    //      PressToStartAnimDurationIn;
    //  Implementation->PressToStartAnimation.DurationOut =
    //      PressToStartAnimDurationOut;
    //  Implementation->PressToStartAnimation.LoopMode = ALM_ReverseDirection;
    //}

    Pop();
  }
}

}  // namespace TitleMenu
}  // namespace Profile
}  // namespace Impacto