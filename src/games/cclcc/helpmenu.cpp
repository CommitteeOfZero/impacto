#include "helpmenu.h"

#include "../../profile/games/cclcc/helpmenu.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../profile/dialogue.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../vm/interface/input.h"
#include "../../profile/game.h"
#include "../../inputsystem.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

using namespace Impacto::Profile::CCLCC::HelpMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;

HelpMenu::HelpMenu() {
  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
  NextPageAnimation.DurationIn = NextPageInDuration;
  NextPageAnimation.DurationOut = NextPageOutDuration;
}

void HelpMenu::Show() {
  if (State != Showing) {
    State = Showing;

    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    UI::FocusedMenu = this;

    FadeAnimation.StartIn();
    NextPageAnimation.StartIn();
  }
}

void HelpMenu::Hide() {
  if (State != Hiding) {
    State = Hiding;
    FadeAnimation.StartOut();
    NextPageAnimation.StartOut();
    Audio::Channels[Audio::AC_SSE]->Play("sysse", 3, false, 0);
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
    } else {
      UI::FocusedMenu = 0;
    }
  }
}

void HelpMenu::Update(float dt) {
  if (ScrWork[SW_SYSSUBMENUCT] < 32 && State == Shown) {
    Hide();
  } else if (ScrWork[SW_SYSSUBMENUCT] > 0 && State == Hidden &&
             (ScrWork[SW_SYSSUBMENUNO] == 11)) {
    Show();
  }

  if (State != Hidden) {
    FadeAnimation.Update(dt);
    NextPageAnimation.Update(dt);
  }

  if (State == Shown && ScrWork[SW_SYSSUBMENUNO] == 11) {
    UpdateInput(dt);
  }

  if (State == Showing && FadeAnimation.Progress == 1.0f &&
      ScrWork[SW_SYSSUBMENUCT] == 32) {
    State = Shown;
    IsFocused = true;
  } else if (State == Hiding && FadeAnimation.Progress == 0.0f &&
             ScrWork[SW_SYSSUBMENUCT] == 0) {
    State = Hidden;
    IsFocused = false;
    if (UI::FocusedMenu) UI::FocusedMenu->IsFocused = true;
  }
}

void HelpMenu::UpdateInput(float dt) {
  bool backBtnPressed =
      (PADinputButtonWentDown | PADinputMouseWentDown) & PAD1B;
  if (State == Shown && backBtnPressed) {
    PreviousPage = -1;
    SetFlag(SF_SUBMENUEXIT, 1);
  }

  bool prevBtnPressed =
      ((PADinputButtonWentDown | PADinputMouseWentDown) & PADcustom[38]) ||
      Input::MouseWheelDeltaY < 0;
  if (State == Shown && prevBtnPressed && FadeAnimation.Progress == 1.0f) {
    PreviousPage = CurrentPage;
    CurrentPage =
        (int)((CurrentPage - 1 + ManualPages.size()) % ManualPages.size());
    FadeAnimation.StartIn(true);
    NextPageAnimation.StartIn(true);
    IsGoingNext = false;
  }

  bool nextBtnPressed =
      ((PADinputButtonWentDown | PADinputMouseWentDown) & PADcustom[39]) ||
      Input::MouseWheelDeltaY > 0;
  if (State == Shown && nextBtnPressed && FadeAnimation.Progress == 1.0f) {
    PreviousPage = CurrentPage;
    CurrentPage = (CurrentPage + 1) % ManualPages.size();
    FadeAnimation.StartIn(true);
    NextPageAnimation.StartIn(true);
    IsGoingNext = true;
  }
}

void HelpMenu::Render() {
  if (State == Hidden) return;

  glm::vec4 transition(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
  int alpha = (ScrWork[SW_SYSSUBMENUCT] * ScrWork[SW_SYSSUBMENUALPHA]) >> 5;
  float topLeftX = 0.0f;

  if (PreviousPage != -1) {
    Renderer->DrawSprite(ManualPages[PreviousPage], glm::vec2(0.0f, 0.0f),
                         glm::vec4{1.0f});

    transition = {1.0f, 1.0f, 1.0f, NextPageAnimation.Progress};
    topLeftX = IsGoingNext ? NextPageAnimation.Progress * 1920.0f - 1920.0f
                           : (1.0f - NextPageAnimation.Progress) * 1920.0f;
    Renderer->DrawSprite(
        ManualPages[CurrentPage], glm::vec2(topLeftX, 0.0f),
        glm::vec4{glm::vec3{transition}, transition.a * alpha / 256.0f});
  } else {
    topLeftX = FadeAnimation.Progress * 32 * 200 * 0.0625f - 400.0f;
    Renderer->DrawSprite(
        ManualPages[CurrentPage], glm::vec2(topLeftX, 0.0f),
        glm::vec4{glm::vec3{transition}, transition.a * alpha / 256.0f});
  }

  Renderer->DrawSprite(
      HelpMaskSprite,
      RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
      glm::vec4{glm::vec3{transition}, 0.85f});
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto