#include "dialoguebox.h"

#include "../../renderer/renderer.h"
#include "../../text/dialoguepage.h"
#include "../../profile/dialogue.h"
#include "../../profile/game.h"
#include "../../profile/games/chlcc/dialoguebox.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../log.h"

namespace Impacto {
namespace CHLCC {

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::Dialogue;
using namespace Impacto::Profile::CHLCC::DialogueBox;

void DialogueBox::Render(const DialoguePageMode mode, const NameInfo& nameInfo,
                         glm::vec4 tint) {
  switch (mode) {
    case DPM_ADV: {
      tint = {1.0f, 1.0f, 1.0f, tint.a};
      const Sprite advBoxSprite = ScrWork[SW_MESWINDOW_COLOR] == 1
                                      ? SecondaryADVBoxSprite
                                      : ADVBoxSprite;
      Renderer->DrawSprite(advBoxSprite, ADVBoxPos, tint);

      NametagDisplayInst->Render(nameInfo, tint);
    } break;

    case DPM_REV: {
      if (ScrWork[SW_MESWIN0TYPE] != 1) break;

      Renderer->DrawSprite(ErinBoxSprite, ErinBoxPos, glm::vec4(1.0f));
    } break;

    case DPM_NVL: {
      glm::vec4 nvlBoxTint(0.0f, 0.0f, 0.0f, tint.a * NVLBoxMaxOpacity);
      Renderer->DrawQuad(
          RectF(0, 0, Profile::DesignWidth, Profile::DesignHeight), nvlBoxTint);
    } break;

    default:
      ImpLogSlow(LogLevel::Warning, LogChannel::General,
                 "Unexpected dialogue page mode {:d} when rendering Chaos;Head "
                 "LCC dialogue box",
                 static_cast<int>(mode));
      break;
  }
}

}  // namespace CHLCC
}  // namespace Impacto