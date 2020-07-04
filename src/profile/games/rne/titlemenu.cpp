#include "titlemenu.h"
#include "../../profile_internal.h"
#include "tilebackground.h"

namespace Impacto {
namespace Profile {
namespace RNE {
namespace TitleMenu {

Sprite LineSprite;
Sprite CopyrightSprite;
Sprite EliteSprite;
Sprite LogoSprite;

float PreTitleAnimDurationIn;
float PreTitleAnimDurationOut;

float LineWidth;
float CopyrightWidth;
float EliteHeight;
float LogoWidth;
float LineX;
float LineY;
float CopyrightX;
float CopyrightY;
float EliteX;
float EliteY;
float LogoX;
float LogoY;

void Configure() {
  EnsurePushMemberOfType("Background", kObjectType);
  // result->BackgroundAnimation = RNE::ParseTileBackground();
  RNE::ParseTileBackground();
  Pop();

  LineSprite = EnsureGetMemberSprite("LineSprite");
  CopyrightSprite = EnsureGetMemberSprite("CopyrightSprite");
  EliteSprite = EnsureGetMemberSprite("EliteSprite");
  LogoSprite = EnsureGetMemberSprite("LogoSprite");

  LineWidth = EnsureGetMemberFloat("LineWidth");
  CopyrightWidth = EnsureGetMemberFloat("CopyrightWidth");
  EliteHeight = EnsureGetMemberFloat("EliteHeight");
  LogoWidth = EnsureGetMemberFloat("LogoWidth");
  LineX = EnsureGetMemberFloat("LineX");
  LineY = EnsureGetMemberFloat("LineY");
  CopyrightX = EnsureGetMemberFloat("CopyrightX");
  CopyrightY = EnsureGetMemberFloat("CopyrightY");
  EliteX = EnsureGetMemberFloat("EliteX");
  EliteY = EnsureGetMemberFloat("EliteY");
  LogoX = EnsureGetMemberFloat("LogoX");
  LogoY = EnsureGetMemberFloat("LogoY");

  PreTitleAnimDurationIn = EnsureGetMemberFloat("PreTitleAnimDurationIn");
  PreTitleAnimDurationOut = EnsureGetMemberFloat("PreTitleAnimDurationOut");

  // result->PreTitleItemsAnimation.DurationIn = PreTitleAnimDurationIn;
  // result->PreTitleItemsAnimation.DurationOut = PreTitleAnimDurationOut;
}

}  // namespace TitleMenu
}  // namespace RNE
}  // namespace Profile
}  // namespace Impacto