#include "librarysubmenus.h"

#include "../../ui/ui.h"
#include "../../profile/games/cclcc/librarymenu.h"
#include "../../video/videosystem.h"
#include "../../vm/interface/input.h"
#include "../../inputsystem.h"
#include "../../text.h"
#include "../../profile/dialogue.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

using namespace Impacto::Profile::CCLCC::LibraryMenu;
using namespace Impacto::Profile::ScriptVars;

LibrarySubmenu::LibrarySubmenu() : Menu(), MainItems(this) {
  FadeAnimation.DurationIn = SubMenuFadeInDuration;
  FadeAnimation.DurationOut = SubMenuFadeOutDuration;
};

void LibrarySubmenu::Show() {
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    MainItems.Show();
  }
  if (UI::FocusedMenu != 0 && UI::FocusedMenu != this) {
    LastFocusedMenu = UI::FocusedMenu;
    LastFocusedMenu->IsFocused = false;
  }
  if (State == Shown) {
    IsFocused = true;
  }
  UI::FocusedMenu = this;
}
void LibrarySubmenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    FadeAnimation.StartOut();
    Unfocus();
  }
}

void LibrarySubmenu::Update(float dt) {
  FadeAnimation.Update(dt);
  MainItems.HasFocus = IsFocused;
  MainItems.Update(dt);
  UpdateInput(dt);
  if (CurrentlyFocusedElement) {
    CurrentlyFocusedElement->Update(dt);
  }
  if (State == Showing && FadeAnimation.IsIn()) {
    State = Shown;
    IsFocused = true;
  } else if (State == Hiding && FadeAnimation.IsOut()) {
    State = Hidden;
    IsFocused = false;
    MainItems.Hide();
    if (UI::FocusedMenu) UI::FocusedMenu->IsFocused = true;
  }
}

void LibrarySubmenu::Render() {
  if (State == Hidden) return;

  MainItems.Tint = glm::vec4(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
  MainItems.Render();
}

void LibrarySubmenu::Unfocus() {
  if (!IsFocused) return;

  if (UI::FocusedMenu == this) {
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
    } else {
      UI::FocusedMenu = 0;
    }
  }
  IsFocused = false;
}
}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto