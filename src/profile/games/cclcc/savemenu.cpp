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
      EnsureGetMember<int>("DrawType"));

  UI::SaveMenuPtr = new UI::CCLCC::SaveMenu();
  UI::Menus[drawType].push_back(UI::SaveMenuPtr);

  EntryStartXL = EnsureGetMember<float>("EntryStartXL");
  EntryStartXR = EnsureGetMember<float>("EntryStartXR");
  EntryStartYL = EnsureGetMember<float>("EntryStartYL");
  EntryStartYR = EnsureGetMember<float>("EntryStartYR");
  EntryYPadding = EnsureGetMember<float>("EntryYPadding");

  SaveMenuMaskSprite = EnsureGetMember<Sprite>("SaveMenuMaskSprite");
  SaveEntryPrimaryColor = EnsureGetMember<uint32_t>("SaveEntryPrimaryColor");
  SaveEntrySecondaryColor =
      EnsureGetMember<uint32_t>("SaveEntrySecondaryColor");
  for (int i = 0; i < sizeof(SaveMenuTypeNames) / sizeof(*SaveMenuTypeNames);
       i++) {
    UI::SaveMenuPageType menuType =
        UI::SaveMenuPageType::_from_integral_unchecked(i);
    std::string menuName = menuType._to_string();

    MenuTextSprite[menuType] =
        EnsureGetMember<Sprite>((menuName + "TextSprite").c_str());
    EntryHighlightedBoxSprite[menuType] = EnsureGetMember<Sprite>(
        (menuName + "EntryHighlightedBoxSprite").c_str());
    EntryHighlightedTextSprite[menuType] = EnsureGetMember<Sprite>(
        (menuName + "EntryHighlightedTextSprite").c_str());
    EntrySlotsSprite[menuType] =
        EnsureGetMember<Sprite>((menuName + "EntrySlotsSprite").c_str());
    ButtonGuideSprite[menuType] =
        EnsureGetMember<Sprite>((menuName + "ButtonGuideSprite").c_str());
    SeparationLineSprite[menuType] =
        EnsureGetMember<Sprite>((menuName + "SeparationLineSprite").c_str());
    for (int j = 0; j < 10; j++) {
      NumberDigitSprite[menuType][j] = EnsureGetMember<Sprite>(
          (menuName + "NumberDigitSprite" + std::to_string(j)).c_str());
    }
    NoDataSprite[menuType] =
        EnsureGetMember<Sprite>((menuName + "NoDataEntrySprite").c_str());
    BrokenDataSprite[menuType] =
        EnsureGetMember<Sprite>((menuName + "BrokenDataEntrySprite").c_str());
    SlotLockedSprite[menuType] =
        EnsureGetMember<Sprite>((menuName + "SlotLockedSprite").c_str());
    for (int j = 0; j < 6; j++) {
      PageNumSprite[menuType][j] = EnsureGetMember<Sprite>(
          (menuName + "PageNumSprite" + std::to_string(j)).c_str());
    }
    SaveTimeSprite[menuType] =
        EnsureGetMember<Sprite>((menuName + "SaveTimeSprite").c_str());
  }
}
}  // namespace SaveMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto