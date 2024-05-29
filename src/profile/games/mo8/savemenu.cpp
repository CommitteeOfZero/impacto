#include "savemenu.h"
#include "../../profile_internal.h"
#include "../../../log.h"

#include "../../ui/savemenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/mo8/savemenu.h"

namespace Impacto {
namespace Profile {
namespace MO8 {
namespace SaveMenu {

float EntryStartX;
float EntryXPadding;
float EntryStartY;
float EntryYPadding;

Sprite QuickLoadTextSprite;
Sprite LoadTextSprite;
Sprite SaveTextSprite;
glm::vec2 MenuTitleTextPos;

Sprite NextButtonSprite;
Sprite NextButtonHighlightedSprite;
glm::vec2 NextButtonPosition;
Sprite BackButtonSprite;
Sprite BackButtonHighlightedSprite;
glm::vec2 BackButtonPosition;

void Configure() {
  EntryStartX = EnsureGetMemberFloat("EntryStartX");
  EntryXPadding = EnsureGetMemberFloat("EntryXPadding");
  EntryStartY = EnsureGetMemberFloat("EntryStartY");
  EntryYPadding = EnsureGetMemberFloat("EntryYPadding");

  QuickLoadTextSprite = EnsureGetMemberSprite("QuickLoadTextSprite");
  LoadTextSprite = EnsureGetMemberSprite("LoadTextSprite");
  SaveTextSprite = EnsureGetMemberSprite("SaveTextSprite");
  MenuTitleTextPos = EnsureGetMemberVec2("MenuTitleTextPos");

  NextButtonSprite = EnsureGetMemberSprite("NextButtonSprite");
  NextButtonHighlightedSprite =
      EnsureGetMemberSprite("NextButtonHighlightedSprite");
  NextButtonPosition = EnsureGetMemberVec2("NextButtonPosition");

  BackButtonSprite = EnsureGetMemberSprite("BackButtonSprite");
  BackButtonHighlightedSprite =
      EnsureGetMemberSprite("BackButtonHighlightedSprite");
  BackButtonPosition = EnsureGetMemberVec2("BackButtonPosition");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::SaveMenuPtr = new UI::MO8::SaveMenu();
  UI::Menus[drawType].push_back(UI::SaveMenuPtr);
}

}  // namespace SaveMenu
}  // namespace MO8
}  // namespace Profile
}  // namespace Impacto
