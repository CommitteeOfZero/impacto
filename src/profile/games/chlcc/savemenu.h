#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace SaveMenu {

int constexpr Pages = 8;
int constexpr EntriesPerPage = 6;

inline float EntryStartX;
inline float EntryXPadding;
inline float EntryStartY;
inline float EntryYPadding;

inline uint32_t BackgroundColor;
inline uint32_t SaveBackgroundColor;
inline uint32_t LoadBackgroundColor;
inline uint32_t QuickLoadBackgroundColor;
inline Sprite CircleSprite;
inline Sprite SaveCircle;
inline Sprite LoadCircle;
inline Sprite QuickLoadCircle;
inline Sprite MenuTitleTextSprite;
inline Sprite QuickLoadTextSprite;
inline Sprite LoadTextSprite;
inline Sprite SaveTextSprite;
inline glm::vec2 MenuTitleTextLeftPos;
inline glm::vec2 MenuTitleTextRightPos;
inline float MenuTitleTextAngle;

inline glm::vec2 SaveListPosition;
inline Sprite SaveListSprite;
inline glm::vec2 EntryPositions[EntriesPerPage];
inline Sprite QuickLoadEntrySprite;
inline Sprite SaveEntrySprite;
inline Sprite LoadEntrySprite;
inline Sprite SelectionMarkerSprite;
inline glm::vec2 SelectionMarkerOffset;
inline Sprite EntryHighlightedSprite;
inline Sprite LockedSymbolSprite;
inline glm::vec2 LockedSymbolRelativePos;
inline glm::vec2 ThumbnailRelativePos;
inline glm::vec2 PageNumBackgroundPos;
inline Sprite PageNumBackgroundSprite;
inline glm::vec2 CurrentPageNumPos;
inline Sprite BigDigits[10];
inline glm::vec2 PageNumSeparatorSlashPos;
inline Sprite PageNumSeparatorSlashSprite;
inline glm::vec2 MaxPageNumPos;
inline Sprite MaxPageNumSprite;
inline glm::vec2 ButtonPromptPosition;
inline Sprite ButtonPromptSprite;
inline std::vector<glm::vec2> SelectDataTextPositions;
inline std::vector<Sprite> SelectDataTextSprites;

inline glm::vec2 EntryNumberHintTextRelativePos;
inline glm::vec2 EntryNumberTextRelativePos;
inline glm::vec2 SceneTitleTextRelativePos;
inline glm::vec2 NoDataTextRelativePos;
inline glm::vec2 PlayTimeHintTextRelativePos;
inline glm::vec2 PlayTimeTextRelativePos;
inline glm::vec2 SaveDateHintTextRelativePos;
inline glm::vec2 SaveDateTextRelativePos;
inline glm::vec2 SaveHourTextRelativePos;
inline float MaxTitleWidth;

void Configure();

}  // namespace SaveMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto