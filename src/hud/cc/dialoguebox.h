#pragma once

#include "../dialoguebox.h"
#include "../../animation.h"
#include "../../spriteanimation.h"

namespace Impacto {
namespace CC {

class DialogueBox : public Impacto::DialogueBox {
 public:
  DialogueBox();

  void Update(float dt) override;
  void Render(DialoguePageMode mode, const NameInfo& nameInfo,
              glm::vec4 tint) override;

 private:
  Animation TextBoxEffect;
};

}  // namespace CC
}  // namespace Impacto