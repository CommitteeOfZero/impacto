#pragma once

#include <enum.h>
#include <flat_hash_map.hpp>
#include "../text.h"

namespace Impacto {

BETTER_ENUM(DialogueBoxType, int, None, Plain, Dash, MO6TW, CC)

enum DialogueBoxState { Hidden, Hiding, Showing, Shown };

class DialogueBox {
 public:
  virtual void Show();
  virtual void Hide();
  virtual void Update(float dt);
  virtual void Render(DialoguePageMode mode, bool hasName, float nameWidth,
                      uint32_t nameId, float opacity);

  DialogueBoxState State;
};

}  // namespace Impacto