#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace SG0 {
namespace TipsMenu {

void Configure();

int constexpr TabCount = 4;  // ALL, ACQUIRED, UNREAD, NEW

inline Sprite PanelsSprite;  // tipschips list + note panels, full screen
inline Sprite HeaderSprite;
inline glm::vec2 HeaderPos;
inline Sprite HeaderIconSprite;
inline glm::vec2 HeaderIconPos;
inline Sprite GuideSprite;
inline glm::vec2 GuidePos;

inline Sprite TabSprites[TabCount];
inline Sprite TabDisabledSprites[TabCount];
inline Sprite TabSelectedSprites[TabCount];
inline RectF TabBounds[TabCount];

// Tip list (left panel).
inline RectF ListBounds;
inline float ListTop;
inline float RowHeight;
inline float RowPadding;
inline int VisibleRows;
inline float ListNumberX;
inline float ListNameX;
inline float ListNameMaxWidth;
inline float ListFontSize;
inline Sprite SelectionSprite;
inline Sprite NewBadgeSprite;
inline float NewBadgeX;

// Tip view (right panel). The note text itself is laid out by the scripts'
// text mode 7; NoteBounds clips and scrolls it.
inline glm::vec2 NumberPos;
inline glm::vec2 CategoryPos;
inline glm::vec2 NamePos;
inline float NameMaxWidth;
inline float HeaderFontSize;
inline float NameFontSize;
inline RectF NoteBounds;
inline Sprite NoteThumbSprite;
inline float NoteThumbX;
inline float NoteScrollSpeed;

inline uint32_t TextColor;
inline uint32_t LockedColor;
// SW_SYSSUBMENUCT value at which the screen is fully shown.
inline int CountMax;

}  // namespace TipsMenu
}  // namespace SG0
}  // namespace Profile
}  // namespace Impacto
