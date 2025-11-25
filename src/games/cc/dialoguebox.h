#pragma once

#include "../../hud/dialoguebox.h"
#include "../../animation.h"
#include "../../spriteanimation.h"

namespace Impacto {
namespace CC {

class DialogueBox : public Impacto::DialogueBox {
 public:
  void Init() override;
  void Update(float dt) override;
  void Render(DialoguePageMode mode, float nameWidth,
              std::optional<uint32_t> nameId, float opacity) override;

 private:
  Animation TextBoxEffect;
  void RenderNameSprites(uint32_t nameId, float progress,
                         AnimationDirection direction);
};

}  // namespace CC
}  // namespace Impacto