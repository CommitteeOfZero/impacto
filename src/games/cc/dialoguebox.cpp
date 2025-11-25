#include "dialoguebox.h"

#include "../../hud/nametagdisplay.h"
#include "../../renderer/renderer.h"
#include "../../profile/dialogue.h"
#include "../../profile/game.h"
#include "../../profile/games/cc/dialoguebox.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"

namespace Impacto {
namespace CC {

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::Dialogue;
using namespace Impacto::Profile::CC::DialogueBox;

void DialogueBox::Init() { Impacto::DialogueBox::Init(); }

void DialogueBox::Update(float dt) {
  if (TextBoxEffect.State != AnimationState::Playing) {
    TextBoxEffect.DurationIn = ADVBoxEffectDuration;
    TextBoxEffect.LoopMode = AnimationLoopMode::Loop;
    TextBoxEffect.StartIn();
  }

  TextBoxEffect.Update(dt);
}

void DialogueBox::Render(DialoguePageMode mode, float nameWidth,
                         std::optional<uint32_t> nameId, float opacity) {
  const glm::vec4 col = glm::vec4(1.0f);
  if (mode == DPM_ADV) {
    Renderer->DrawCCMessageBox(ADVBoxSprite, ADVBoxMask, ADVBoxPos, col,
                               (int)(opacity * 272.0f), 16,
                               TextBoxEffect.Progress);
    if (!nameId.has_value()) {
      return;
    }

    if (NametagDisplay::IsHiding()) {
      // hiding old name
      const float progressHiding = NametagDisplay::GetHidingProgress();
      RenderNameSprites(nameId.value(), progressHiding,
                        AnimationDirection::Out);
    } else if (NametagDisplay::IsShowing()) {
      // showing new name
      float progressShowing = NametagDisplay::GetShowingProgress();
      RenderNameSprites(nameId.value(), progressShowing,
                        AnimationDirection::In);
    }
    return;
  }

  glm::vec4 nvlBoxTint(0.0f, 0.0f, 0.0f, opacity * NVLBoxMaxOpacity);
  Renderer->DrawQuad(RectF(0, 0, Profile::DesignWidth, Profile::DesignHeight),
                     nvlBoxTint);
}

void DialogueBox::RenderNameSprites(uint32_t nameId, float progress,
                                    AnimationDirection direction) {
  const float invertedProgress = 1.0f - progress;

  const float alpha =
      direction == AnimationDirection::In ? progress : invertedProgress;

  glm::vec4 tint = glm::vec4(1.0f, 1.0f, 1.0f, alpha);

  const float translationProgress =
      (direction == AnimationDirection::In ? invertedProgress : progress);

  const glm::vec2 mainPlateTranslation = glm::vec2(
      -NamePlateMainSprites[nameId].Bounds.Width * translationProgress, 0);

  const CornersQuad mainPlateDest = NamePlateMainSprites[nameId]
                                        .ScaledBounds()
                                        .Translate(ADVBoxNamePlateMainPos)
                                        .Translate(mainPlateTranslation);

  const CornersQuad labelPlateDest = NamePlateLabelSprites[nameId]
                                         .ScaledBounds()
                                         .Translate(ADVBoxNamePlateLabelPos)
                                         .ScaleAroundCenter({alpha, 1.0f});

  Renderer->DrawSprite(NamePlateMainSprites[nameId], mainPlateDest, tint);
  Renderer->DrawSprite(NamePlateLabelSprites[nameId], labelPlateDest, tint);
}

}  // namespace CC
}  // namespace Impacto