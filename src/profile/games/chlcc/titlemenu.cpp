#include "titlemenu.h"
#include "../../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace TitleMenu {

Sprite BackgroundSprite;
Sprite DelusionADVUnderSprite;
Sprite DelusionADVSprite;
Sprite SeiraUnderSprite;
Sprite SeiraSprite;
Sprite CHLogoSprite;
Sprite LCCLogoUnderSprite;
Sprite ChuLeftLogoSprite;
Sprite ChuRightLogoSprite;
Sprite LoveLogoSprite;
Sprite StarLogoSprite;
Sprite ExclMarkLogoSprite;
Sprite CopyrightTextSprite;
Sprite SpinningCircleSprite;

float DelusionADVUnderX;
float DelusionADVUnderY;
float DelusionADVX;
float DelusionADVY;
float SeiraUnderX;
float SeiraUnderY;
float SeiraX;
float SeiraY;
float CHLogoX;
float CHLogoY;
float LCCLogoUnderX;
float LCCLogoUnderY;
float ChuLeftLogoX;
float ChuLeftLogoY;
float ChuRightLogoX;
float ChuRightLogoY;
float LoveLogoX;
float LoveLogoY;
float StarLogoX;
float StarLogoY;
float ExclMarkLogoX;
float ExclMarkLogoY;
float CopyrightTextX;
float CopyrightTextY;
float SpinningCircleX;
float SpinningCircleY;

Impacto::TitleMenu::TitleMenuBase* Configure() {
  Impacto::CHLCC::TitleMenu* result = new Impacto::CHLCC::TitleMenu();

  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");
  DelusionADVUnderSprite = EnsureGetMemberSprite("DelusionADVUnderSprite");
  DelusionADVUnderX = EnsureGetMemberFloat("DelusionADVUnderX");
  DelusionADVUnderY = EnsureGetMemberFloat("DelusionADVUnderY");
  DelusionADVSprite = EnsureGetMemberSprite("DelusionADVSprite");
  DelusionADVX = EnsureGetMemberFloat("DelusionADVX");
  DelusionADVY = EnsureGetMemberFloat("DelusionADVY");
  SeiraUnderSprite = EnsureGetMemberSprite("SeiraUnderSprite");
  SeiraUnderX = EnsureGetMemberFloat("SeiraUnderX");
  SeiraUnderY = EnsureGetMemberFloat("SeiraUnderY");
  SeiraSprite = EnsureGetMemberSprite("SeiraSprite");
  SeiraX = EnsureGetMemberFloat("SeiraX");
  SeiraY = EnsureGetMemberFloat("SeiraY");
  CHLogoSprite = EnsureGetMemberSprite("CHLogoSprite");
  CHLogoX = EnsureGetMemberFloat("CHLogoX");
  CHLogoY = EnsureGetMemberFloat("CHLogoY");
  LCCLogoUnderSprite = EnsureGetMemberSprite("LCCLogoUnderSprite");
  LCCLogoUnderX = EnsureGetMemberFloat("LCCLogoUnderX");
  LCCLogoUnderY = EnsureGetMemberFloat("LCCLogoUnderY");
  ChuLeftLogoSprite = EnsureGetMemberSprite("ChuLeftLogoSprite");
  ChuLeftLogoX = EnsureGetMemberFloat("ChuLeftLogoX");
  ChuLeftLogoY = EnsureGetMemberFloat("ChuLeftLogoY");
  ChuRightLogoSprite = EnsureGetMemberSprite("ChuRightLogoSprite");
  ChuRightLogoX = EnsureGetMemberFloat("ChuRightLogoX");
  ChuRightLogoY = EnsureGetMemberFloat("ChuRightLogoY");
  LoveLogoSprite = EnsureGetMemberSprite("LoveLogoSprite");
  LoveLogoX = EnsureGetMemberFloat("LoveLogoX");
  LoveLogoY = EnsureGetMemberFloat("LoveLogoY");
  StarLogoSprite = EnsureGetMemberSprite("StarLogoSprite");
  StarLogoX = EnsureGetMemberFloat("StarLogoX");
  StarLogoY = EnsureGetMemberFloat("StarLogoY");
  ExclMarkLogoSprite = EnsureGetMemberSprite("ExclMarkLogoSprite");
  ExclMarkLogoX = EnsureGetMemberFloat("ExclMarkLogoX");
  ExclMarkLogoY = EnsureGetMemberFloat("ExclMarkLogoY");
  CopyrightTextSprite = EnsureGetMemberSprite("CopyrightTextSprite");
  CopyrightTextX = EnsureGetMemberFloat("CopyrightTextX");
  CopyrightTextY = EnsureGetMemberFloat("CopyrightTextY");
  SpinningCircleSprite = EnsureGetMemberSprite("SpinningCircleSprite");
  SpinningCircleX = EnsureGetMemberFloat("SpinningCircleX");
  SpinningCircleY = EnsureGetMemberFloat("SpinningCircleY");

  return result;
}

}  // namespace TitleMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto