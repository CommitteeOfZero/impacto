#include "sysmesbox.h"
#include "../../profile_internal.h"

#include "../../../ui/ui.h"
#include "../../../games/chlcc/sysmesbox.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace SysMesBox {

Sprite Box;
Sprite BoxDecoration;
Sprite SelectionLeftPart;
Sprite SelectionRightPart;
Sprite SelectionMiddlePart;

float BoxX;
float BoxY;
float ChoicePadding;
float ChoiceY;
float ChoiceXBase;
float MinMaxMesWidth;
float MinHighlightWidth;
float HighlightBaseWidth;
float HighlightRightPartSpriteWidth;
float HighlightYOffset;
float HighlightXOffset;
float HighlightXBase;
float HighlightXStep;

void Configure() {
  Box = EnsureGetMemberSprite("Box");
  BoxDecoration = EnsureGetMemberSprite("BoxDecoration");
  SelectionLeftPart = EnsureGetMemberSprite("SelectionLeftPart");
  SelectionRightPart = EnsureGetMemberSprite("SelectionRightPart");
  SelectionMiddlePart = EnsureGetMemberSprite("SelectionMiddlePart");

  BoxX = EnsureGetMemberFloat("BoxX");
  BoxY = EnsureGetMemberFloat("BoxY");
  ChoicePadding = EnsureGetMemberFloat("ChoicePadding");
  ChoiceY = EnsureGetMemberFloat("ChoiceY");
  ChoiceXBase = EnsureGetMemberFloat("ChoiceXBase");
  MinMaxMesWidth = EnsureGetMemberFloat("MinMaxMesWidth");
  MinHighlightWidth = EnsureGetMemberFloat("MinHighlightWidth");
  HighlightBaseWidth = EnsureGetMemberFloat("HighlightBaseWidth");
  HighlightRightPartSpriteWidth =
      EnsureGetMemberFloat("HighlightRightPartSpriteWidth");
  HighlightYOffset = EnsureGetMemberFloat("HighlightYOffset");
  HighlightXOffset = EnsureGetMemberFloat("HighlightXOffset");
  HighlightXBase = EnsureGetMemberFloat("HighlightXBase");
  HighlightXStep = EnsureGetMemberFloat("HighlightXStep");

  UI::SysMesBoxPtr = new UI::CHLCC::SysMesBox();
}

}  // namespace SysMesBox
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto