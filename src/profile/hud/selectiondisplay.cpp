#include "selectiondisplay.h"
#include "../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace SelectionDisplay {

Sprite SelectionBackground;
Sprite PlainSelectionFrameTopLeft;
Sprite PlainSelectionFrameTopSide;
Sprite PlainSelectionFrameTopRight;
Sprite PlainSelectionFrameLeftSide;
Sprite PlainSelectionFrameBottomLeft;
Sprite PlainSelectionFrameRightSide;
Sprite PlainSelectionFrameBottomRight;
Sprite PlainSelectionFrameBottomSide;
Sprite PlainSelectionFrameMiddle;
Sprite SelectionHighlight;

float SelectionBackgroundX;
float SelectionBackgroundY1;
float SelectionBackgroundY2;
float SelectionBackgroundY3;
float SelectionBackgroundY4;
float SelectionBackgroundY5;
float SelectionYSpacing;
float PlainSelectionYSpacing;
float FadeAnimationDurationInOut;

void Configure() {
  EnsurePushMemberOfType("SelectionDisplay", kObjectType);

  SelectionBackground = EnsureGetMemberSprite("SelectionBackgroundSprite");
  PlainSelectionFrameTopLeft =
      EnsureGetMemberSprite("PlainSelectionFrameTopLeftSprite");
  PlainSelectionFrameTopSide =
      EnsureGetMemberSprite("PlainSelectionFrameTopSideSprite");
  PlainSelectionFrameTopRight =
      EnsureGetMemberSprite("PlainSelectionFrameTopRightSprite");
  PlainSelectionFrameLeftSide =
      EnsureGetMemberSprite("PlainSelectionFrameLeftSideSprite");
  PlainSelectionFrameBottomLeft =
      EnsureGetMemberSprite("PlainSelectionFrameBottomLeftSprite");
  PlainSelectionFrameRightSide =
      EnsureGetMemberSprite("PlainSelectionFrameRightSideSprite");
  PlainSelectionFrameBottomRight =
      EnsureGetMemberSprite("PlainSelectionFrameBottomRightSprite");
  PlainSelectionFrameBottomSide =
      EnsureGetMemberSprite("PlainSelectionFrameBottomSideSprite");
  PlainSelectionFrameMiddle =
      EnsureGetMemberSprite("PlainSelectionFrameMiddleSprite");
  SelectionHighlight = EnsureGetMemberSprite("SelectionHighlightSprite");

  SelectionBackgroundX = EnsureGetMemberFloat("SelectionBackgroundX");
  SelectionBackgroundY1 = EnsureGetMemberFloat("SelectionBackgroundY1");
  SelectionBackgroundY2 = EnsureGetMemberFloat("SelectionBackgroundY2");
  SelectionBackgroundY3 = EnsureGetMemberFloat("SelectionBackgroundY3");
  SelectionBackgroundY4 = EnsureGetMemberFloat("SelectionBackgroundY4");
  SelectionBackgroundY5 = EnsureGetMemberFloat("SelectionBackgroundY5");
  SelectionYSpacing = EnsureGetMemberFloat("SelectionYSpacing");
  PlainSelectionYSpacing = EnsureGetMemberFloat("PlainSelectionYSpacing");
  FadeAnimationDurationInOut =
      EnsureGetMemberFloat("FadeAnimationDurationInOut");

  Pop();
}

}  // namespace SelectionDisplay
}  // namespace Profile
}  // namespace Impacto