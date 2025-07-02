#include "backlogmenu.h"

#include "../../profile/game.h"
#include "../../ui/backlogmenu.h"
#include "../../ui/widgets/cc/backlogentry.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../profile/games/cc/backlogmenu.h"
#include "../../profile/scriptvars.h"

namespace Impacto {
namespace UI {
namespace CC {

using namespace Impacto::UI::CC;
using namespace Impacto::Profile::BacklogMenu;
using namespace Impacto::Profile::CC::BacklogMenu;
using namespace Impacto::UI::Widgets::CC;
using namespace Impacto::Profile::ScriptVars;

void BacklogMenu::MenuButtonOnClick(Widgets::BacklogEntry* target) {
  UI::BacklogMenu::MenuButtonOnClick(target);

  if (target->AudioId == -1)
    Audio::Channels[Audio::AC_SSE]->Play("sysse", 4, false, 0.0f);
}

void BacklogMenu::Show() {
  if (State == Hidden) {
    if (ScrWork[SW_SYSMENUALPHA] == 0x100) {
      FadeAnimation.DurationIn = FadeInDuration;
      FadeAnimation.DurationOut = FadeOutDuration;
    } else {
      FadeAnimation.DurationIn = FadeInDirectDuration;
      FadeAnimation.DurationOut = FadeOutDirectDuration;
    }
  }

  UI::BacklogMenu::Show();
}

void BacklogMenu::Hide() {
  if (State == Shown) {
    if (ScrWork[SW_SYSMENUALPHA] == 0x100) {
      FadeAnimation.DurationIn = FadeInDuration;
      FadeAnimation.DurationOut = FadeOutDuration;
    } else {
      FadeAnimation.DurationIn = FadeInDirectDuration;
      FadeAnimation.DurationOut = FadeOutDirectDuration;
    }
  }
  Audio::Channels[Audio::AC_REV]->Stop(0.0f);
  UI::BacklogMenu::Hide();
}

void BacklogMenu::UpdateVisibility() {
  if (ScrWork[SW_SYSSUBMENUCT] < 32 && State == Shown) {
    Hide();
  } else if (ScrWork[SW_SYSSUBMENUCT] > 0 && State == Hidden &&
             ScrWork[SW_SYSSUBMENUNO] == 1) {
    Show();
  }

  if (FadeAnimation.IsIn() && ScrWork[SW_SYSSUBMENUCT] == 32) {
    State = Shown;
    IsFocused = true;
  } else if (State == Hiding && FadeAnimation.IsOut() &&
             ScrWork[SW_SYSSUBMENUCT] == 0) {
    State = Hidden;
    IsFocused = false;
    if (UI::FocusedMenu) UI::FocusedMenu->IsFocused = true;

    MainItems->Hide();
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

}  // namespace CC
}  // namespace UI
}  // namespace Impacto