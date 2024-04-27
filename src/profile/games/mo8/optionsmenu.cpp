#include "optionsmenu.h"
#include "../../profile_internal.h"
#include "../../../log.h"

#include "../../ui/optionsmenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/mo8/optionsmenu.h"

namespace Impacto {
namespace Profile {
namespace MO8 {
namespace OptionsMenu {

Sprite NextButtonSprite;
Sprite NextButtonHighlightedSprite;
glm::vec2 NextButtonPosition;
Sprite BackButtonSprite;
Sprite BackButtonHighlightedSprite;
glm::vec2 BackButtonPosition;

void Configure() {
  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  NextButtonSprite = EnsureGetMemberSprite("NextButtonSprite");
  NextButtonHighlightedSprite =
      EnsureGetMemberSprite("NextButtonHighlightedSprite");
  NextButtonPosition = EnsureGetMemberVec2("NextButtonPosition");

  BackButtonSprite = EnsureGetMemberSprite("BackButtonSprite");
  BackButtonHighlightedSprite =
      EnsureGetMemberSprite("BackButtonHighlightedSprite");
  BackButtonPosition = EnsureGetMemberVec2("BackButtonPosition");

  UI::OptionsMenuPtr = new UI::MO8::OptionsMenu();
  UI::Menus[drawType].push_back(UI::OptionsMenuPtr);
}

}  // namespace OptionsMenu
}  // namespace MO8
}  // namespace Profile
}  // namespace Impacto
