#pragma once

#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace SelectionMenu {

extern Sprite SelectionBackground;
extern Sprite PlainSelectionFrameTopLeft;
extern Sprite PlainSelectionFrameTopSide;
extern Sprite PlainSelectionFrameTopRight;
extern Sprite PlainSelectionFrameLeftSide;
extern Sprite PlainSelectionFrameBottomLeft;
extern Sprite PlainSelectionFrameRightSide;
extern Sprite PlainSelectionFrameBottomRight;
extern Sprite PlainSelectionFrameBottomSide;
extern Sprite PlainSelectionFrameMiddle;
extern Sprite SelectionHighlight;

extern int SelectionMaxCount;
extern float SelectionBackgroundX;
extern float SelectionBackgroundY[15];
extern float SelectionBackgroundY1;
extern float SelectionBackgroundY2;
extern float SelectionBackgroundY3;
extern float SelectionBackgroundY4;
extern float SelectionBackgroundY5;
extern float SelectionYSpacing;
extern float PlainSelectionYSpacing;
extern float FadeAnimationDurationInOut;

void Configure();

}  // namespace SelectionMenu
}  // namespace Profile
}  // namespace Impacto