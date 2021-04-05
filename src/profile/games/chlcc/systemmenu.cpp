#include "systemmenu.h"
#include "../../profile_internal.h"
#include "../../../window.h"
#include "../../../log.h"

#include "../../ui/systemmenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/chlcc/systemmenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace SystemMenu {


void Configure() {
  UI::SystemMenuPtr = new UI::CHLCC::SystemMenu();
}

}  // namespace SystemMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto