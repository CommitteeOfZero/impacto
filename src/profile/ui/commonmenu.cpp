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

    Type = EnsureGetMember<CommonMenuType>("Type");

    if (Type == CommonMenuType::CHLCC) {
      CHLCC::CommonMenu::Configure();
    }

    Pop();
  }
}

}  // namespace CommonMenu
}  // namespace Profile
}  // namespace Impacto