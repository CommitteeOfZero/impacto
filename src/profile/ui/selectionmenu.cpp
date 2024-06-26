#include "selectionmenu.h"
#include "../profile_internal.h"
#include "../../ui/ui.h"
// #include "../../window.h"
#include "../../renderer/renderer.h"
#include "../../game.h"

namespace Impacto {
namespace Profile {
namespace SelectionMenu {

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

  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);
  SelectionHighlight = nullSprite;
  TryGetMemberSprite("SelectionHighlightSprite", SelectionHighlight);
  SelectionFocused = nullSprite;
  TryGetMemberSprite("SelectionFocusedSprite", SelectionFocused);

  TryGetMemberBool("HighlightTextOnly", HighlightTextOnly);

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