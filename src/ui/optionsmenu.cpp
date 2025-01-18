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

OptionsMenu::OptionsMenu() {
  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
}

void OptionsMenu::Show() {
  if (State != Shown) {
    if (State != Showing) FadeAnimation.StartIn();
    State = Showing;

    GoToPage(0);

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

  if (State != Hidden) {
    UpdateInput(dt);
    Pages[CurrentPage]->Update(dt);
  }
}

void OptionsMenu::UpdatePageInput(float dt) {
  // Button input
  const int direction = (bool)(PADinputButtonIsDown & PAD1R1) -
                        (bool)(PADinputButtonIsDown & PAD1L1);

  if (direction == 0) {
    PageDirectionButtonHeldTime = 0.0f;
    PageDirectionButtonWaitTime = 0.0f;
    return;
  }

  if (0.0f < PageDirectionButtonHeldTime &&
      PageDirectionButtonHeldTime < MinButtonHoldTime) {
    PageDirectionButtonHeldTime += dt;
    PageDirectionButtonWaitTime = 0.0f;
    return;
  }

  if (PageDirectionButtonWaitTime > 0.0f) {
    PageDirectionButtonWaitTime -= dt;
    return;
  }

  // Page advancement fired
  PageDirectionButtonHeldTime += dt;
  PageDirectionButtonWaitTime = ButtonHoldTimeInterval;

  GoToPage((CurrentPage + direction) % Pages.size());
}

void OptionsMenu::UpdateEntryMovementInput(float dt) {
  const int verticalMovement = (bool)(PADinputButtonIsDown & PAD1DOWN) -
                               (bool)(PADinputButtonIsDown & PAD1UP);
  const FocusDirection verticalDirection = verticalMovement == -1
                                               ? FocusDirection::FDIR_UP
                                               : FocusDirection::FDIR_DOWN;
  const int horizontalMovement = (bool)(PADinputButtonIsDown & PAD1RIGHT) -
                                 (bool)(PADinputButtonIsDown & PAD1LEFT);
  const FocusDirection horizontalDirection = horizontalMovement == -1
                                                 ? FocusDirection::FDIR_LEFT
                                                 : FocusDirection::FDIR_RIGHT;

  // Don't count towards holding the movement button if it is not
  // in a valid movement direction
  bool moving = CurrentlyFocusedElement &&
                (verticalMovement &&
                     CurrentlyFocusedElement->GetFocus(verticalDirection) ||
                 horizontalMovement &&
                     CurrentlyFocusedElement->GetFocus(horizontalDirection));
  moving |=
      !CurrentlyFocusedElement && (verticalMovement || horizontalMovement);

  if (!moving) {
    DirectionButtonHeldTime = 0.0f;
    DirectionButtonWaitTime = 0.0f;
    return;
  }

  if (0.0f < DirectionButtonHeldTime &&
      DirectionButtonHeldTime < MinButtonHoldTime) {
    DirectionButtonHeldTime += dt;
    DirectionButtonWaitTime = 0.0f;
    return;
  }

  if (DirectionButtonWaitTime > 0.0f) {
    DirectionButtonWaitTime -= dt;
    return;
  }

  // Advance entry

  DirectionButtonHeldTime += dt;
  DirectionButtonWaitTime = ButtonHoldTimeInterval;

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
  if (CurrentPage == pageNumber && Pages[CurrentPage]->IsShown) return;

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

  if (toHighlight) toHighlight->HasFocus = true;
  CurrentlyFocusedElement = toHighlight;
}

}  // namespace UI
}  // namespace Impacto