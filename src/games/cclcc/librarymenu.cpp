#include "librarymenu.h"

#include "clearlistmenu.h"

#include "../../profile/games/cclcc/librarymenu.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../profile/dialogue.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../vm/interface/input.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

using namespace Impacto::Profile::CCLCC::LibraryMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;

LibraryMenu::LibraryMenu() {
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
}

void LibraryMenu::Show() {
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}

void LibraryMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    FadeAnimation.StartOut();
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void LibraryMenu::Update(float dt) {
  if (ScrWork[SW_SYSSUBMENUCT] < 32 && State == Shown &&
      (ScrWork[SW_SYSSUBMENUNO] == 8)) {
    Hide();
  } else if (ScrWork[SW_SYSSUBMENUCT] >= 32 && State == Hidden &&
             (ScrWork[SW_SYSSUBMENUNO] == 8)) {
    Show();
  }
  if (State == Shown && ScrWork[SW_SYSSUBMENUNO] == 8) {
    UpdateInput();
  }
  FadeAnimation.Update(dt);
  if (State == Showing && FadeAnimation.Progress == 1.0f) {
    State = Shown;
  } else if (State == Hiding && FadeAnimation.Progress == 0.0f) {
    State = Hidden;
  }
}

void LibraryMenu::Render() {
  if (State != Hidden && ScrWork[SW_SYSSUBMENUCT] >= 32 &&
      ScrWork[SW_SYSSUBMENUNO] == 8) {
    glm::vec4 maskTint = glm::vec4(1.0f);
    maskTint.a = 0.85f;
    Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f));
  }
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto