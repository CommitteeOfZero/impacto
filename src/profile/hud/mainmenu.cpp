#include "mainmenu.h"
#include "../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace MainMenu {

using namespace Impacto::MainMenu;

MainMenuType Type = MainMenuType::None;

float BackgroundInDuration;
float BackgroundOutDuration;
Sprite BackgroundSprite;

void Configure() {
  if (TryPushMember("MainMenu")) {
    AssertIs(kObjectType);

    Type = MainMenuType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

    if (Type == +MainMenuType::RNE) {
      BackgroundInDuration = EnsureGetMemberFloat("BackgroundInDuration");
      BackgroundOutDuration = EnsureGetMemberFloat("BackgroundOutDuration");
      BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");
    }

    Pop();
  }
}

}  // namespace MainMenu
}  // namespace Profile
}  // namespace Impacto