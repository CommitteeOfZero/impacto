#include "backlogmenu.h"
#include "../../profile_internal.h"

#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/sg0/backlogmenu.h"

namespace Impacto {
namespace Profile {
namespace SG0 {
namespace BacklogMenu {

void Configure() {
  PanelPos = EnsureGetMember<glm::vec2>("PanelPos");
  ShadeOpacity = EnsureGetMember<float>("ShadeOpacity");
  HighlightBounds = EnsureGetMember<RectF>("HighlightBounds");
  const uint32_t color = EnsureGetMember<uint32_t>("HighlightColor");
  HighlightColor =
      glm::vec3((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF) /
      255.0f;
  HighlightOpacity = EnsureGetMember<float>("HighlightOpacity");

  auto drawType = EnsureGetMember<Game::DrawComponentType>("DrawType");
  UI::BacklogMenuPtr = new UI::SG0::BacklogMenu();
  UI::Menus[drawType].push_back(UI::BacklogMenuPtr);
}

}  // namespace BacklogMenu
}  // namespace SG0
}  // namespace Profile
}  // namespace Impacto
