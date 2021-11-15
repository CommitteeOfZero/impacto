#pragma once

#include <functional>

#include "../widget.h"
#include "../../spritesheet.h"
#include "../../text.h"

namespace Impacto {
namespace UI {
namespace Widgets {

class BacklogEntry : public Widget {
 public:
  BacklogEntry(int id, uint8_t* str, int audioId, glm::vec2 pos);

  void Update(float dt) override;
  void UpdateInput();
  void Render() override;

  void Move(glm::vec2 relativePosition) override;

  int Id;
  int AudioId = -1;
  float TextHeight = 0.0f;

  std::function<void(BacklogEntry*)> OnClickHandler;

 protected:
  DialoguePage* BacklogPage;
  int TextLength = 0;
  int NameLength = 0;
  float NameWidth = 0.0f;
};

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto