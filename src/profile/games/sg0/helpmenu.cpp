#include "helpmenu.h"
#include "../../profile_internal.h"

#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/sg0/helpmenu.h"

namespace Impacto {
namespace Profile {
namespace SG0 {
namespace HelpMenu {

void Configure() {
  PageSprites = EnsureGetMember<std::vector<Sprite>>("PageSprites");
  CountMax = EnsureGetMember<int>("CountMax");

  UI::HelpMenuPtr = new UI::SG0::HelpMenu();
  auto drawType = EnsureGetMember<Game::DrawComponentType>("DrawType");
  UI::Menus[drawType].push_back(UI::HelpMenuPtr);
}

}  // namespace HelpMenu
}  // namespace SG0
}  // namespace Profile
}  // namespace Impacto
