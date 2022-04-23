#include "clearlistmenu.h"

#include "../../profile/ui/extramenus.h"
#include "../../profile/games/cclcc/clearlistmenu.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../profile/dialogue.h"
#include "../../renderer2d.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../vm/vm.h"
#include "../../vm/interface/input.h"
#include "../../data/savesystem.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

using namespace Impacto::Profile::CCLCC::ClearListMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;

ClearListMenu::ClearListMenu() {
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
}

void ClearListMenu::Show() {
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

void ClearListMenu::Hide() {
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

void ClearListMenu::Update(float dt) {
  // UpdateInput();
  FadeAnimation.Update(dt);
  if (ScrWork[2142] != 32 && State == Shown && ScrWork[2148] == 7) {
    Hide();
  } else if (ScrWork[2142] == 32 && State == Hidden && ScrWork[2148] == 7) {
    Show();
  }
}

void ClearListMenu::Render() {
  if (State != Hidden && ScrWork[2142] == 32 && ScrWork[2148] == 7) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    Renderer2D::DrawSprite(BackgroundSprite, glm::vec2(0.0f), col);
  }
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto