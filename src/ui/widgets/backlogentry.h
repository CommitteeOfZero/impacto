#pragma once

#include <functional>

#include "../../profile/dialogue.h"
#include "../widget.h"
#include "../../text.h"
#include "../../vm/vm.h"

namespace Impacto {
namespace UI {
namespace Widgets {

class BacklogEntry : public Widget {
 public:
  BacklogEntry(int id, Vm::BufferOffsetContext scrCtx, int audioId,
               int characterId, glm::vec2 pos, const RectF& hoverBounds);
  ~BacklogEntry();

  void UpdateInput(float dt) override;
  void Render() override;

  using Widget::Move;
  void Move(glm::vec2 relativePosition) override;

  int Id;
  int AudioId = -1;
  int CharacterId = 0;
  float TextHeight = 0.0f;

  std::function<void(BacklogEntry*)> OnClickHandler;

 protected:
  DialoguePage* BacklogPage;
  int TextLength = 0;

 private:
  glm::vec2 Position;

  RectF REVBounds = Profile::Dialogue::REVBounds;
  const RectF& HoverBounds;
};

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto