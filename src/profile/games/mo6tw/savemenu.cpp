#include "savemenu.h"
#include "../../profile_internal.h"
#include "../../../log.h"

#include "../../ui/savemenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/mo6tw/savemenu.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace SaveMenu {

void Configure() {
  SaveMenuBackgroundSprite = EnsureGetMemberSprite("SaveMenuBackgroundSprite");

  EntryStartX = EnsureGetMemberFloat("EntryStartX");
  EntryXPadding = EnsureGetMemberFloat("EntryXPadding");
  EntryStartY = EnsureGetMemberFloat("EntryStartY");
  EntryYPadding = EnsureGetMemberFloat("EntryYPadding");
  QuickLoadTextSprite = EnsureGetMemberSprite("QuickLoadTextSprite");
  LoadTextSprite = EnsureGetMemberSprite("LoadTextSprite");
  SaveTextSprite = EnsureGetMemberSprite("SaveTextSprite");
  MenuTitleTextPos = EnsureGetMemberVec2("MenuTitleTextPos");

  QuickLoadEntrySprite = EnsureGetMemberSprite("QuickLoadEntrySprite");
  QuickLoadEntryHighlightedSprite =
      EnsureGetMemberSprite("QuickLoadEntryHighlightedSprite");
  SaveEntrySprite = EnsureGetMemberSprite("SaveEntrySprite");
  SaveEntryHighlightedSprite =
      EnsureGetMemberSprite("SaveEntryHighlightedSprite");
  LoadEntrySprite = EnsureGetMemberSprite("LoadEntrySprite");
  LoadEntryHighlightedSprite =
      EnsureGetMemberSprite("LoadEntryHighlightedSprite");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::SaveMenuPtr = new UI::MO6TW::SaveMenu();
  UI::Menus[drawType].push_back(UI::SaveMenuPtr);
}

}  // namespace SaveMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto
