#include "backlogmenu.h"

#include "../../profile/game.h"
#include "../../ui/backlogmenu.h"
#include "../../profile/ui/systemmenu.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../profile/games/cc/backlogmenu.h"
#include "../../profile/games/cclcc/systemmenu.h"

namespace Impacto {
namespace UI {
namespace CC {

using namespace Impacto::UI::CC;
using namespace Impacto::Profile::BacklogMenu;
using namespace Impacto::Profile::CC::BacklogMenu;

BacklogMenu::BacklogMenu() : UI::BacklogMenu::BacklogMenu() {
  switch (Impacto::Profile::SystemMenu::Type) {
    default:
      MaskSprite = nullptr;
      break;
    case UI::SystemMenuType::CCLCC:
      MaskSprite = &Profile::CCLCC::SystemMenu::MenuMask;
      break;
      /* Enable once CC system menu is properly seperated
    case UI::SystemMenuType::CC:
      MaskSprite = &Profile::CC::SystemMenu::MenuMask;
      break;
      */
  }
}

void BacklogMenu::Render() {
  if (State == Hidden) return;

  float opacity = glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress);
  glm::vec4 transition(1.0f, 1.0f, 1.0f, opacity);

  glm::vec4 maskTint = transition;
  maskTint.a *= (float)0xa0 / 0x100;

  MainItems->Tint = transition;
  MainScrollbar->Tint = transition;

  int repeatHeight = BacklogBackgroundRepeatHeight;
  float backgroundY =
      fmod(PageY - EntryYPadding - RenderingBounds.Y, repeatHeight);
  Renderer->DrawSprite(BacklogBackground, glm::vec2(0.0f, backgroundY),
                       transition);
  Renderer->DrawSprite(BacklogBackground,
                       glm::vec2(0.0f, backgroundY + repeatHeight), transition);

  RenderHighlight();
  Renderer->DrawSprite(BacklogHeaderSprite, BacklogHeaderPosition, transition);
  MainItems->Render();
  MainScrollbar->Render();

  if (MaskSprite != nullptr)
    Renderer->DrawSprite(
        *MaskSprite,
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
        maskTint);

  Renderer->DrawSprite(BacklogControlsSprite, BacklogControlsPosition,
                       transition);
}

}  // namespace CC
}  // namespace UI
}  // namespace Impacto