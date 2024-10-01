#include "backlogmenu.h"

#include "../../profile/game.h"
#include "../../ui/backlogmenu.h"
#include "../../ui/widgets/cc/backlogentry.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../profile/games/cc/backlogmenu.h"

namespace Impacto {
namespace UI {
namespace CC {

using namespace Impacto::UI::CC;
using namespace Impacto::Profile::BacklogMenu;
using namespace Impacto::Profile::CC::BacklogMenu;
using namespace Impacto::UI::Widgets::CC;

void BacklogMenu::MenuButtonOnClick(Widgets::BacklogEntry* target) {
  if (target->AudioId != -1) {
    Audio::Channels[Audio::AC_REV]->Play("voice", target->AudioId, false, 0.0f);
  } else {
    Audio::Channels[Audio::AC_REV]->Play("sysse", 4, false, 0.0f);
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

  Renderer->DrawSprite(
      MenuMaskSprite,
      RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight), maskTint);

  Renderer->DrawSprite(BacklogControlsSprite, BacklogControlsPosition,
                       transition);
}

BacklogEntry* BacklogMenu::CreateBacklogEntry(int id, uint8_t* str, int audioId,
                                              glm::vec2 pos,
                                              const RectF& hoverBounds) const {
  return new BacklogEntry(id, str, audioId, pos, HoverBounds);
}

}  // namespace CC
}  // namespace UI
}  // namespace Impacto