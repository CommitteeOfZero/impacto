#include "tipsmenu.h"

#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../vm/vm.h"
#include "../../profile/scriptvars.h"
#include "../../profile/dialogue.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../profile/ui/tipsmenu.h"
#include "../../profile/games/cclcc/tipsmenu.h"
#include "../../io/memorystream.h"
#include "../../data/tipssystem.h"
#include "../../vm/interface/input.h"
#include "../../profile/game.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

using namespace Impacto::Profile::TipsMenu;
using namespace Impacto::Profile::CCLCC::TipsMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;

TipsMenu::TipsMenu() {
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
}

void TipsMenu::Show() {
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
void TipsMenu::Hide() {
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
    CurrentlyDisplayedTipId = -1;
  }
}

void TipsMenu::UpdateInput() {
  Menu::UpdateInput();
  if (State == Shown) {
    // ItemsList->UpdateInput();
    if (CurrentlyDisplayedTipId != -1) {
      if (PADinputButtonWentDown & PAD1X) {
        NextTipPage();
      }
    }
  }
}

void TipsMenu::Update(float dt) {
  UpdateInput();
  FadeAnimation.Update(dt);
  if (ScrWork[2142] != 32 && State == Shown && ScrWork[SW_SYSSUBMENUNO] == 2) {
    Hide();
  } else if (ScrWork[2142] == 32 && State == Hidden &&
             ScrWork[SW_SYSSUBMENUNO] == 2) {
    Show();
  }
}

void TipsMenu::Render() {
  if (State != Hidden && ScrWork[2142] == 32 && ScrWork[SW_SYSSUBMENUNO] == 2) {
    glm::vec4 transition(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    glm::vec4 maskTint = glm::vec4(1.0f);
    maskTint.a = 0.85f;
    Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f));
    Renderer->DrawSprite(TipsBookLayerSprite, glm::vec2(0.0f), transition);
    Renderer->DrawSprite(
        TipsMaskSprite,
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight), maskTint);
    Renderer->DrawSprite(TipsGuideSprite, glm::vec2(TipsGuideX, TipsGuideY),
                           transition);
  }
}

void TipsMenu::Init() {}

void TipsMenu::SwitchToTipId(int id) {}

void TipsMenu::NextTipPage() {}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto