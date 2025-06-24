#include "dialoguebox.h"

#include "../../renderer/renderer.h"
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

void DialogueBox::Render(DialoguePageMode mode, bool hasName, float nameWidth,
                         uint32_t nameId, float opacity) {
  glm::vec4 col = ScrWorkGetColor(SW_MESWINDOW_COLOR);
  col.a = opacity;
  if (mode == DPM_ADV) {
    Renderer->DrawSprite(ADVBoxPartLeft, ADVBoxPartLeftPos, col);
    Renderer->DrawSprite(ADVBoxSprite, ADVBoxPos, col);
    Renderer->DrawSprite(ADVBoxPartRight, ADVBoxPartRightPos, col);
    Renderer->DrawSprite(ADVBoxDecoration, ADVBoxDecorationPos, col);
  } else {
    glm::vec4 nvlBoxTint(0.0f, 0.0f, 0.0f, opacity * NVLBoxMaxOpacity);
    Renderer->DrawQuad(RectF(0, 0, Profile::DesignWidth, Profile::DesignHeight),
                       nvlBoxTint);
  }

  if (mode == DPM_ADV && hasName) {
    if (HaveADVNameTag) {
      Renderer->DrawSprite(ADVNameTag::LeftSprite, ADVNameTag::Position, col);
    }

    if (HaveADVNameTag) {
      // Name graphic additional length
      float lineWidth = nameWidth - ADVNameTag::BaseLineWidth;
      float lineX =
          ADVNameTag::Position.x + ADVNameTag::LeftSprite.ScaledWidth();
      while (lineWidth > 0.0f) {
        Sprite lineSprite = ADVNameTag::LineSprite;
        lineSprite.SetScaledWidth(fminf(lineSprite.ScaledWidth(), lineWidth));
        Renderer->DrawSprite(lineSprite,
                             glm::vec2(lineX, ADVNameTag::Position.y), col);
        lineX += lineSprite.ScaledWidth();
        lineWidth -= lineSprite.ScaledWidth();
      }
      Renderer->DrawSprite(ADVNameTag::RightSprite,
                           glm::vec2(lineX, ADVNameTag::Position.y), col);
    }
  }
}

}  // namespace MO6TW
}  // namespace Impacto