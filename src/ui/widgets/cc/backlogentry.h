#pragma once

#include "../backlogentry.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CC {

class BacklogEntry : public Widgets::BacklogEntry {
 public:
  BacklogEntry(int id, uint8_t* str, int audioId, glm::vec2 pos,
               const RectF& hoverBounds);

  void Render();
};

}  // namespace CC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto