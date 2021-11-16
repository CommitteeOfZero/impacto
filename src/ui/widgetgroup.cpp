#include "widgetgroup.h"

#include "../inputsystem.h"
#include "../vm/interface/input.h"
#include "../profile/game.h"

namespace Impacto {
namespace UI {

using namespace Impacto::Vm::Interface;

WidgetGroup::WidgetGroup() {
  Position = glm::vec2(0.0f);
  RenderingBounds =
      RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight);
}
WidgetGroup::WidgetGroup(glm::vec2 pos) {
  Position = pos;
  RenderingBounds =
      RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight);
}

void WidgetGroup::Add(Widget* widget, int focusDirection) {
  // TODO: Yes, this doesn't work if we delete widgets after init. Do we even
  // *need* to though?
  if (focusDirection & FocusDirection::Vertical) {
    VerticalFocusChain.push_back(Children.size());
  }
  if (focusDirection & FocusDirection::Horizontal) {
    HorizontalFocusChain.push_back(Children.size());
  }
  if (widget->HasFocus) FocusId = Children.size();
  Children.push_back(widget);
}

void WidgetGroup::AddToFocusChain(Widget* widget, int focusDirection) {
  auto it = std::find(Children.begin(), Children.end(), widget);
  int id = std::distance(Children.begin(), it);
  if (focusDirection & FocusDirection::Vertical) {
    VerticalFocusChain.push_back(id);
  }
  if (focusDirection & FocusDirection::Horizontal) {
    HorizontalFocusChain.push_back(id);
  }
}

void WidgetGroup::FocusChainAdvance(std::vector<int> const& focusChain,
                                    bool forward) {
  auto it = std::find(focusChain.begin(), focusChain.end(), FocusId);
  if (forward) {
    if (it == focusChain.end()) {
      it = focusChain.begin();
    } else {
      Children[*it]->HasFocus = false;
      it = std::next(it);
      if (it == focusChain.end()) it = focusChain.begin();
    }
    if (it != focusChain.end()) {
      while (!Children[*it]->Enabled) {
        it = std::next(it);
        if (it == focusChain.end()) {
          it = focusChain.begin();
        }
      }
      FocusId = *it;
      Children[FocusId]->HasFocus = true;
    }
  } else {
    if (it == focusChain.end()) {
      it = std::prev(it);
    } else {
      Children[*it]->HasFocus = false;
      if (it == focusChain.begin()) it = focusChain.end();
      it = std::prev(it);
    }
    if (it != focusChain.end()) {
      while (!Children[*it]->Enabled) {
        if (it == focusChain.begin()) {
          it = focusChain.end();
        }
        it = std::prev(it);
      }
      FocusId = *it;
      Children[FocusId]->HasFocus = true;
    }
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
    el->UpdateInput();
    if (el->Enabled && el->Hovered &&
        Input::CurrentInputDevice == Input::IDEV_Mouse) {
      if (FocusId != -1) Children[FocusId]->HasFocus = false;
      el->HasFocus = true;
      auto it = std::find(Children.begin(), Children.end(), el);
      FocusId = std::distance(Children.begin(), it);
    }
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
      if (RenderingBounds.Contains(el->Bounds)) {
        float alpha = el->Tint.a;
        el->Tint.a *= Opacity;
        el->Render();
        el->Tint.a = alpha;
      }
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

void WidgetGroup::Move(glm::vec2 relativePosition, float duration) {
  for (auto el : Children) {
    el->Move(relativePosition, duration);
  }
  Position += relativePosition;
}

void WidgetGroup::Move(glm::vec2 relativePosition) {
  for (auto el : Children) {
    el->Move(relativePosition);
  }
  Position += relativePosition;
}

void WidgetGroup::MoveTo(glm::vec2 pos, float duration) {
  auto relativePosition = pos - Position;
  for (auto el : Children) {
    el->Move(relativePosition, duration);
  }
  Position = pos;
}

void WidgetGroup::MoveTo(glm::vec2 pos) {
  auto relativePosition = pos - Position;
  for (auto el : Children) {
    el->Move(relativePosition);
  }
  Position = pos;
}

void WidgetGroup::Clear() {
  for (auto el : Children) {
    delete el;
  }
  Children.clear();
  VerticalFocusChain.clear();
  HorizontalFocusChain.clear();
  FocusId = -1;
}

}  // namespace UI
}  // namespace Impacto