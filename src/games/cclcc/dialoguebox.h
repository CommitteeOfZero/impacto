#pragma once

#include "../../hud/dialoguebox.h"

namespace Impacto {
namespace CCLCC {

class DialogueBox : public Impacto::DialogueBox {
 public:
  void Render(DialoguePageMode mode, bool hasName, float nameWidth,
              float opacity) override;
};

}  // namespace CCLCC
}  // namespace Impacto