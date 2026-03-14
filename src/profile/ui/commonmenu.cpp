#include "commonmenu.h"
#include "../profile_internal.h"
#include "../../ui/ui.h"
#include "../games/chlcc/commonmenu.h"

namespace Impacto {
namespace Profile {
namespace CommonMenu {

void Configure() {
  if (TryPushMember("CommonMenu")) {
    AssertIs(LUA_TTABLE);

    Type =
        CommonMenuType::_from_integral_unchecked(EnsureGetMember<int>("Type"));

    if (Type == +CommonMenuType::CHLCC) {
      CHLCC::CommonMenu::Configure();
    }

    Pop();
  }
}

}  // namespace CommonMenu
}  // namespace Profile
}  // namespace Impacto