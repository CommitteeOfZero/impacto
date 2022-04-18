#include "savemenu.h"
#include "../../profile_internal.h"
#include "../../../window.h"
#include "../../../log.h"

#include "../../ui/savemenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/cclcc/savemenu.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace SaveMenu {

void Configure() {
  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::SaveMenuPtr = new UI::CCLCC::SaveMenu();
  UI::Menus[drawType].push_back(UI::SaveMenuPtr);
}

}  // namespace SaveMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto