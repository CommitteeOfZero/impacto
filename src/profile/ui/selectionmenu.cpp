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

  SelectionBackground = EnsureGetMember<Sprite>("SelectionBackgroundSprite");
  PlainSelectionFrameTopLeft =
      EnsureGetMember<Sprite>("PlainSelectionFrameTopLeftSprite");
  PlainSelectionFrameTopSide =
      EnsureGetMember<Sprite>("PlainSelectionFrameTopSideSprite");
  PlainSelectionFrameTopRight =
      EnsureGetMember<Sprite>("PlainSelectionFrameTopRightSprite");
  PlainSelectionFrameLeftSide =
      EnsureGetMember<Sprite>("PlainSelectionFrameLeftSideSprite");
  PlainSelectionFrameBottomLeft =
      EnsureGetMember<Sprite>("PlainSelectionFrameBottomLeftSprite");
  PlainSelectionFrameRightSide =
      EnsureGetMember<Sprite>("PlainSelectionFrameRightSideSprite");
  PlainSelectionFrameBottomRight =
      EnsureGetMember<Sprite>("PlainSelectionFrameBottomRightSprite");
  PlainSelectionFrameBottomSide =
      EnsureGetMember<Sprite>("PlainSelectionFrameBottomSideSprite");
  PlainSelectionFrameMiddle =
      EnsureGetMember<Sprite>("PlainSelectionFrameMiddleSprite");

  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);
  SelectionHighlight = nullSprite;
  TryGetMember<Sprite>("SelectionHighlightSprite", SelectionHighlight);
  SelectionFocused = nullSprite;
  TryGetMember<Sprite>("SelectionFocusedSprite", SelectionFocused);

  TryGetMember<bool>("HighlightTextOnly", HighlightTextOnly);

  SelectionMaxCount = EnsureGetMember<int>("SelectionMaxCount");
  SelectionBackgroundX = EnsureGetMember<float>("SelectionBackgroundX");
  GetMemberArray<float>(SelectionBackgroundY, SelectionMaxCount,
                        "SelectionBackgroundY");
  SelectionYSpacing = EnsureGetMember<float>("SelectionYSpacing");
  PlainSelectionYSpacing = EnsureGetMember<float>("PlainSelectionYSpacing");
  FadeAnimationDurationInOut =
      EnsureGetMember<float>("FadeAnimationDurationInOut");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  UI::SelectionMenuPtr = new UI::SelectionMenu();
  UI::Menus[drawType].push_back(UI::SelectionMenuPtr);

  Pop();
}

}  // namespace SelectionMenu
}  // namespace Profile
}  // namespace Impacto