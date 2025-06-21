#include "backlogmenu.h"

#include "../../ui/backlogmenu.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../profile/games/mo6tw/backlogmenu.h"
#include "../../profile/games/mo6tw/systemmenu.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

using namespace Impacto::Profile::BacklogMenu;
using namespace Impacto::Profile::MO6TW::BacklogMenu;

void BacklogMenu::Render() {
  if (State == Hidden) return;

  float opacity = glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress);
  glm::vec4 col(1.0f, 1.0f, 1.0f, opacity);
  MainItems->Tint = col;
  MainScrollbar->Tint = col;

  Renderer->DrawSprite(BacklogBackground, glm::vec2(0.0f), col);
  RenderHighlight();
  MainItems->Render();
  MainScrollbar->Render();
}

void BacklogMenu::UpdateVisibility() {
  if (FadeAnimation.IsIn()) {
    State = Shown;
    IsFocused = true;
  } else if (FadeAnimation.IsOut()) {
    State = Hidden;
    IsFocused = false;
    if (UI::FocusedMenu) UI::FocusedMenu->IsFocused = true;

    MainItems->Hide();
  }
}

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto
