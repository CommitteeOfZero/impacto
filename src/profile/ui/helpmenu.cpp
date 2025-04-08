#include "helpmenu.h"
#include "../profile_internal.h"
#include "../games/cclcc/helpmenu.h"
#include "../../ui/ui.h"
#include "../../log.h"

namespace Impacto {
namespace Profile {
namespace HelpMenu {

using namespace Impacto::UI;

void Configure() {
  if (TryPushMember("HelpMenu")) {
    AssertIs(LUA_TTABLE);

    Type = HelpMenuType::_from_integral_unchecked(EnsureGetMember<int>("Type"));

    if (Type == +HelpMenuType::CCLCC) {
      CCLCC::HelpMenu::Configure();
    }

    Pop();
  }
}

}  // namespace HelpMenu
}  // namespace Profile
}  // namespace Impacto