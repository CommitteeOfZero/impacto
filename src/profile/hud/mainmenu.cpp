#include "mainmenu.h"
#include "../profile_internal.h"
#include "../games/rne/tilebackground.h"

namespace Impacto {
namespace Profile {
namespace MainMenu {

using namespace Impacto::MainMenu;

MainMenuType Type = MainMenuType::None;

void Configure() {
  if (TryPushMember("MainMenu")) {
    AssertIs(kObjectType);

    Type = MainMenuType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

    if (Type == +MainMenuType::RNE) {
      EnsurePushMemberOfType("Background", kObjectType);
      BackgroundAnimation = RNE::ParseTileBackground();
      Pop();
    }

    Pop();
  }
}

}  // namespace MainMenu
}  // namespace Profile
}  // namespace Impacto