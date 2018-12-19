#include "titlemenu.h"
#include "../profile_internal.h"
#include "../games/rne/tilebackground.h"
#include "../../log.h"
#include "../../window.h"

namespace Impacto {
namespace Profile {
namespace TitleMenu {

using namespace Impacto::TitleMenu;

TitleMenuType Type = TitleMenuType::None;

Sprite PressToStartSprite;
Sprite LineSprite;
Sprite CopyrightSprite;
Sprite EliteSprite;
Sprite LogoSprite;

float PreTitleAnimDurationIn;
float PreTitleAnimDurationOut;
float PressToStartAnimDurationIn;
float PressToStartAnimDurationOut;

float LineWidth;
float CopyrightWidth;
float EliteHeight;
float LogoWidth;
float PressToStartX;
float PressToStartY;
float LineX;
float LineY;
float CopyrightX;
float CopyrightY;
float EliteX;
float EliteY;
float LogoX;
float LogoY;

void Configure() {
  if (TryPushMember("TitleMenu")) {
    AssertIs(kObjectType);

    Type = TitleMenuType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

    if (Type == +TitleMenuType::RNE) {
      EnsurePushMemberOfType("Background", kObjectType);
      BackgroundAnimation = RNE::ParseTileBackground();
      Pop();
    }
    PressToStartSprite = EnsureGetMemberSprite("PressToStartSprite");
    LineSprite = EnsureGetMemberSprite("LineSprite");
    CopyrightSprite = EnsureGetMemberSprite("CopyrightSprite");
    EliteSprite = EnsureGetMemberSprite("EliteSprite");
    LogoSprite = EnsureGetMemberSprite("LogoSprite");

    PreTitleAnimDurationIn = EnsureGetMemberFloat("PreTitleAnimDurationIn");
    PreTitleAnimDurationOut = EnsureGetMemberFloat("PreTitleAnimDurationOut");
    PressToStartAnimDurationIn =
        EnsureGetMemberFloat("PressToStartAnimDurationIn");
    PressToStartAnimDurationOut =
        EnsureGetMemberFloat("PressToStartAnimDurationOut");

    LineWidth = EnsureGetMemberFloat("LineWidth");
    CopyrightWidth = EnsureGetMemberFloat("CopyrightWidth");
    EliteHeight = EnsureGetMemberFloat("EliteHeight");
    LogoWidth = EnsureGetMemberFloat("LogoWidth");
    PressToStartX = EnsureGetMemberFloat("PressToStartX");
    PressToStartY = EnsureGetMemberFloat("PressToStartY");
    LineX = EnsureGetMemberFloat("LineX");
    LineY = EnsureGetMemberFloat("LineY");
    CopyrightX = EnsureGetMemberFloat("CopyrightX");
    CopyrightY = EnsureGetMemberFloat("CopyrightY");
    EliteX = EnsureGetMemberFloat("EliteX");
    EliteY = EnsureGetMemberFloat("EliteY");
    LogoX = EnsureGetMemberFloat("LogoX");
    LogoY = EnsureGetMemberFloat("LogoY");
    Pop();
  }
}

}  // namespace TitleMenu
}  // namespace Profile
}  // namespace Impacto