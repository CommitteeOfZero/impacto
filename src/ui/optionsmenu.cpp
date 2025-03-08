#include "optionsmenu.h"

#include "../profile/game.h"
#include "../profile/ui/optionsmenu.h"
#include "../profile/scriptinput.h"
#include "../vm/interface/input.h"

namespace Impacto {
namespace UI {

using namespace Impacto::Profile::OptionsMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::UI::Widgets;
using namespace Impacto::Vm::Interface;

OptionsMenu::OptionsMenu()
    : DirectionButtonHeldHandler(MinButtonHoldTime, ButtonHoldFireInterval,
                                 PAD1UP | PAD1DOWN | PAD1LEFT | PAD1RIGHT),
      PageButtonHeldHandler(MinButtonHoldTime, ButtonHoldFireInterval,
                            PAD1L1 | PAD1R1) {
  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
}

void OptionsMenu::Show() {
  if (State != Shown) {
    if (State != Showing) {
      FadeAnimation.StartIn();

      CurrentPage = 0;
      Pages[0]->HasFocus = true;
      Pages[0]->Show();
      Highlight(Pages[0]->GetFirstFocusableChild());

      DirectionButtonHeldHandler.Reset();
      PageButtonHeldHandler.Reset();
    }
    State = Showing;

    if (UI::FocusedMenu != nullptr) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}

void OptionsMenu::Hide() {
  if (State != Hidden) {
    if (State != Hiding) FadeAnimation.StartOut();
    State = Hiding;

    Pages[CurrentPage]->Hide();

    if (LastFocusedMenu != nullptr) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = nullptr;
    }
    IsFocused = false;

    if (CurrentlyFocusedElement) {
      CurrentlyFocusedElement->HasFocus = false;
      CurrentlyFocusedElement = nullptr;
    }
  }
}

void OptionsMenu::Update(float dt) {
  FadeAnimation.Update(dt);
  UpdateVisibility();

  if (State != Hidden && IsFocused) {
    UpdateInput(dt);
    Pages[CurrentPage]->Update(dt);
  }
}

void OptionsMenu::UpdatePageInput(float dt) {
  PageButtonHeldHandler.Update(dt);
  const int shouldFire = PageButtonHeldHandler.ShouldFire();

  // Button input
  const int direction =
      (bool)(shouldFire & PAD1R1) - (bool)(shouldFire & PAD1L1);

  if (direction == 0 && shouldFire) {
    PageButtonHeldHandler.Reset();
    return;
  }

  GoToPage((CurrentPage + direction) % Pages.size());
}

void OptionsMenu::UpdateEntryMovementInput(float dt) {
  DirectionButtonHeldHandler.Update(dt);
  const int shouldFire = DirectionButtonHeldHandler.ShouldFire();

  const int verticalMovement =
      (bool)(shouldFire & PAD1DOWN) - (bool)(shouldFire & PAD1UP);
  const FocusDirection verticalDirection = verticalMovement == -1
                                               ? FocusDirection::FDIR_UP
                                               : FocusDirection::FDIR_DOWN;
  const int horizontalMovement =
      (bool)(shouldFire & PAD1RIGHT) - (bool)(shouldFire & PAD1LEFT);
  const FocusDirection horizontalDirection = horizontalMovement == -1
                                                 ? FocusDirection::FDIR_LEFT
                                                 : FocusDirection::FDIR_RIGHT;

  if (!verticalMovement && !horizontalMovement) {
    if (shouldFire) DirectionButtonHeldHandler.Reset();
    return;
  }

  if (horizontalMovement != 0) AdvanceFocus(horizontalDirection);
  if (verticalMovement != 0) AdvanceFocus(verticalDirection);

  Highlight(CurrentlyFocusedElement);
}

void OptionsMenu::UpdateInput(float dt) {
  UpdatePageInput(dt);

  if (GetControlState(CT_Back)) {
    Hide();
    return;
  }

  UpdateEntryMovementInput(dt);
}

void OptionsMenu::GoToPage(int pageNumber) {
  if (CurrentPage == pageNumber) return;

  Pages[CurrentPage]->Hide();

  CurrentPage = pageNumber;
  std::unique_ptr<Group>& page = Pages[CurrentPage];

  page->HasFocus = true;
  page->Show();
  Highlight(page->GetFirstFocusableChild());
}

void OptionsMenu::Highlight(Widget* toHighlight) {
  if (CurrentlyFocusedElement == toHighlight) return;

  for (Widget* entry : Pages[CurrentPage]->Children) {
    entry->HasFocus = false;
  }

  if (toHighlight) {
    toHighlight->HasFocus = true;

    DirectionButtonHeldHandler.PADinputButtonHoldMask =
        (PAD1UP * (bool)toHighlight->GetFocus(FDIR_UP)) |
        (PAD1DOWN * (bool)toHighlight->GetFocus(FDIR_DOWN)) |
        (PAD1LEFT * (bool)toHighlight->GetFocus(FDIR_LEFT)) |
        (PAD1RIGHT * (bool)toHighlight->GetFocus(FDIR_RIGHT));
  }
  CurrentlyFocusedElement = toHighlight;
}

}  // namespace UI
}  // namespace Impacto