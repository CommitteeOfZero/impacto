#include "selectionmenu.h"
#include "../profile_internal.h"
#include "../../ui/ui.h"
//#include "../../window.h"
#include "../../renderer/renderer.h"
#include "../../game.h"

namespace Impacto {
namespace Profile {
namespace SelectionMenu {

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

int SelectionMaxCount;
float SelectionBackgroundX;
float SelectionBackgroundY[15];
float SelectionBackgroundY1;
float SelectionBackgroundY2;
float SelectionBackgroundY3;
float SelectionBackgroundY4;
float SelectionBackgroundY5;
float SelectionYSpacing;
float PlainSelectionYSpacing;
float FadeAnimationDurationInOut;

void Configure() {
  EnsurePushMemberOfType("SelectionDisplay", LUA_TTABLE);

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

  SelectionMaxCount = EnsureGetMemberInt("SelectionMaxCount");
  SelectionBackgroundX = EnsureGetMemberFloat("SelectionBackgroundX");
  GetMemberFloatArray(SelectionBackgroundY, SelectionMaxCount,
                      "SelectionBackgroundY");
  SelectionYSpacing = EnsureGetMemberFloat("SelectionYSpacing");
  PlainSelectionYSpacing = EnsureGetMemberFloat("PlainSelectionYSpacing");
  FadeAnimationDurationInOut =
      EnsureGetMemberFloat("FadeAnimationDurationInOut");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::SelectionMenuPtr = new UI::SelectionMenu();
  UI::Menus[drawType].push_back(UI::SelectionMenuPtr);

  Pop();
}

}  // namespace SelectionMenu
}  // namespace Profile
}  // namespace Impacto