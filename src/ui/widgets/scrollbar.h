#pragma once

#include <functional>

#include "../widget.h"
#include "../../spritesheet.h"
#include "../../text.h"

namespace Impacto {
namespace UI {
namespace Widgets {

enum ScrollbarDirection { SBDIR_VERTICAL, SBDIR_HORIZONTAL };

class Scrollbar : public Widget {
 public:
  Scrollbar(int id, glm::vec2 pos, float min, float max,
            float* value, ScrollbarDirection dir, Sprite const& track,
            Sprite const& thumb);

  void UpdateInput();
  virtual void Render();

  int Id;
  ScrollbarDirection Direction;
  Sprite TrackSprite;
  Sprite ThumbSprite;
  float Length;
  float MinValue;
  float MaxValue;
  float* Value;

 protected:
  RectF TrackBounds;
  bool Scrolling = false;
};

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto