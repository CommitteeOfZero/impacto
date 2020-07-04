#include "titlemenu.h"
#include "../../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace Dash {
namespace TitleMenu {

Sprite BackgroundSprite;

void Configure() {
  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");
}

}  // namespace TitleMenu
}  // namespace Dash
}  // namespace Profile
}  // namespace Impacto