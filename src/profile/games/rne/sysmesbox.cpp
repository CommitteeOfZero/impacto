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
  BoxDecorationTop = EnsureGetMember<Sprite>("BoxDecorationTop");
  BoxDecorationBottom = EnsureGetMember<Sprite>("BoxDecorationBottom");
  TextDecoration = EnsureGetMember<Sprite>("TextDecoration");
  MessageLabel = EnsureGetMember<Sprite>("MessageLabel");
  Line1 = EnsureGetMember<Sprite>("Line1");
  Line2 = EnsureGetMember<Sprite>("Line2");
  ButtonYes = EnsureGetMember<Sprite>("ButtonYes");
  ButtonNo = EnsureGetMember<Sprite>("ButtonNo");
  ButtonOK = EnsureGetMember<Sprite>("ButtonOK");
  ButtonYesHighlighted = EnsureGetMember<Sprite>("ButtonYesHighlighted");
  ButtonNoHighlighted = EnsureGetMember<Sprite>("ButtonNoHighlighted");
  ButtonOKHighlighted = EnsureGetMember<Sprite>("ButtonOKHighlighted");

  LinePositionXFirst = EnsureGetMember<float>("LinePositionXFirst");
  LinePositionX = EnsureGetMember<float>("LinePositionX");
  LinePositionMultiplier = EnsureGetMember<float>("LinePositionMultiplier");
  LineWidthFirst = EnsureGetMember<float>("LineWidthFirst");
  LineWidthBase = EnsureGetMember<float>("LineWidthBase");
  LineWidthMultiplier = EnsureGetMember<float>("LineWidthMultiplier");
  Line1SpriteY = EnsureGetMember<float>("Line1SpriteY");
  Line2SpriteY = EnsureGetMember<float>("Line2SpriteY");
  LineSpriteHeight = EnsureGetMember<float>("LineSpriteHeight");
  LineDisplayXBase = EnsureGetMember<float>("LineDisplayXBase");
  Line1DisplayY = EnsureGetMember<float>("Line1DisplayY");
  Line2DisplayY = EnsureGetMember<float>("Line2DisplayY");
  BoxDisplayStartCount = EnsureGetMember<float>("BoxDisplayStartCount");
  BoxHeightBase = EnsureGetMember<float>("BoxHeightBase");
  BoxHeightMultiplier = EnsureGetMember<float>("BoxHeightMultiplier");
  BoxWidth = EnsureGetMember<float>("BoxWidth");
  BoxTextFontSize = EnsureGetMember<float>("BoxTextFontSize");
  BoxTopYBase = EnsureGetMember<float>("BoxTopYBase");
  BoxDisplayX = EnsureGetMember<float>("BoxDisplayX");
  MessageLabelSpriteXBase = EnsureGetMember<float>("MessageLabelSpriteXBase");
  MessageLabelSpriteY = EnsureGetMember<float>("MessageLabelSpriteY");
  MessageLabelSpriteHeight = EnsureGetMember<float>("MessageLabelSpriteHeight");
  MessageLabelSpriteMultiplier =
      EnsureGetMember<float>("MessageLabelSpriteMultiplier");
  ButtonYesDisplayXBase = EnsureGetMember<float>("ButtonYesDisplayXBase");
  ButtonRightDisplayXBase = EnsureGetMember<float>("ButtonRightDisplayXBase");
  ButtonWidth = EnsureGetMember<float>("ButtonWidth");
  ButtonYOffset = EnsureGetMember<float>("ButtonYOffset");
  ButtonYWidthBase = EnsureGetMember<float>("ButtonYWidthBase");
  ButtonRightWidthBase = EnsureGetMember<float>("ButtonRightWidthBase");
  TextDecorationStart = EnsureGetMember<float>("TextDecorationStart");
  TextDecorationTopYOffset = EnsureGetMember<float>("TextDecorationTopYOffset");
  TextDecorationBottomYOffset =
      EnsureGetMember<float>("TextDecorationBottomYOffset");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  UI::SysMesBoxPtr = new UI::RNE::SysMesBox();
  UI::Menus[drawType].push_back(UI::SysMesBoxPtr);
}

}  // namespace SysMesBox
}  // namespace RNE
}  // namespace Profile
}  // namespace Impacto