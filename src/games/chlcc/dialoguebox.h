#pragma once

#include "../../hud/dialoguebox.h"
#include "../../animation.h"

namespace Impacto {
namespace CHLCC {

class DialogueBox : public Impacto::DialogueBox {
 public:
  void Render(DialoguePageMode mode, float nameWidth,
              std::optional<uint32_t> nameId, float opacity) override;
};

}  // namespace CHLCC
}  // namespace Impacto