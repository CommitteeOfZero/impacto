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

static void GetMemberFloatArray(float* arr, uint32_t count, char const* name) {
  EnsurePushMemberOfType(name, kArrayType);

  if (TopVal().Size() != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d floats for %s\n", count,
           name);
    Renderer->Window->Shutdown();
  }

  for (uint32_t i = 0; i < count; i++) {
    arr[i] = EnsureGetArrayElementFloat(i);
  }

  Pop();
}

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