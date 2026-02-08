#include "dialoguebox.h"

#include "../../hud/nametagdisplay.h"
#include "../../text/dialoguepage.h"
#include "../../renderer/renderer.h"
#include "../../profile/dialogue.h"
#include "../../profile/game.h"
#include "../../profile/games/cc/dialoguebox.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../log.h"

namespace Impacto {
namespace CC {

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::Dialogue;
using namespace Impacto::Profile::CC::DialogueBox;

DialogueBox::DialogueBox() {
  TextBoxEffect.DurationIn = ADVBoxEffectDuration;
  TextBoxEffect.LoopMode = AnimationLoopMode::Loop;
  TextBoxEffect.StartIn();
}

void DialogueBox::Update(float dt) {
  Impacto::DialogueBox::Update(dt);
  TextBoxEffect.Update(dt);
}

void DialogueBox::Render(const DialoguePageMode mode, const NameInfo& nameInfo,
                         const glm::vec4 tint) {
  switch (mode) {
    case DPM_ADV: {
      Renderer->DrawCCMessageBox(ADVBoxSprite, ADVBoxMask, ADVBoxPos,
                                 glm::vec4(1.0f), (int)(tint.a * 272.0f), 16,
                                 TextBoxEffect.Progress);

      NametagDisplayInst->Render(nameInfo, tint);
    } break;

    case DPM_NVL: {
      glm::vec4 nvlBoxTint(0.0f, 0.0f, 0.0f, tint.a * NVLBoxMaxOpacity);
      Renderer->DrawQuad(
          RectF(0, 0, Profile::DesignWidth, Profile::DesignHeight), nvlBoxTint);
    } break;

    default:
      ImpLogSlow(LogLevel::Error, LogChannel::General,
                 "Unexpected dialogue page mode {:d} in Chaos;Child textbox",
                 static_cast<int>(mode));
      break;
  }
}

}  // namespace CC
}  // namespace Impacto