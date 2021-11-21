#include "group.h"

#include "../../profile/game.h"
#include "../../inputsystem.h"

namespace Impacto {
namespace UI {
namespace Widgets {

Group::Group(Menu* ctx) {
  MenuContext = ctx;
  Position = glm::vec2(0.0f);
  RenderingBounds =
      RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight);
}

Group::Group(Menu* ctx, glm::vec2 pos) : Group(ctx) {
  Position = pos;
  RenderingBounds =
      RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight);
}

void Group::Add(Widget* widget) { Children.push_back(widget); }

void Group::Add(Widget* widget, FocusDirection dir) {
  FocusDirection oppositeDir;
  switch (dir) {
    case FDIR_LEFT:
      oppositeDir = FDIR_RIGHT;
      break;
    case FDIR_RIGHT:
      oppositeDir = FDIR_LEFT;
      break;
    case FDIR_UP:
      oppositeDir = FDIR_DOWN;
      break;
    case FDIR_DOWN:
      oppositeDir = FDIR_UP;
      break;
  }

  if (!Children.empty()) {
    auto el = Children.back();
    el->SetFocus(widget, dir);
    widget->SetFocus(el, oppositeDir);
    if (!FocusStart[dir]) FocusStart[dir] = el;
    if (!FocusStart[oppositeDir]) FocusStart[oppositeDir] = el;
  }

  Add(widget);
  if (WrapFocus) {
    auto firstEl = Children.front();
    widget->SetFocus(firstEl, dir);
    firstEl->SetFocus(widget, oppositeDir);
    FocusStart[oppositeDir] = widget;
  }
}

WidgetType Group::GetType() { return WT_GROUP; }

void Group::UpdateInput() {
  for (const auto& el : Children) {
    if (el->GetType() == WT_NORMAL) {
      el->UpdateInput();
      if (el->Enabled && el->Hovered &&
          Input::CurrentInputDevice == Input::IDEV_Mouse) {
        if (MenuContext->CurrentlyFocusedElement)
          MenuContext->CurrentlyFocusedElement->HasFocus = false;
        el->HasFocus = true;
        MenuContext->CurrentlyFocusedElement = el;
      }
    }
  }
}

void Group::Update(float dt) {
  if (IsShown) {
    if ((FocusLock && HasFocus) || !FocusLock) {
      UpdateInput();
    }
    bool isFocused = false;
    for (const auto& el : Children) {
      if (!FocusLock) {
        isFocused = isFocused ||
                    (el == MenuContext->CurrentlyFocusedElement ? true : false);
        HasFocus = isFocused;
      }
      el->Update(dt);
    }
  }
}

void Group::Render() {
  if (IsShown) {
    for (const auto& el : Children) {
      if (RenderingBounds.Contains(el->Bounds)) {
        auto tint = el->Tint;
        el->Tint *= Tint;
        el->Render();
        el->Tint = tint;
      }
    }
  }
}

Widget* Group::GetFocus(FocusDirection dir) {
  if (!Children.empty()) {
    if (dir == FDIR_DOWN || dir == FDIR_RIGHT)
      return Children.front();
    else if (dir == FDIR_UP || dir == FDIR_LEFT)
      return Children.back();
  } else
    return 0;
}

void Group::Show() {
  Tint.a = 1.0f;
  IsShown = true;
  if (FocusLock) {
    HasFocus = true;
    if (!Children.empty()) {
      PreviousFocusElement = MenuContext->CurrentlyFocusedElement;
      MenuContext->CurrentlyFocusedElement = 0;
      memcpy(PreviousFocusStart, MenuContext->FocusStart,
             sizeof(MenuContext->FocusStart));
      memcpy(MenuContext->FocusStart, FocusStart,
             sizeof(MenuContext->FocusStart));
    }
  }
  for (const auto& el : Children) {
    el->Show();
  }
}

void Group::Hide() {
  Tint.a = 0.0f;
  IsShown = false;
  HasFocus = false;
  if (FocusLock) {
    if (MenuContext->CurrentlyFocusedElement)
      MenuContext->CurrentlyFocusedElement->HasFocus = false;
    MenuContext->CurrentlyFocusedElement = PreviousFocusElement;
    memcpy(MenuContext->FocusStart, PreviousFocusStart,
           sizeof(MenuContext->FocusStart));
  }
  for (const auto& el : Children) {
    el->Hide();
  }
}

void Group::Move(glm::vec2 relativePosition, float duration) {
  for (const auto& el : Children) {
    el->Move(relativePosition, duration);
  }
  Position += relativePosition;
}

void Group::Move(glm::vec2 relativePosition) {
  for (const auto& el : Children) {
    el->Move(relativePosition);
  }
  Position += relativePosition;
}

void Group::MoveTo(glm::vec2 pos, float duration) {
  auto relativePosition = pos - Position;
  for (const auto& el : Children) {
    el->Move(relativePosition, duration);
  }
  Position = pos;
}

void Group::MoveTo(glm::vec2 pos) {
  auto relativePosition = pos - Position;
  for (const auto& el : Children) {
    el->Move(relativePosition);
  }
  Position = pos;
}

void Group::Clear() {
  for (auto el : Children) {
    delete el;
  }
  Children.clear();
}

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto