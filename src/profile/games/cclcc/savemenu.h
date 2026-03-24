#pragma once

#include "../../../spritesheet.h"
#include "../../../ui/savemenu.h"
#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_containers.hpp>

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace SaveMenu {

int constexpr Pages = 6;

inline float EntryStartXL;
inline float EntryStartXR;
inline float EntryStartYL;
inline float EntryStartYR;
inline float EntryYPadding;
inline float PageSwapDuration;
inline glm::vec2 GuidePosition;
inline glm::vec2 MenuTextPosition;
inline glm::vec2 SlotsBackgroundPosition;
inline glm::vec2 PageNumberPosition;
inline glm::vec2 SlotLockedSpritePosition;
inline glm::vec2 NoDataSpritePosition;

int constexpr EntriesPerRow = 2;
int constexpr RowsPerPage = 4;
inline Sprite SaveMenuMaskSprite;
inline uint32_t SaveEntryPrimaryColor;
inline uint32_t LoadEntryPrimaryColor;
inline uint32_t SaveEntrySecondaryColor;

inline magic_enum::containers::array<UI::SaveMenuPageType, Sprite>
    MenuTextSprite;
inline magic_enum::containers::array<UI::SaveMenuPageType, Sprite>
    EntryHighlightedBoxSprite;
inline magic_enum::containers::array<UI::SaveMenuPageType, Sprite>
    EntryHighlightedTextSprite;
inline magic_enum::containers::array<UI::SaveMenuPageType, Sprite>
    EntrySlotsSprite;
inline magic_enum::containers::array<UI::SaveMenuPageType, Sprite>
    ButtonGuideSprite;
inline magic_enum::containers::array<UI::SaveMenuPageType, Sprite>
    SeparationLineSprite;
inline magic_enum::containers::array<UI::SaveMenuPageType,
                                     std::array<Sprite, 10>>
    NumberDigitSprite;
inline magic_enum::containers::array<UI::SaveMenuPageType, Sprite> NoDataSprite;
inline magic_enum::containers::array<UI::SaveMenuPageType, Sprite>
    BrokenDataSprite;
inline magic_enum::containers::array<UI::SaveMenuPageType, Sprite>
    SlotLockedSprite;
inline magic_enum::containers::array<UI::SaveMenuPageType,
                                     std::array<Sprite, 6>>
    PageNumSprite;
inline magic_enum::containers::array<UI::SaveMenuPageType, Sprite>
    SaveTimeSprite;

void Configure();

}  // namespace SaveMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto