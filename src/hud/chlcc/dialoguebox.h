#pragma once

#include "../dialoguebox.h"
#include "../../animation.h"

namespace Impacto {
namespace CHLCC {

class DialogueBox : public Impacto::DialogueBox {
 public:
  DialogueBox(const DialoguePage& page) : Impacto::DialogueBox(page) {}

  void Render(DialoguePageMode mode, const NameInfo& nameInfo,
              glm::vec4 tint) override;
};

}  // namespace CHLCC
}  // namespace Impacto