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
  SaveMenuBackgroundSprite =
      EnsureGetMember<Sprite>("SaveMenuBackgroundSprite");

  EntryStartX = EnsureGetMember<float>("EntryStartX");
  EntryXPadding = EnsureGetMember<float>("EntryXPadding");
  EntryStartY = EnsureGetMember<float>("EntryStartY");
  EntryYPadding = EnsureGetMember<float>("EntryYPadding");
  QuickLoadTextSprite = EnsureGetMember<Sprite>("QuickLoadTextSprite");
  LoadTextSprite = EnsureGetMember<Sprite>("LoadTextSprite");
  SaveTextSprite = EnsureGetMember<Sprite>("SaveTextSprite");
  MenuTitleTextPos = EnsureGetMember<glm::vec2>("MenuTitleTextPos");

  QuickLoadEntrySprite = EnsureGetMember<Sprite>("QuickLoadEntrySprite");
  QuickLoadEntryHighlightedSprite =
      EnsureGetMember<Sprite>("QuickLoadEntryHighlightedSprite");
  SaveEntrySprite = EnsureGetMember<Sprite>("SaveEntrySprite");
  SaveEntryHighlightedSprite =
      EnsureGetMember<Sprite>("SaveEntryHighlightedSprite");
  LoadEntrySprite = EnsureGetMember<Sprite>("LoadEntrySprite");
  LoadEntryHighlightedSprite =
      EnsureGetMember<Sprite>("LoadEntryHighlightedSprite");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  UI::SaveMenuPtr = new UI::MO6TW::SaveMenu();
  UI::Menus[drawType].push_back(UI::SaveMenuPtr);
}

}  // namespace SaveMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto
