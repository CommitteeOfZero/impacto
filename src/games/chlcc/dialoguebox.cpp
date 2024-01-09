#include "dialoguebox.h"

#include "../../renderer/renderer.h"
#include "../../profile/dialogue.h"
#include "../../profile/game.h"
#include "../../profile/games/chlcc/dialoguebox.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"

namespace Impacto {
namespace CHLCC {

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::Dialogue;
using namespace Impacto::Profile::CHLCC::DialogueBox;

void DialogueBox::Render(DialoguePageMode mode, bool hasName, float nameWidth,
                         uint32_t nameId, float opacity) {
  if (mode == DPM_ADV) {
    glm::vec4 col = glm::vec4(1.0f);
    Sprite* advBoxSprite = &ADVBoxSprite;
    switch (ScrWork[SW_MESWINDOW_COLOR]) {
      case 1:
        // For the pink textbox with the girls, we can't get the color from the
        // ScrWork as it *acts* as a boolean.
        advBoxSprite = &SecondaryADVBoxSprite;
        break;
      case 0xFFFFFF:
        // On the first loaded script, it forces 0xffffff for the normal textbox.
        advBoxSprite = &ADVBoxSprite;
        col = ScrWorkGetColor(SW_MESWINDOW_COLOR);
        break;
    }
    // On subsequent calls it's equal to 0 and *acts* as a boolean again.
    col.a = opacity;
    Renderer->DrawSprite(*advBoxSprite, ADVBoxPos, col);
  }

  if (mode == DPM_REV) {
    glm::vec4 col = glm::vec4(1.0f);
    Renderer->DrawSprite(REVBoxSprite, REVBoxPos, col);
  }

  if (mode == DPM_ADV && hasName) {
    if (HaveADVNameTag) {
      glm::vec4 col = glm::vec4(1.0f);
      Sprite* advNameTagLeftSprite = &ADVNameTag::LeftSprite;
      glm::vec2* advNameTagPosition = &ADVNameTag::Position;
      float* advNameTagBaseLineWidth = &ADVNameTag::BaseLineWidth;
      Sprite* advNameTagLineSprite = &ADVNameTag::LineSprite;
      Sprite* advNameTagRightSprite = &ADVNameTag::RightSprite;
      switch (ScrWork[SW_MESWINDOW_COLOR]) {
        case 1:
          // For the pink textbox with the girls, we can't get the color from
          // the ScrWork as it *acts* as a boolean.
          advNameTagLeftSprite = &SecondaryADVNameTag::LeftSprite;
          advNameTagPosition = &SecondaryADVNameTag::Position;
          advNameTagBaseLineWidth = &SecondaryADVNameTag::BaseLineWidth;
          advNameTagLineSprite = &SecondaryADVNameTag::LineSprite;
          advNameTagRightSprite = &SecondaryADVNameTag::RightSprite;
          break;
        case 0xFFFFFF:
          // On the first loaded script, it forces 0xffffff for the normal
          // textbox.
          advNameTagLeftSprite = &ADVNameTag::LeftSprite;
          advNameTagPosition = &ADVNameTag::Position;
          advNameTagBaseLineWidth = &ADVNameTag::BaseLineWidth;
          advNameTagLineSprite = &ADVNameTag::LineSprite;
          advNameTagRightSprite = &ADVNameTag::RightSprite;
          col = ScrWorkGetColor(SW_MESWINDOW_COLOR);
          break;
      }
      // On subsequent calls it's equal to 0 and *acts* as a boolean again.
      col.a = opacity;

      Renderer->DrawSprite(*advNameTagLeftSprite, *advNameTagPosition, col);
      // Name graphic additional length
      float lineWidth = nameWidth - *advNameTagBaseLineWidth;
      float lineX = advNameTagPosition->x + advNameTagLeftSprite->ScaledWidth();
      while (lineWidth > 0.0f) {
        Sprite lineSprite = *advNameTagLineSprite;
        lineSprite.SetScaledWidth(fminf(lineSprite.ScaledWidth(), lineWidth));
        Renderer->DrawSprite(lineSprite,
                             glm::vec2(lineX, advNameTagPosition->y), col);
        lineX += lineSprite.ScaledWidth();
        lineWidth -= lineSprite.ScaledWidth();
      }
      Renderer->DrawSprite(*advNameTagRightSprite,
                           glm::vec2(lineX, advNameTagPosition->y), col);
    }
  } 
  
  if (mode == DPM_NVL) {
    glm::vec4 nvlBoxTint(0.0f, 0.0f, 0.0f, opacity * NVLBoxMaxOpacity);
    Renderer->DrawRect(RectF(0, 0, Profile::DesignWidth, Profile::DesignHeight),
                       nvlBoxTint);
  }
}

}  // namespace CHLCC
}  // namespace Impacto