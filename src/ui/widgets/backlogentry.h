#pragma once

#include <functional>

#include "../../profile/dialogue.h"
#include "../widget.h"
#include "../../text/backlogpage.h"
#include "../../vm/vm.h"

namespace Impacto {
namespace UI {
namespace Widgets {

class BacklogEntry : public Widget {
 public:
  BacklogEntry(Vm::BufferOffsetContext scrCtx, std::optional<int> audioId,
               int characterId, glm::vec2 pos);

  void UpdateInput(float dt) override;
  void Render() override;

  using Widget::Move;
  void Move(glm::vec2 relativePosition) override;

  std::optional<int> AudioId;
  int CharacterId = 0;

  std::function<void(BacklogEntry*)> OnClickHandler;

  BacklogPage Page;

 private:
  glm::vec2 Position;
};

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto