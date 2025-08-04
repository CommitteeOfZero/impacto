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

  ManualPages = GetMemberVector<Sprite>("ManualPages");
  HelpMaskSprite = EnsureGetMember<Sprite>("HelpMaskSprite");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  UI::HelpMenuPtr = new UI::CCLCC::HelpMenu();
  UI::Menus[drawType].push_back(UI::HelpMenuPtr);
}

}  // namespace HelpMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto