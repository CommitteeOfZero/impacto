#include "widgetgroup.h"

#include "../inputsystem.h"
#include "../vm/interface/input.h"

namespace Impacto {
namespace UI {

using namespace Impacto::Vm::Interface;

void WidgetGroup::Add(Widget* widget, int focusDirection) {
  // TODO: Yes, this doesn't work if we delete widgets after init. Do we even
  // *need* to though?
  if (widget->Enabled) {
    if (focusDirection & FocusDirection::Vertical) {
      VerticalFocusChain.push_back(Children.size());
    }
    if (focusDirection & FocusDirection::Horizontal) {
      HorizontalFocusChain.push_back(Children.size());
    }
  }
  Children.push_back(widget);
}

void WidgetGroup::AddToFocusChain(Widget* widget, int focusDirection) {
  if (widget->Enabled) {
    auto it = std::find(Children.begin(), Children.end(), widget);
    int id = std::distance(Children.begin(), it);
    if (focusDirection & FocusDirection::Vertical) {
      VerticalFocusChain.push_back(id);
    }
    if (focusDirection & FocusDirection::Horizontal) {
      HorizontalFocusChain.push_back(id);
    }
  }
}

void WidgetGroup::FocusChainAdvance(std::vector<int> const& focusChain,
                                    bool forward) {
  auto it = std::find(focusChain.begin(), focusChain.end(), FocusId);
  if (forward) {
    if (it != focusChain.end()) {
      Children[*it]->HasFocus = false;
      auto next = std::next(it);
      if (next != focusChain.end()) {
        FocusId = *next;
      } else {
        FocusId = focusChain[0];
      }
    } else {
      FocusId = focusChain[0];
    }
    Children[FocusId]->HasFocus = true;
  } else {
    if (it != focusChain.end()) {
      Children[*it]->HasFocus = false;
      if (it == focusChain.begin()) {
        FocusId = focusChain.back();
      } else {
        auto prev = std::prev(it);
        FocusId = *prev;
      }
    } else {
      FocusId = focusChain[focusChain.size() - 1];
    }
    Children[FocusId]->HasFocus = true;
  }
}

void WidgetGroup::UpdateInput() {
  if (VerticalFocusChain.size() > 0) {
    if (PADinputButtonWentDown & PAD1DOWN) {
      FocusChainAdvance(VerticalFocusChain, true);
    } else if (PADinputButtonWentDown & PAD1UP) {
      FocusChainAdvance(VerticalFocusChain, false);
    }
  }
  if (HorizontalFocusChain.size() > 0) {
    if (PADinputButtonWentDown & PAD1RIGHT) {
      FocusChainAdvance(HorizontalFocusChain, true);
    } else if (PADinputButtonWentDown & PAD1LEFT) {
      FocusChainAdvance(HorizontalFocusChain, false);
    }
  }

  for (auto el : Children) {
    if (el->Enabled && el->HasFocus) {
      auto it = std::find(Children.begin(), Children.end(), el);
      FocusId = std::distance(Children.begin(), it);
    }
    el->UpdateInput();
  }
}
void WidgetGroup::Update(float dt) {
  if (IsShown) {
    if (HasFocus) {
      UpdateInput();
    }
    for (auto el : Children) {
      el->Update(dt);
    }
  }
}
void WidgetGroup::Render() {
  if (IsShown) {
    for (auto el : Children) {
      el->Opacity = Opacity;
      el->Render();
    }
  }
}

void WidgetGroup::Show() {
  Opacity = 1.0f;
  IsShown = true;
  HasFocus = true;
}

void WidgetGroup::Hide() {
  Opacity = 0.0f;
  IsShown = false;
  HasFocus = false;
}

}  // namespace UI
}  // namespace Impacto