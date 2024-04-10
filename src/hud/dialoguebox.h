#pragma once

#include <enum.h>
#include <flat_hash_map.hpp>
#include "../text.h"
#include "../ui/widgets/button.h"

namespace Impacto {

BETTER_ENUM(DialogueBoxType, int, None, Plain, Dash, MO6TW, CHLCC, CC)

enum DialogueBoxState { Hidden, Hiding, Showing, Shown };

class DialogueBox {
 public:
  virtual void Init();
  virtual void Show();
  virtual void Hide();
  virtual void Update(float dt);
  virtual void Render(DialoguePageMode mode, bool hasName, float nameWidth,
                      uint32_t nameId, float opacity);

  DialogueBoxState State;

 private:
  std::vector<UI::Widgets::Button*> ControlButtons;
  virtual void UpdateControlButtons(float dt);
  virtual void RenderControlButtons(glm::vec4 col);
};

}  // namespace Impacto