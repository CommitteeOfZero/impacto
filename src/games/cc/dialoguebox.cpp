#include "dialoguebox.h"

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

void DialogueBox::Render(DialoguePageMode mode, bool hasName, float nameWidth,
                         uint32_t nameId, float opacity,
                         const Animation& nametagAnim) {
  const glm::vec4 col = glm::vec4(1.0f);
  if (mode == DPM_ADV) {
    Renderer->DrawCCMessageBox(ADVBoxSprite, ADVBoxMask, ADVBoxPos, col,
                               (int)(opacity * 272.0f), 16,
                               TextBoxEffect.Progress);

    if (nametagAnim.Progress < NameTagAnimStage1) {
      // hiding old name
      if (hasName) {
        const float progressHiding = nametagAnim.Progress / NameTagAnimStage1;
        RenderNametags(nameId, progressHiding, AnimationDirection::Out);
      }
    } else if (nametagAnim.Progress >= NameTagAnimStage2) {
      // showing new name
      if (hasName) {
        float progressShowing = (nametagAnim.Progress - NameTagAnimStage2) /
                                (1 - NameTagAnimStage2);
        RenderNametags(nameId, progressShowing, AnimationDirection::In);
      }
    }
    return;
  }

  glm::vec4 nvlBoxTint(0.0f, 0.0f, 0.0f, opacity * NVLBoxMaxOpacity);
  Renderer->DrawQuad(RectF(0, 0, Profile::DesignWidth, Profile::DesignHeight),
                     nvlBoxTint);
}

void DialogueBox::RenderNametags(uint32_t nameId, float progress,
                                 AnimationDirection direction) {
  const float invertedProgress = 1 - progress;

  const float alpha =
      direction == AnimationDirection::In ? progress : invertedProgress;
  const float translationProgress =
      (direction == AnimationDirection::In ? invertedProgress : progress);

  glm::vec4 tint = glm::vec4(1.0f, 1.0f, 1.0f, alpha);

  const glm::vec2 mainPlateTranslation = glm::vec2(
      -NamePlateMainSprites[nameId].Bounds.Width * translationProgress, 0);

  const glm::mat4 mainPlateTransformationMatrix =
      TransformationMatrix({0, 0}, {1, 1}, {0, 0}, 0, mainPlateTranslation);

  const CornersQuad dest =
      NamePlateMainSprites[nameId].ScaledBounds().Translate(
          ADVBoxNamePlateMainPos);

  const CornersQuad namePlateDest =
      NamePlateLabelSprites[nameId].ScaledBounds().Translate(
          ADVBoxNamePlateLabelPos);

  const glm::vec2 namePlateScale = glm::vec2(alpha, 1);
  glm::vec2 namePlateScaleOrigin =
      NamePlateLabelSprites[nameId].Bounds.GetSize();
  namePlateScaleOrigin.x /= 2;

  const glm::mat4 namePlateTransformationMatrix = TransformationMatrix(
      namePlateScaleOrigin, namePlateScale, {0, 0}, 0, {0, 0});

  Renderer->DrawSprite(NamePlateMainSprites[nameId], dest,
                       mainPlateTransformationMatrix, tint);
  Renderer->DrawSprite(NamePlateLabelSprites[nameId], namePlateDest,
                       namePlateTransformationMatrix, tint);
}

}  // namespace CC
}  // namespace Impacto