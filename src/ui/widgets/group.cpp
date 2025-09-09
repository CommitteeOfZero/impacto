#include "group.h"

#include "../../profile/game.h"
#include "../../inputsystem.h"
#include "../../renderer/renderer.h"

namespace Impacto {
namespace UI {
namespace Widgets {

Group::Group(Menu* ctx) {
  Enabled = true;
  MenuContext = ctx;
  Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);
  RenderingBounds =
      RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight);
  HoverBounds = RenderingBounds;
}

Group::Group(Menu* ctx, glm::vec2 pos) : Group(ctx) {
  Bounds = RectF(pos.x, pos.y, 0.0f, 0.0f);
}

Group::~Group() { Clear(); }

void Group::Add(Widget* widget) { Children.push_back(widget); }

void Group::Add(Widget* widget, FocusDirection dir) {
  const FocusDirection oppositeDir = [dir]() {
    switch (dir) {
      case FDIR_LEFT:
        return FDIR_RIGHT;

      case FDIR_RIGHT:
        return FDIR_LEFT;

      case FDIR_UP:
        return FDIR_DOWN;

      case FDIR_DOWN:
        return FDIR_UP;

      default:
        throw std::invalid_argument(
            fmt::format("Unknown focus direction {}", (int)dir));
    }
  }();

  if (LastFocusableElementId != -1) {
    auto el = Children.at(LastFocusableElementId);
    el->SetFocus(widget, dir);
    widget->SetFocus(el, oppositeDir);
    if (!FocusStart[dir]) FocusStart[dir] = el;
    if (!FocusStart[oppositeDir]) FocusStart[oppositeDir] = el;
  } else {
    FirstFocusableElementId = (int)Children.size();
  }

  Add(widget);
  LastFocusableElementId = (int)Children.size() - 1;
  if (WrapFocus) {
    auto firstEl = Children.at(FirstFocusableElementId);
    widget->SetFocus(firstEl, dir);
    firstEl->SetFocus(widget, oppositeDir);
    FocusStart[oppositeDir] = widget;
  }
}

WidgetType Group::GetType() { return WT_GROUP; }

void Group::UpdateInput(float dt) {
  for (const auto& el : Children) {
    if (el->GetType() == WT_NORMAL) {
      el->UpdateInput(dt);
      if (el->Enabled && el->Hovered && el->Bounds.Intersects(HoverBounds) &&
          (Input::CurrentInputDevice == Input::Device::Mouse ||
           Input::CurrentInputDevice == Input::Device::Touch)) {
        if (MenuContext->CurrentlyFocusedElement &&
            el != MenuContext->CurrentlyFocusedElement)
          MenuContext->CurrentlyFocusedElement->HasFocus = false;
        el->HasFocus = true;
        MenuContext->CurrentlyFocusedElement = el;
      }
    }
  }
}

void Group::Update(float dt) {
  if (IsShown) {
    Widget::Update(dt);
    if ((FocusLock && HasFocus) || !FocusLock) {
      UpdateInput(dt);
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
    Renderer->EnableScissor();
    Renderer->SetScissorRect(RenderingBounds);
    for (const auto& el : Children) {
      if (RenderingBounds.Intersects(el->Bounds)) {
        auto tint = el->Tint;
        el->Tint *= Tint;
        el->Render();
        el->Tint = tint;
      }
    }
    Renderer->DisableScissor();
  }
}

Widget* Group::GetFocus(FocusDirection dir) {
  if (!Children.empty()) {
    if (dir == FDIR_DOWN || dir == FDIR_RIGHT)
      return Children.front();
    else if (dir == FDIR_UP || dir == FDIR_LEFT)
      return Children.back();
  }
  return NULL;
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

void Group::Move(glm::vec2 relativePosition) {
  for (const auto& el : Children) {
    el->Move(relativePosition);
  }
  Widget::Move(relativePosition);
}

void Group::Clear() {
  for (auto el : Children) {
    delete el;
  }
  Children.clear();
  LastFocusableElementId = -1;
  std::fill(std::begin(FocusStart), std::end(FocusStart), nullptr);
}

Widget* Group::GetFirstFocusableChild() {
  return Children.at(FirstFocusableElementId);
}

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto