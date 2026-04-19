#pragma once

#include "../dialoguebox.h"

namespace Impacto {
namespace MO6TW {

class DialogueBox : public Impacto::DialogueBox {
 public:
  DialogueBox(const DialoguePage& page) : Impacto::DialogueBox(page) {}

  void Render(DialoguePageMode mode, const NameInfo& nameInfo,
              glm::vec4 tint) override;
};

}  // namespace MO6TW
}  // namespace Impacto