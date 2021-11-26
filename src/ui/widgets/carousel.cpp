#include "carousel.h"
#include "../../vm/interface/input.h"
#include "../../profile/scriptinput.h"
#include "../../inputsystem.h"

namespace Impacto {
namespace UI {
namespace Widgets {

using namespace Impacto::Profile::ScriptInput;
using namespace Impacto::Vm::Interface;

Carousel::Carousel(CarouselDirection dir) { Direction = dir; }

void Carousel::Update(float dt) {
  if (!Children.empty() && Iterator != Children.end()) (*Iterator)->Update(dt);
}

void Carousel::UpdateInput() {
  if (!Children.empty()) {
    auto buttonAdvance = Direction == CDIR_HORIZONTAL ? PAD1RIGHT : PAD1DOWN;
    auto buttonBack = Direction == CDIR_HORIZONTAL ? PAD1LEFT : PAD1UP;

    if (PADinputButtonWentDown & buttonBack) {
      (*Iterator)->Hide();
      if (Iterator == Children.begin()) {
        Iterator = Children.end();
      }
      Iterator--;
      (*Iterator)->Show();
    }
    if (PADinputButtonWentDown & buttonAdvance) {
      (*Iterator)->Hide();
      Iterator++;
      if (Iterator == Children.end()) {
        Iterator = Children.begin();
      }
      (*Iterator)->Show();
    }

    if (Iterator != Children.end()) (*Iterator)->UpdateInput();
  }
}

void Carousel::Render() {
  if (!Children.empty() && Iterator != Children.end()) (*Iterator)->Render();
}

void Carousel::Add(Widget* widget) {
  Children.push_back(widget);
  Iterator = Children.begin();
}

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto