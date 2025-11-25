#pragma once

#include "../../hud/dialoguebox.h"

namespace Impacto {
namespace MO6TW {

class DialogueBox : public Impacto::DialogueBox {
 public:
  void Render(DialoguePageMode mode, float nameWidth,
              std::optional<uint32_t> nameId, float opacity) override;
};

}  // namespace MO6TW
}  // namespace Impacto