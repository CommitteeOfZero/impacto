#include "tipsmenu.h"
#include "../../profile_internal.h"
#include "../../../window.h"
#include "../../../log.h"

#include "../../ui/tipsmenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/mo6tw/tipsmenu.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace TipsMenu {

void Configure() { UI::TipsMenuPtr = new UI::MO6TW::TipsMenu(); }

}  // namespace TipsMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto
