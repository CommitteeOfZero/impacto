#include "clearlistmenu.h"

#include "../../profile/ui/extramenus.h"
#include "../../profile/games/cclcc/clearlistmenu.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../profile/dialogue.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../vm/vm.h"
#include "../../vm/interface/input.h"
#include "../../data/savesystem.h"
#include "../../profile/game.h"

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
  if (ScrWork[2147] < 32 && State == Shown && ScrWork[2148] == 7) {
    Hide();
  } else if (ScrWork[2147] >= 32 && State == Hidden && ScrWork[2148] == 7) {
    Show();
  }
}

void ClearListMenu::Render() {
  if (State != Hidden && ScrWork[2147] >= 32 && ScrWork[2148] == 7) {
    glm::vec4 transition(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    glm::vec4 maskTint = glm::vec4(1.0f);
    maskTint.a = 0.85f;
    Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f));
    Renderer->DrawSprite(ClearListBookLayerSprite, glm::vec2(0.0f), transition);
    Renderer->DrawSprite(
        ClearListMaskSprite,
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight), maskTint);
    Renderer->DrawSprite(ClearListGuideSprite,
                           glm::vec2(ClearListGuideX, ClearListGuideY), transition);
  }
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto