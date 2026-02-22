#include "dialoguebox.h"

#include "../../renderer/renderer.h"
#include "../../text/dialoguepage.h"
#include "../../profile/dialogue.h"
#include "../../profile/game.h"
#include "../../profile/games/mo6tw/dialoguebox.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"

namespace Impacto {
namespace MO6TW {

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::Dialogue;
using namespace Impacto::Profile::MO6TW::DialogueBox;

void DialogueBox::Render(const DialoguePageMode mode, const NameInfo& nameInfo,
                         glm::vec4 tint) {
  tint = glm::vec4(glm::vec3(ScrWorkGetColor(SW_MESWINDOW_COLOR)), tint.a);

  if (mode == DPM_ADV) {
    Renderer->DrawSprite(ADVBoxPartLeft, ADVBoxPartLeftPos, tint);
    Renderer->DrawSprite(ADVBoxSprite, ADVBoxPos, tint);
    Renderer->DrawSprite(ADVBoxPartRight, ADVBoxPartRightPos, tint);
    Renderer->DrawSprite(ADVBoxDecoration, ADVBoxDecorationPos, tint);

    NametagDisplayInst->Render(nameInfo, tint);

  } else if (mode == DPM_NVL) {
    glm::vec4 nvlBoxTint(0.0f, 0.0f, 0.0f, tint.a * NVLBoxMaxOpacity);
    Renderer->DrawQuad(RectF(0, 0, Profile::DesignWidth, Profile::DesignHeight),
                       nvlBoxTint);
  }
}

}  // namespace MO6TW
}  // namespace Impacto