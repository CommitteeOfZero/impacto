#pragma once

#include "../widget.h"
#include <vector>

namespace Impacto {
namespace UI {
namespace Widgets {

enum CarouselDirection { CDIR_VERTICAL, CDIR_HORIZONTAL };

class Carousel : public Widget {
 public:
  Carousel(CarouselDirection dir);

  void Update(float dt) override;
  void UpdateInput();
  void Render();

  void Add(Widget* widget);

 private:
  CarouselDirection Direction;
  std::vector<Widget*> Children;
  std::vector<Widget*>::iterator Iterator;
};

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto