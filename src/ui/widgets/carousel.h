#pragma once

#include "../widget.h"
#include <vector>
#include <functional>

namespace Impacto {
namespace UI {
namespace Widgets {

enum CarouselDirection { CDIR_VERTICAL, CDIR_HORIZONTAL };

class Carousel : public Widget {
 public:
  Carousel(CarouselDirection dir);
  Carousel(CarouselDirection dir,
           std::function<void(Widget*, Widget*)> onAdvanceHandler,
           std::function<void(Widget*, Widget*)> onBackHandler);

  void Update(float dt) override;
  void UpdateInput() override;
  void Render() override;

  void Add(Widget* widget);
  void Next();
  void Previous();

 private:
  void OnChange(Widget* current, Widget* next);

  std::function<void(Widget*, Widget*)> OnAdvanceHandler;
  std::function<void(Widget*, Widget*)> OnBackHandler;

  CarouselDirection Direction;
  std::vector<Widget*> Children;
  std::vector<Widget*>::iterator Iterator;
};

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto