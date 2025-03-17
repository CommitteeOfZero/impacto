#include "sysmesbox.h"
#include "../../profile_internal.h"

#include "../../../ui/ui.h"
#include "../../../games/cc/sysmesbox.h"
#include "../../../game.h"

namespace Impacto {
namespace Profile {
namespace CC {
namespace SysMesBox {

void Configure() {
  GetMemberArray<Sprite>(SumoSealSprites, SealSpriteCount, "SumoSealSprites");

  GetMemberArray<float> (SumoSealCenterPosX, SealSpriteCount,
                         "SumoSealCenterPosX");
  GetMemberArray<float> (SumoSealCenterPosY, SealSpriteCount,
                         "SumoSealCenterPosY");
  ButtonYes = EnsureGetMember<Sprite>("ButtonYes");
  ButtonNo = EnsureGetMember<Sprite>("ButtonNo");
  ButtonOK = EnsureGetMember<Sprite>("ButtonOK");
  ButtonYesHighlighted = EnsureGetMember<Sprite>("ButtonYesHighlighted");
  ButtonNoHighlighted = EnsureGetMember<Sprite>("ButtonNoHighlighted");
  ButtonOKHighlighted = EnsureGetMember<Sprite>("ButtonOKHighlighted");
  ButtonYesCenterPosX = EnsureGetMember<float>("ButtonYesCenterPosX");
  ButtonYesCenterPosY = EnsureGetMember<float>("ButtonYesCenterPosY");
  ButtonNoCenterPosX = EnsureGetMember<float>("ButtonNoCenterPosX");
  ButtonNoCenterPosY = EnsureGetMember<float>("ButtonNoCenterPosY");
  ButtonOKCenterPosX = EnsureGetMember<float>("ButtonOKCenterPosX");
  ButtonOKCenterPosY = EnsureGetMember<float>("ButtonOKCenterPosY");
  AnimationProgressWidgetsStartOffset =
      EnsureGetMember<float>("AnimationProgressWidgetsStartOffset");
  ButtonNoDisplayStart = EnsureGetMember<float>("ButtonNoDisplayStart");
  ButtonNoAnimationProgressOffset =
      EnsureGetMember<float>("ButtonNoAnimationProgressOffset");
  ButtonYesNoScaleMultiplier =
      EnsureGetMember<float>("ButtonYesNoScaleMultiplier");
  ButtonOKScaleMultiplier = EnsureGetMember<float>("ButtonOKScaleMultiplier");
  ButtonScaleMax = EnsureGetMember<float>("ButtonScaleMax");
  ButtonYesAnimationProgressEnd =
      EnsureGetMember<float>("ButtonYesAnimationProgressEnd");
  ButtonYesNoAlphaDivider = EnsureGetMember<float>("ButtonYesNoAlphaDivider");
  WidgetsAlphaMultiplier = EnsureGetMember<float>("WidgetsAlphaMultiplier");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  UI::SysMesBoxPtr = new UI::CC::SysMesBox();
  UI::Menus[drawType].push_back(UI::SysMesBoxPtr);
}

}  // namespace SysMesBox
}  // namespace CC
}  // namespace Profile
}  // namespace Impacto