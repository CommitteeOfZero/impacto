#pragma once

#include "../../hud/dialoguebox.h"
#include "../../animation.h"

namespace Impacto {
namespace CC {

class DialogueBox : public Impacto::DialogueBox {
 public:
  void Update(float dt) override;
  void Render(DialoguePageMode mode, bool hasName, float nameWidth,
              uint32_t nameId, float opacity) override;

 private:
  Animation TextBoxEffect;
};

}  // namespace CC
}  // namespace Impacto