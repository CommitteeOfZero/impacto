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

void DialogueBox::Init() {
  Impacto::DialogueBox::Init();

  SkipAnimation = Profile::CC::DialogueBox::SkipAnimation.Instantiate();
  AutoAnimation = Profile::CC::DialogueBox::AutoAnimation.Instantiate();
  SkipAnimation.DurationIn = SkipAutoAnimationDuration;
  AutoAnimation.DurationIn = SkipAutoAnimationDuration;
  SkipAnimation.DurationOut = SkipAutoAnimationDuration;
  AutoAnimation.DurationOut = SkipAutoAnimationDuration;
  SkipAnimation.LoopMode = ALM_Stop;
  AutoAnimation.LoopMode = ALM_Stop;
}

void DialogueBox::Update(float dt) {
  if (TextBoxEffect.State != AS_Playing) {
    TextBoxEffect.DurationIn = ADVBoxEffectDuration;
    TextBoxEffect.LoopMode = ALM_Loop;
    TextBoxEffect.StartIn();
  }

  TextBoxEffect.Update(dt);

  if ((MesSkipMode & 0b11) && SkipAnimation.Direction == -1)
    SkipAnimation.StartIn();
  if (!(MesSkipMode & 0b11) && SkipAnimation.Direction == 1)
    SkipAnimation.StartOut();
  SkipAnimation.Update(dt);

  if ((MesSkipMode & 0b100) && AutoAnimation.Direction == -1)
    AutoAnimation.StartIn();
  if (!(MesSkipMode & 0b100) && AutoAnimation.Direction == 1)
    AutoAnimation.StartOut();
  AutoAnimation.Update(dt);
}

void DialogueBox::Render(DialoguePageMode mode, bool hasName, float nameWidth,
                         uint32_t nameId, float opacity) {
  const glm::vec4 col = glm::vec4(1.0f);
  if (mode == DPM_ADV) {
    Renderer->DrawCCMessageBox(ADVBoxSprite, ADVBoxMask, ADVBoxPos, col,
                               (int)(opacity * 272.0f), 16,
                               TextBoxEffect.Progress);
    if (hasName) {
      Renderer->DrawSprite(NamePlateMainSprites[nameId],
                           ADVBoxNamePlateMainPos);
      Renderer->DrawSprite(NamePlateLabelSprites[nameId],
                           ADVBoxNamePlateLabelPos);
    }
  } else {
    glm::vec4 nvlBoxTint(0.0f, 0.0f, 0.0f, opacity * NVLBoxMaxOpacity);
    Renderer->DrawRect(RectF(0, 0, Profile::DesignWidth, Profile::DesignHeight),
                       nvlBoxTint);
  }

  // Draw skip mode sprite
  if (!SkipAnimation.IsOut())
    Renderer->DrawSprite(SkipAnimation.CurrentSprite(), SkipIconPos);

  // Draw auto mode sprite
  if (!AutoAnimation.IsOut())
    Renderer->DrawSprite(AutoAnimation.CurrentSprite(), AutoIconPos);
}

}  // namespace CC
}  // namespace Impacto