#pragma once

#include "../backlogentry.h"
#include "../label.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

class BacklogEntry : public Widgets::BacklogEntry {
 public:
  BacklogEntry(int id, Vm::BufferOffsetContext scrCtx, int audioId,
               int characterId, glm::vec2 pos, const RectF& hoverBounds);

  void Render() override;

 private:
  Label beforeNametagLabel;
  Label nametagLabel;
  Label afterNametagLabel;
};

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto