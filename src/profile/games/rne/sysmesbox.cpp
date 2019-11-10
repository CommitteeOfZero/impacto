#include "sysmesbox.h"
#include "../../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace RNE {
namespace SysMesBox {

Sprite BoxDecorationTop;
Sprite BoxDecorationBottom;
Sprite TextDecoration;
Sprite MessageLabel;
Sprite Line1;
Sprite Line2;
Sprite ButtonYes;
Sprite ButtonNo;
Sprite ButtonOK;

float LinePositionXFirst;
float LinePositionX;
float LinePositionMultiplier;
float LineWidthFirst;
float LineWidthBase;
float LineWidthMultiplier;
float Line1SpriteY;
float Line2SpriteY;
float LineSpriteHeight;
float LineDisplayXBase;
float Line1DisplayY;
float Line2DisplayY;
float BoxDisplayStartCount;
float BoxHeightBase;
float BoxHeightMultiplier;
float BoxWidth;
float BoxTextFontSize;
float BoxTopYBase;
float BoxDisplayX;
float MessageLabelSpriteXBase;
float MessageLabelSpriteY;
float MessageLabelSpriteHeight;
float MessageLabelSpriteMultiplier;
float ButtonYesDisplayXBase;
float ButtonRightDisplayXBase;
float ButtonSpriteY;
float ButtonWidth;
float ButtonSelectedSpriteY;
float ButtonYOffset;
float ButtonYWidthBase;
float ButtonRightWidthBase;
float TextDecorationStart;
float TextDecorationTopYOffset;
float TextDecorationBottomYOffset;
float TextFontSize;
float TextMiddleY;
float TextX;
float TextLineHeight;
float TextMarginY;
float AnimationSpeed;
float FadeInDuration;
float FadeOutDuration;
float SpriteMargin;

Impacto::SysMesBox::SysMesBoxBase* Configure() {
  Impacto::RNE::SysMesBox* result = new Impacto::RNE::SysMesBox();

  BoxDecorationTop = EnsureGetMemberSprite("BoxDecorationTop");
  BoxDecorationBottom = EnsureGetMemberSprite("BoxDecorationBottom");
  TextDecoration = EnsureGetMemberSprite("TextDecoration");
  MessageLabel = EnsureGetMemberSprite("MessageLabel");
  Line1 = EnsureGetMemberSprite("Line1");
  Line2 = EnsureGetMemberSprite("Line2");
  ButtonYes = EnsureGetMemberSprite("ButtonYes");
  ButtonNo = EnsureGetMemberSprite("ButtonNo");
  ButtonOK = EnsureGetMemberSprite("ButtonOK");

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
  ButtonSpriteY = EnsureGetMemberFloat("ButtonSpriteY");
  ButtonWidth = EnsureGetMemberFloat("ButtonWidth");
  ButtonSelectedSpriteY = EnsureGetMemberFloat("ButtonSelectedSpriteY");
  ButtonYOffset = EnsureGetMemberFloat("ButtonYOffset");
  ButtonYWidthBase = EnsureGetMemberFloat("ButtonYWidthBase");
  ButtonRightWidthBase = EnsureGetMemberFloat("ButtonRightWidthBase");
  TextDecorationStart = EnsureGetMemberFloat("TextDecorationStart");
  TextDecorationTopYOffset = EnsureGetMemberFloat("TextDecorationTopYOffset");
  TextDecorationBottomYOffset =
      EnsureGetMemberFloat("TextDecorationBottomYOffset");
  SpriteMargin = EnsureGetMemberFloat("SpriteMargin");

  return result;
}

}  // namespace SysMesBox
}  // namespace RNE
}  // namespace Profile
}  // namespace Impacto