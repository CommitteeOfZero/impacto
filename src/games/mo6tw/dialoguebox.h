#pragma once

#include "../../hud/dialoguebox.h"

namespace Impacto {
namespace MO6TW {

class DialogueBox : public Impacto::DialogueBox {
 public:
  void Render(DialoguePageMode mode, bool hasName, float nameWidth,
              uint32_t nameId, float opacity,
              const Animation& nameTagAnim) override;
};

}  // namespace MO6TW
}  // namespace Impacto