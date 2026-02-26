#include "nametagdisplay.h"

#include "../../profile/games/cc/dialoguebox.h"
#include "../../renderer/renderer.h"

namespace Impacto {
namespace CC {

using namespace Impacto::Profile::CC::DialogueBox;

NametagDisplay::NametagDisplay() {
  ShowAnimation.SetDuration(NametagShowDuration);
  ShowAnimation.SkipOnSkipMode = true;
  ShowAnimation.Reset(AnimationDirection::In);
}

void NametagDisplay::Render(const NameInfo& nameInfo, const glm::vec4 tint) {
  if (ShowAnimation.IsOut() || !nameInfo.RenderWindow ||
      !nameInfo.NameId.has_value() ||
      nameInfo.NameId.value() >= std::ssize(NametagMainSprites)) {
    return;
  }

  const Sprite& mainSprite = NametagMainSprites[*nameInfo.NameId];
  const Sprite& labelSprite = NametagLabelSprites[*nameInfo.NameId];

  const glm::vec2 mainStartPos = {-mainSprite.ScaledWidth(), NametagMainPos.y};
  const glm::vec2 mainPos =
      glm::mix(mainStartPos, NametagMainPos, ShowAnimation.Progress);
  Renderer->DrawSprite(mainSprite, mainPos, tint);

  const RectF labelDest =
      labelSprite.ScaledBounds()
          .Translate(NametagLabelPos)
          .ScaleAroundCenter({ShowAnimation.Progress, 1.0f});
  Renderer->DrawSprite(labelSprite, labelDest, tint);
}

void NametagDisplay::Update(const float dt) { ShowAnimation.Update(dt); }

void NametagDisplay::Reset() { ShowAnimation.Reset(AnimationDirection::In); }

void NametagDisplay::Show() { ShowAnimation.StartIn(); }

void NametagDisplay::Hide() { ShowAnimation.StartOut(); }

}  // namespace CC
}  // namespace Impacto
