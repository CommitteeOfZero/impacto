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
  bool success;
  auto const& _mainMenu = TryGetMember(Json, "MainMenu", success);
  if (!success) return;

  AssertIs(_mainMenu, "/MainMenu", kObjectType);

  Type = MainMenuType::_from_integral_unchecked(
      EnsureGetMemberInt(_mainMenu, "/MainMenu", "Type"));

  if (Type == +MainMenuType::RNE) {
    BackgroundInDuration =
        EnsureGetMemberFloat(_mainMenu, "/MainMenu", "BackgroundInDuration");
    BackgroundOutDuration =
        EnsureGetMemberFloat(_mainMenu, "/MainMenu", "BackgroundOutDuration");
    BackgroundSprite =
        EnsureGetMemberSprite(_mainMenu, "/MainMenu", "BackgroundSprite");
  }
}

}  // namespace MainMenu
}  // namespace Profile
}  // namespace Impacto