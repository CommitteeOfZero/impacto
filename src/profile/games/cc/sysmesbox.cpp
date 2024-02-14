#include "sysmesbox.h"
#include "../../profile_internal.h"

#include "../../../ui/ui.h"
#include "../../../games/cc/sysmesbox.h"
#include "../../../game.h"

namespace Impacto {
namespace Profile {
namespace CC {
namespace SysMesBox {

Sprite SumoSealSprites[SealSpriteCount];

Sprite ButtonYes;
Sprite ButtonNo;
Sprite ButtonOK;
Sprite ButtonYesHighlighted;
Sprite ButtonNoHighlighted;
Sprite ButtonOKHighlighted;

float SumoSealCenterPosX[SealSpriteCount];
float SumoSealCenterPosY[SealSpriteCount];
float ButtonYesCenterPosX;
float ButtonYesCenterPosY;
float ButtonNoCenterPosX;
float ButtonNoCenterPosY;
float ButtonOKCenterPosX;
float ButtonOKCenterPosY;
float AnimationSpeed;
float AnimationProgressWidgetsStartOffset;
float ButtonNoDisplayStart;
float ButtonNoAnimationProgressOffset;
float ButtonYesNoScaleMultiplier;
float ButtonOKScaleMultiplier;
float ButtonScaleMax;
float ButtonYesAnimationProgressEnd;
float ButtonYesNoAlphaDivider;
float WidgetsAlphaMultiplier;

void Configure() {
  GetMemberSpriteArray(SumoSealSprites, SealSpriteCount, "SumoSealSprites");

  GetMemberFloatArray(SumoSealCenterPosX, SealSpriteCount,
                      "SumoSealCenterPosX");
  GetMemberFloatArray(SumoSealCenterPosY, SealSpriteCount,
                      "SumoSealCenterPosY");
  ButtonYes = EnsureGetMemberSprite("ButtonYes");
  ButtonNo = EnsureGetMemberSprite("ButtonNo");
  ButtonOK = EnsureGetMemberSprite("ButtonOK");
  ButtonYesHighlighted = EnsureGetMemberSprite("ButtonYesHighlighted");
  ButtonNoHighlighted = EnsureGetMemberSprite("ButtonNoHighlighted");
  ButtonOKHighlighted = EnsureGetMemberSprite("ButtonOKHighlighted");
  ButtonYesCenterPosX = EnsureGetMemberFloat("ButtonYesCenterPosX");
  ButtonYesCenterPosY = EnsureGetMemberFloat("ButtonYesCenterPosY");
  ButtonNoCenterPosX = EnsureGetMemberFloat("ButtonNoCenterPosX");
  ButtonNoCenterPosY = EnsureGetMemberFloat("ButtonNoCenterPosY");
  ButtonOKCenterPosX = EnsureGetMemberFloat("ButtonOKCenterPosX");
  ButtonOKCenterPosY = EnsureGetMemberFloat("ButtonOKCenterPosY");
  AnimationProgressWidgetsStartOffset =
      EnsureGetMemberFloat("AnimationProgressWidgetsStartOffset");
  ButtonNoDisplayStart = EnsureGetMemberFloat("ButtonNoDisplayStart");
  ButtonNoAnimationProgressOffset =
      EnsureGetMemberFloat("ButtonNoAnimationProgressOffset");
  ButtonYesNoScaleMultiplier =
      EnsureGetMemberFloat("ButtonYesNoScaleMultiplier");
  ButtonOKScaleMultiplier = EnsureGetMemberFloat("ButtonOKScaleMultiplier");
  ButtonScaleMax = EnsureGetMemberFloat("ButtonScaleMax");
  ButtonYesAnimationProgressEnd =
      EnsureGetMemberFloat("ButtonYesAnimationProgressEnd");
  ButtonYesNoAlphaDivider = EnsureGetMemberFloat("ButtonYesNoAlphaDivider");
  WidgetsAlphaMultiplier = EnsureGetMemberFloat("WidgetsAlphaMultiplier");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::SysMesBoxPtr = new UI::CC::SysMesBox();
  UI::Menus[drawType].push_back(UI::SysMesBoxPtr);
}

}  // namespace SysMesBox
}  // namespace CC
}  // namespace Profile
}  // namespace Impacto