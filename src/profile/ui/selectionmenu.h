#pragma once

#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace SelectionMenu {

inline Sprite SelectionBackground;
inline Sprite PlainSelectionFrameTopLeft;
inline Sprite PlainSelectionFrameTopSide;
inline Sprite PlainSelectionFrameTopRight;
inline Sprite PlainSelectionFrameLeftSide;
inline Sprite PlainSelectionFrameBottomLeft;
inline Sprite PlainSelectionFrameRightSide;
inline Sprite PlainSelectionFrameBottomRight;
inline Sprite PlainSelectionFrameBottomSide;
inline Sprite PlainSelectionFrameMiddle;
inline Sprite SelectionHighlight;
inline Sprite SelectionFocused;

inline int SelectionMaxCount;
inline float SelectionBackgroundX;
inline float SelectionBackgroundY[15];
inline float SelectionBackgroundY1;
inline float SelectionBackgroundY2;
inline float SelectionBackgroundY3;
inline float SelectionBackgroundY4;
inline float SelectionBackgroundY5;
inline float SelectionYSpacing;
inline float PlainSelectionYSpacing;
inline float FadeAnimationDurationInOut;

inline bool HighlightTextOnly = true;

void Configure();

}  // namespace SelectionMenu
}  // namespace Profile
}  // namespace Impacto