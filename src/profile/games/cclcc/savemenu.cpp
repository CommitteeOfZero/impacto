#include "savemenu.h"
#include "../../profile_internal.h"
#include "../../../log.h"

#include "../../ui/savemenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/cclcc/savemenu.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace SaveMenu {

constexpr char const* SaveMenuTypeNames[] = {
    "QuickLoad",
    "Load",
    "Save",
};

void Configure() {
  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::SaveMenuPtr = new UI::CCLCC::SaveMenu();
  UI::Menus[drawType].push_back(UI::SaveMenuPtr);

  EntryStartXL = EnsureGetMemberFloat("EntryStartXL");
  EntryStartXR = EnsureGetMemberFloat("EntryStartXR");
  EntryStartYL = EnsureGetMemberFloat("EntryStartYL");
  EntryStartYR = EnsureGetMemberFloat("EntryStartYR");
  EntryYPadding = EnsureGetMemberFloat("EntryYPadding");

  SaveMenuMaskSprite = EnsureGetMemberSprite("SaveMenuMaskSprite");
  SaveEntryPrimaryColor = EnsureGetMemberUint("SaveEntryPrimaryColor");
  SaveEntrySecondaryColor = EnsureGetMemberUint("SaveEntrySecondaryColor");
  for (int i = 0; i < sizeof(SaveMenuTypeNames) / sizeof(*SaveMenuTypeNames);
       i++) {
    SaveMenuPageType menuType = SaveMenuPageType::_from_integral_unchecked(i);
    std::string menuName = menuType._to_string();

    MenuTextSprite[menuType] =
        EnsureGetMemberSprite((menuName + "TextSprite").c_str());
    EntryHighlightedBoxSprite[menuType] =
        EnsureGetMemberSprite((menuName + "EntryHighlightedBoxSprite").c_str());
    EntryHighlightedTextSprite[menuType] = EnsureGetMemberSprite(
        (menuName + "EntryHighlightedTextSprite").c_str());
    EntrySlotsSprite[menuType] =
        EnsureGetMemberSprite((menuName + "EntrySlotsSprite").c_str());
    ButtonGuideSprite[menuType] =
        EnsureGetMemberSprite((menuName + "ButtonGuideSprite").c_str());
    SeparationLineSprite[menuType] =
        EnsureGetMemberSprite((menuName + "SeparationLineSprite").c_str());
    for (int j = 0; j < 10; j++) {
      NumberDigitSprite[menuType][j] = EnsureGetMemberSprite(
          (menuName + "NumberDigitSprite" + std::to_string(j)).c_str());
    }
    NoDataSprite[menuType] =
        EnsureGetMemberSprite((menuName + "NoDataEntrySprite").c_str());
    BrokenDataSprite[menuType] =
        EnsureGetMemberSprite((menuName + "BrokenDataEntrySprite").c_str());
    SlotLockedSprite[menuType] =
        EnsureGetMemberSprite((menuName + "SlotLockedSprite").c_str());
    for (int j = 0; j < 6; j++) {
      PageNumSprite[menuType][j] = EnsureGetMemberSprite(
          (menuName + "PageNumSprite" + std::to_string(j)).c_str());
    }
    SaveTimeSprite[menuType] =
        EnsureGetMemberSprite((menuName + "SaveTimeSprite").c_str());
  }
}
}  // namespace SaveMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto