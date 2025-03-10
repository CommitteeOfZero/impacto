#include "sysmenubutton.h"

#include "../../../renderer/renderer.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

void SysMenuButton::Render() {
  glm::vec4 tint = glm::vec4(1.0f);
  tint.a = Tint.a;

  if (HasFocus) {
    Renderer->DrawSprite(HighlightSprite, RenderPos, Tint);
  } else {
    if (Enabled) {
      Renderer->DrawSprite(NormalSprite, RenderPos, tint);
    } else {
      Renderer->DrawSprite(DisabledSprite, RenderPos, Tint);
    }
  }
}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto