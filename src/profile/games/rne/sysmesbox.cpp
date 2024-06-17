#include "sysmesbox.h"
#include "../../profile_internal.h"

#include "../../../ui/ui.h"
#include "../../../games/rne/sysmesbox.h"
#include "../../../game.h"

namespace Impacto {
namespace Profile {
namespace RNE {
namespace SysMesBox {

void Configure() {
  BoxDecorationTop = EnsureGetMemberSprite("BoxDecorationTop");
  BoxDecorationBottom = EnsureGetMemberSprite("BoxDecorationBottom");
  TextDecoration = EnsureGetMemberSprite("TextDecoration");
  MessageLabel = EnsureGetMemberSprite("MessageLabel");
  Line1 = EnsureGetMemberSprite("Line1");
  Line2 = EnsureGetMemberSprite("Line2");
  ButtonYes = EnsureGetMemberSprite("ButtonYes");
  ButtonNo = EnsureGetMemberSprite("ButtonNo");
  ButtonOK = EnsureGetMemberSprite("ButtonOK");
  ButtonYesHighlighted = EnsureGetMemberSprite("ButtonYesHighlighted");
  ButtonNoHighlighted = EnsureGetMemberSprite("ButtonNoHighlighted");
  ButtonOKHighlighted = EnsureGetMemberSprite("ButtonOKHighlighted");

  LinePositionXFirst = EnsureGetMemberFloat("LinePositionXFirst");
  LinePositionX = EnsureGetMemberFloat("LinePositionX");
  LinePositionMultiplier = EnsureGetMemberFloat("LinePositionMultiplier");
  LineWidthFirst = EnsureGetMemberFloat("LineWidthFirst");
  LineWidthBase = EnsureGetMemberFloat("LineWidthBase");
  LineWidthMultiplier = EnsureGetMemberFloat("LineWidthMultiplier");
  Line1SpriteY = EnsureGetMemberFloat("Line1SpriteY");
  Line2SpriteY = EnsureGetMemberFloat("Line2SpriteY");
  LineSpriteHeight = EnsureGetMemberFloat("LineSpriteHeight");
  LineDisplayXBase = EnsureGetMemberFloat("LineDisplayXBase");
  Line1DisplayY = EnsureGetMemberFloat("Line1DisplayY");
  Line2DisplayY = EnsureGetMemberFloat("Line2DisplayY");
  BoxDisplayStartCount = EnsureGetMemberFloat("BoxDisplayStartCount");
  BoxHeightBase = EnsureGetMemberFloat("BoxHeightBase");
  BoxHeightMultiplier = EnsureGetMemberFloat("BoxHeightMultiplier");
  BoxWidth = EnsureGetMemberFloat("BoxWidth");
  BoxTextFontSize = EnsureGetMemberFloat("BoxTextFontSize");
  BoxTopYBase = EnsureGetMemberFloat("BoxTopYBase");
  BoxDisplayX = EnsureGetMemberFloat("BoxDisplayX");
  MessageLabelSpriteXBase = EnsureGetMemberFloat("MessageLabelSpriteXBase");
  MessageLabelSpriteY = EnsureGetMemberFloat("MessageLabelSpriteY");
  MessageLabelSpriteHeight = EnsureGetMemberFloat("MessageLabelSpriteHeight");
  MessageLabelSpriteMultiplier =
      EnsureGetMemberFloat("MessageLabelSpriteMultiplier");
  ButtonYesDisplayXBase = EnsureGetMemberFloat("ButtonYesDisplayXBase");
  ButtonRightDisplayXBase = EnsureGetMemberFloat("ButtonRightDisplayXBase");
  ButtonWidth = EnsureGetMemberFloat("ButtonWidth");
  ButtonYOffset = EnsureGetMemberFloat("ButtonYOffset");
  ButtonYWidthBase = EnsureGetMemberFloat("ButtonYWidthBase");
  ButtonRightWidthBase = EnsureGetMemberFloat("ButtonRightWidthBase");
  TextDecorationStart = EnsureGetMemberFloat("TextDecorationStart");
  TextDecorationTopYOffset = EnsureGetMemberFloat("TextDecorationTopYOffset");
  TextDecorationBottomYOffset =
      EnsureGetMemberFloat("TextDecorationBottomYOffset");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::SysMesBoxPtr = new UI::RNE::SysMesBox();
  UI::Menus[drawType].push_back(UI::SysMesBoxPtr);
}

}  // namespace SysMesBox
}  // namespace RNE
}  // namespace Profile
}  // namespace Impacto