#pragma once

#include "../../hud/dialoguebox.h"
#include "../../animation.h"

namespace Impacto {
namespace CHLCC {

class DialogueBox : public Impacto::DialogueBox {
 public:
  void Render(DialoguePageMode mode, std::optional<NameInfo> nameInfo,
              glm::vec4 tint) override;
};

}  // namespace CHLCC
}  // namespace Impacto