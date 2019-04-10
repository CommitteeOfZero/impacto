#include "titlemenu.h"
#include "../../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace Dash {
namespace TitleMenu {

Sprite BackgroundSprite;

Impacto::TitleMenu::TitleMenuBase* Configure() {
  Impacto::Dash::TitleMenu* result = new Impacto::Dash::TitleMenu();

  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");

  return result;
}

}  // namespace TitleMenu
}  // namespace Dash
}  // namespace Profile
}  // namespace Impacto