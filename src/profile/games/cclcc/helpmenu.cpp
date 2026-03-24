#include "helpmenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"
#include "../../../games/cclcc/helpmenu.h"
#include "../../../game.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace HelpMenu {

void Configure() {
  FadeInDuration = EnsureGetMember<float>("FadeInDuration");
  FadeOutDuration = EnsureGetMember<float>("FadeOutDuration");
  NextPageInDuration = EnsureGetMember<float>("NextPageInDuration");
  NextPageOutDuration = EnsureGetMember<float>("NextPageOutDuration");

  ManualPages = EnsureGetMember<std::vector<Sprite>>("ManualPages");
  HelpMaskSprite = EnsureGetMember<Sprite>("HelpMaskSprite");

  auto drawType = EnsureGetMember<Game::DrawComponentType>("DrawType");

  UI::HelpMenuPtr = new UI::CCLCC::HelpMenu();
  UI::Menus[drawType].push_back(UI::HelpMenuPtr);
}

}  // namespace HelpMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto