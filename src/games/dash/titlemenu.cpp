#include "titlemenu.h"

#include "../../profile/ui/titlemenu.h"
#include "../../profile/games/dash/titlemenu.h"
#include "../../renderer/renderer.h"
#include "../../texture/texture.h"
#include "../../spritesheet.h"
#include "../../audio/audiosystem.h"
#include "../../audio/audiostream.h"
#include "../../audio/audiochannel.h"
#include "../../io/vfs.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../background2d.h"

namespace Impacto {
namespace UI {
namespace Dash {

using namespace Impacto::Profile::TitleMenu;
using namespace Impacto::Profile::Dash::TitleMenu;
using namespace Impacto::Profile::ScriptVars;

TitleMenu::TitleMenu() {}

void TitleMenu::Show() {
  if (State == Hidden) {
    State = Shown;
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
    if (PressToStartAnimation.State == AS_Stopped) {
      PressToStartAnimation.StartIn();
    }
  }
}
void TitleMenu::Hide() {
  if (State == Shown) {
    State = Hidden;
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void TitleMenu::Update(float dt) {
  UpdateInput();

  PressToStartAnimation.Update(dt);
  if (GetFlag(SF_TITLEMODE) && ScrWork[SW_TITLEDISPCT] == 2) {
    Show();
  } else {
    Hide();
  }
}
void TitleMenu::Render() {
  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
    glm::vec4 col = glm::vec4(1.0f);
    col.a = glm::smoothstep(0.0f, 1.0f, PressToStartAnimation.Progress);
    Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f));
    Renderer->DrawSprite(PressToStartSprite,
                         glm::vec2(PressToStartX, PressToStartY), col);
  }
}

}  // namespace Dash
}  // namespace UI
}  // namespace Impacto