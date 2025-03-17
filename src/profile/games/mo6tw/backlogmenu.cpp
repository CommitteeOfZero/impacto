#include "backlogmenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"
#include "../../../games/mo6tw/backlogmenu.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace BacklogMenu {

void Configure() {
  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  UI::BacklogMenuPtr = new UI::MO6TW::BacklogMenu();
  UI::Menus[drawType].push_back(UI::BacklogMenuPtr);
}

}  // namespace BacklogMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto