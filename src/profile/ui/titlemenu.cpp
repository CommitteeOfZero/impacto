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
#include "../../log.h"

namespace Impacto {
namespace Profile {
namespace TitleMenu {

using namespace Impacto::UI;

void Configure() {
  if (TryPushMember("TitleMenu")) {
    AssertIs(LUA_TTABLE);

    Type = EnsureGetMember<TitleMenuType>("Type");

    MenuEntriesNum = EnsureGetMember<int>("MenuEntriesNum");
    if (MenuEntriesNum > 0) {
      GetMemberArray<Sprite>(std::span(MenuEntriesSprites, MenuEntriesNum),
                             "MenuEntriesSprites");
      GetMemberArray<Sprite>(std::span(MenuEntriesHSprites, MenuEntriesNum),
                             "MenuEntriesHighlightedSprites");
    }

    PressToStartSprite = EnsureGetMember<Sprite>("PressToStartSprite");

    PressToStartAnimDurationIn =
        EnsureGetMember<float>("PressToStartAnimDurationIn");
    PressToStartAnimDurationOut =
        EnsureGetMember<float>("PressToStartAnimDurationOut");

    PressToStartPos = EnsureGetMember<glm::vec2>("PressToStartPos");

    switch (Type) {
      case TitleMenuType::RNE:
        RNE::TitleMenu::Configure();
        break;
      case TitleMenuType::Dash:
        Dash::TitleMenu::Configure();
        break;
      case TitleMenuType::CHLCC:
      case TitleMenuType::CHLCC_Switch:
        CHLCC::TitleMenu::Configure();
        break;
      case TitleMenuType::MO6TW:
        MO6TW::TitleMenu::Configure();
        break;
      case TitleMenuType::MO8:
        MO8::TitleMenu::Configure();
        break;
      case TitleMenuType::CC:
        CC::TitleMenu::Configure();
        break;
      case TitleMenuType::CCLCC:
        CCLCC::TitleMenu::Configure();
        break;
      default:
        break;
    }

    // if (Implementation != 0) {
    //  Implementation->PressToStartAnimation.DurationIn =
    //      PressToStartAnimDurationIn;
    //  Implementation->PressToStartAnimation.DurationOut =
    //      PressToStartAnimDurationOut;
    //  Implementation->PressToStartAnimation.LoopMode = ReverseDirection;
    //}

    Pop();
  }
}

}  // namespace TitleMenu
}  // namespace Profile
}  // namespace Impacto