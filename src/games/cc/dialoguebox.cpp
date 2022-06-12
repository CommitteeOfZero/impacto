#include "dialoguebox.h"

#include "../../renderer2d.h"
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

void DialogueBox::Update(float dt) {
  if (TextBoxEffect.State != AS_Playing) {
    TextBoxEffect.DurationIn = ADVBoxEffectDuration;
    TextBoxEffect.LoopMode = ALM_Loop;
    TextBoxEffect.StartIn();
  }

  TextBoxEffect.Update(dt);
}

void DialogueBox::Render(DialoguePageMode mode, bool hasName, float nameWidth,
                         uint32_t nameId, float opacity) {
  glm::vec4 col = ScrWorkGetColor(SW_MESWINDOW_COLOR);
  col.a = 1.0f;
  if (mode == DPM_ADV) {
    Renderer2D::DrawCCMessageBox(ADVBoxSprite, ADVBoxMask, ADVBoxPos, col,
                                 opacity * 272, 16, TextBoxEffect.Progress);
    if (hasName) {
      Renderer2D::DrawSprite(NamePlateMainSprites[nameId],
                             ADVBoxNamePlateMainPos);
      Renderer2D::DrawSprite(NamePlateLabelSprites[nameId],
                             ADVBoxNamePlateLabelPos);
    }
  } else {
    glm::vec4 nvlBoxTint(0.0f, 0.0f, 0.0f, opacity * NVLBoxMaxOpacity);
    Renderer2D::DrawRect(
        RectF(0, 0, Profile::DesignWidth, Profile::DesignHeight), nvlBoxTint);
  }
}

}  // namespace CC
}  // namespace Impacto