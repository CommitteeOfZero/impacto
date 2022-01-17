#include "menu.h"
#include "ui.h"
#include "../inputsystem.h"
#include "../vm/interface/input.h"

namespace Impacto {
namespace UI {

using namespace Impacto::Vm::Interface;

void Menu::Show() {
  if (State != Shown) {
    State = Showing;
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}
void Menu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void Menu::UpdateInput() {
  if (IsFocused) {
    if (PADinputButtonWentDown & PAD1DOWN) {
      AdvanceFocus(FDIR_DOWN);
    } else if (PADinputButtonWentDown & PAD1UP) {
      AdvanceFocus(FDIR_UP);
    } else if (PADinputButtonWentDown & PAD1RIGHT) {
      AdvanceFocus(FDIR_RIGHT);
    } else if (PADinputButtonWentDown & PAD1LEFT) {
      AdvanceFocus(FDIR_LEFT);
    }
  }
}

void Menu::AdvanceFocus(FocusDirection dir) {
  if (!CurrentlyFocusedElement) {
    if (!FocusStart[dir]) return;

    CurrentlyFocusedElement = FocusStart[dir];
    if (CurrentlyFocusedElement->GetType() == WT_GROUP)
      CurrentlyFocusedElement = CurrentlyFocusedElement->GetFocus(dir);

    if (CurrentlyFocusedElement) CurrentlyFocusedElement->HasFocus = true;
    return;
  }

  auto el = CurrentlyFocusedElement->GetFocus(dir);
  if (el && el->GetType() == WT_GROUP) el = el->GetFocus(dir);

  if (el) {
    CurrentlyFocusedElement->HasFocus = false;
    CurrentlyFocusedElement = el;
    CurrentlyFocusedElement->HasFocus = true;
  }
}

}  // namespace UI
}  // namespace Impacto