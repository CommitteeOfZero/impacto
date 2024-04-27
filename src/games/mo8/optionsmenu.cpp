#include "optionsmenu.h"

#include "../../profile/ui/optionsmenu.h"
#include "../../profile/games/mo8/optionsmenu.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../vm/interface/input.h"

namespace Impacto {
namespace UI {
namespace MO8 {

using namespace Impacto::Profile::OptionsMenu;
using namespace Impacto::Profile::MO8::OptionsMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;

OptionsMenu::OptionsMenu() {
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  PageControls = new Group(this);
  PageControls->FocusLock = false;
  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);
  auto nextPage = new Button(0, NextButtonSprite, NextButtonHighlightedSprite,
                             nullSprite, NextButtonPosition);
  auto previousPage =
      new Button(0, BackButtonSprite, BackButtonHighlightedSprite, nullSprite,
                 BackButtonPosition);
  PageControls->Add(nextPage);
  PageControls->Add(previousPage);
  PageControls->IsShown = true;
}

void OptionsMenu::Show() {
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
void OptionsMenu::Hide() {
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

void OptionsMenu::Update(float dt) {
  UpdateInput();

  FadeAnimation.Update(dt);
  if (ScrWork[SW_SYSSUBMENUCT] < 16 && ScrWork[SW_SYSSUBMENUNO] == 5 &&
      State == Shown) {
    Hide();
  } else if (ScrWork[SW_SYSSUBMENUCT] > 0 && ScrWork[SW_SYSSUBMENUNO] == 5 &&
             State == Hidden) {
    Show();
  }

  if (ScrWork[SW_SYSSUBMENUCT] == 16 && ScrWork[SW_SYSSUBMENUNO] == 5 &&
      FadeAnimation.IsIn())
    State = Shown;
  else if (ScrWork[SW_SYSSUBMENUCT] == 0 && ScrWork[SW_SYSSUBMENUNO] == 5 &&
           FadeAnimation.IsOut())
    State = Hidden;

  if (State == Shown) {
    PageControls->Update(dt);
    if (GetControlState(CT_Back)) {
      SetFlag(SF_SUBMENUEXIT, true);
    }
  }
}

void OptionsMenu::Render() {
  if (State != Hidden) {
    glm::vec4 col(1.0f, 1.0f, 1.0f,
                  glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress));
    Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f, 0.0f), col);
    PageControls->Tint = col;
    PageControls->Render();
  }
}

}  // namespace MO8
}  // namespace UI
}  // namespace Impacto