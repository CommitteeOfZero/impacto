#include "carousel.h"
#include "../../vm/interface/input.h"
#include "../../profile/scriptinput.h"
#include "../../inputsystem.h"

namespace Impacto {
namespace UI {
namespace Widgets {

using namespace Impacto::Profile::ScriptInput;
using namespace Impacto::Vm::Interface;

Carousel::Carousel(CarouselDirection dir) {
  Direction = dir;
  OnAdvanceHandler = [this](Widget* current, Widget* next) {
    return OnChange(current, next);
  };
  OnBackHandler = [this](Widget* current, Widget* next) {
    return OnChange(current, next);
  };
}

Carousel::Carousel(CarouselDirection dir,
                   std::function<void(Widget*, Widget*)> onAdvanceHandler,
                   std::function<void(Widget*, Widget*)> onBackHandler) {
  Direction = dir;
  OnAdvanceHandler = onAdvanceHandler;
  OnBackHandler = onBackHandler;
}

void Carousel::Update(float dt) {
  for (const auto& el : Children) {
    el->Update(dt);
  }
}

void Carousel::UpdateInput() {
  if (!Children.empty()) {
    auto buttonAdvance = Direction == CDIR_HORIZONTAL ? PAD1RIGHT : PAD1DOWN;
    auto buttonBack = Direction == CDIR_HORIZONTAL ? PAD1LEFT : PAD1UP;

    if (PADinputButtonWentDown & buttonBack) {
      Previous();
    }
    if (PADinputButtonWentDown & buttonAdvance) {
      Next();
    }
  }
}

void Carousel::Render() {
  for (const auto& el : Children) {
    auto tint = el->Tint;
    el->Tint *= Tint;
    el->Render();
    el->Tint = tint;
  }
}

void Carousel::Add(Widget* widget) {
  Children.push_back(widget);
  Iterator = Children.begin();
}

void Carousel::Next() {
  auto current = *Iterator;
  Iterator++;
  if (Iterator == Children.end()) {
    Iterator = Children.begin();
  }
  auto next = *Iterator;
  OnAdvanceHandler(current, next);
}

void Carousel::Previous() {
  auto current = *Iterator;
  if (Iterator == Children.begin()) {
    Iterator = Children.end();
  }
  Iterator--;
  auto next = *Iterator;
  OnBackHandler(current, next);
}

void Carousel::OnChange(Widget* current, Widget* next) {
  current->Hide();
  next->Show();
}

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto