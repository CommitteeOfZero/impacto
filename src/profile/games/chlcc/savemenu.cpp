#include "savemenu.h"
#include "../../profile_internal.h"
#include "../../../window.h"
#include "../../../log.h"

#include "../../ui/savemenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/chlcc/savemenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace SaveMenu {

void Configure() {
  UI::SaveMenuPtr = new UI::CHLCC::SaveMenu();
}

}  // namespace SaveMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto