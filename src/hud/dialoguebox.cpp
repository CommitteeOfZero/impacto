#include "dialoguebox.h"

#include "../renderer2d.h"
#include "../profile/dialogue.h"
#include "../profile/game.h"
#include "../mem.h"
#include "../profile/scriptvars.h"

namespace Impacto {

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::Dialogue;

void DialogueBox::Show() {}

void DialogueBox::Hide() {}

void DialogueBox::Update(float dt) {}

void DialogueBox::Render(DialoguePageMode mode, bool hasName, float nameWidth,
                         uint32_t nameId, float opacity) {
  glm::vec4 col = ScrWorkGetColor(SW_MESWINDOW_COLOR);
  col.a = opacity;
  if (mode == DPM_ADV) {
    Renderer2D::DrawSprite(ADVBoxSprite, ADVBoxPos, col);
  } else {
    glm::vec4 nvlBoxTint(0.0f, 0.0f, 0.0f, opacity * NVLBoxMaxOpacity);
    Renderer2D::DrawRect(
        RectF(0, 0, Profile::DesignWidth, Profile::DesignHeight), nvlBoxTint);
  }

  if (mode == DPM_ADV && hasName) {
    if (HaveADVNameTag) {
      Renderer2D::DrawSprite(ADVNameTag::LeftSprite, ADVNameTag::Position, col);
    }

    if (HaveADVNameTag) {
      // Name graphic additional length
      float lineWidth = nameWidth - ADVNameTag::BaseLineWidth;
      float lineX =
          ADVNameTag::Position.x + ADVNameTag::LeftSprite.ScaledWidth();
      while (lineWidth > 0.0f) {
        Sprite lineSprite = ADVNameTag::LineSprite;
        lineSprite.SetScaledWidth(fminf(lineSprite.ScaledWidth(), lineWidth));
        Renderer2D::DrawSprite(lineSprite,
                               glm::vec2(lineX, ADVNameTag::Position.y), col);
        lineX += lineSprite.ScaledWidth();
        lineWidth -= lineSprite.ScaledWidth();
      }
      Renderer2D::DrawSprite(ADVNameTag::RightSprite,
                             glm::vec2(lineX, ADVNameTag::Position.y), col);
    }
  }
}

}  // namespace Impacto