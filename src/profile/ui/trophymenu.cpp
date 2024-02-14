#include "trophymenu.h"
#include "../profile_internal.h"
#include "../games/chlcc/trophymenu.h"
#include "../../ui/ui.h"
#include "../../log.h"

namespace Impacto {
namespace Profile {
namespace TrophyMenu {

using namespace Impacto::UI;

TrophyMenuType Type = TrophyMenuType::None;

void Configure() {
  if (TryPushMember("TrophyMenu")) {
    AssertIs(LUA_TTABLE);

    Type =
        TrophyMenuType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

    if (Type == +TrophyMenuType::CHLCC) {
      CHLCC::TrophyMenu::Configure();
    }

    Pop();
  }
}

}  // namespace TrophyMenu
}  // namespace Profile
}  // namespace Impacto