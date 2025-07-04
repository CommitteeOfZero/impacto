#include "dialoguebox.h"

#include "../renderer/renderer.h"
#include "../profile/dialogue.h"
#include "../profile/game.h"
#include "../mem.h"
#include "../profile/scriptvars.h"
#include "../profile/vm.h"
#include "../vm/vm.h"
#include "../character2d.h"

namespace Impacto {

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::Dialogue;

void DialogueBox::Init() {
  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);

  if (HasAutoButton) {
    UI::Widgets::Button* autoButton = new UI::Widgets::Button(
        0, AutoButtonSprite, AutoButtonSprite, nullSprite, AutoButtonPosition);
    ControlButtons.push_back(autoButton);
  }

  if (HasSkipButton) {
    UI::Widgets::Button* skipButton = new UI::Widgets::Button(
        0, SkipButtonSprite, SkipButtonSprite, nullSprite, SkipButtonPosition);
    ControlButtons.push_back(skipButton);
  }

  if (HasBacklogButton) {
    UI::Widgets::Button* backlogButton =
        new UI::Widgets::Button(0, BacklogButtonSprite, BacklogButtonSprite,
                                nullSprite, BacklogButtonPosition);
    ControlButtons.push_back(backlogButton);
  }

  if (HasMenuButton) {
    UI::Widgets::Button* menuButton = new UI::Widgets::Button(
        0, MenuButtonSprite, MenuButtonSprite, nullSprite, MenuButtonPosition);
    ControlButtons.push_back(menuButton);
  }
}

void DialogueBox::Show() {}

void DialogueBox::Hide() {}

void DialogueBox::Update(float dt) { UpdateControlButtons(dt); }

void DialogueBox::Render(DialoguePageMode mode, bool hasName, float nameWidth,
                         uint32_t nameId, float opacity) {
  glm::vec4 col = ScrWorkGetColor(SW_MESWINDOW_COLOR);
  col.a = opacity;
  if (mode == DPM_ADV) {
    Renderer->DrawSprite(ADVBoxSprite, ADVBoxPos, col);
    RenderControlButtons(col);
  } else {
    glm::vec4 nvlBoxTint(0.0f, 0.0f, 0.0f, opacity * NVLBoxMaxOpacity);
    Renderer->DrawQuad(RectF(0, 0, Profile::DesignWidth, Profile::DesignHeight),
                       nvlBoxTint);
  }

  if (mode == DPM_ADV && hasName) {
    if (HasSpeakerPortraits) {
      // Draw Face
      for (int i = 0; i < SpeakerPortraits.size(); i++) {
        int bufId = ScrWork[SW_FACE1SURF + i];
        SpeakerPortraits[bufId].Tint = col;
        SpeakerPortraits[bufId].Position +=
            glm::vec2(SpeakerPortraitBaseOffsetX, SpeakerPortraitBaseOffsetY);
        SpeakerPortraits[bufId].Render(-1);
      }
    }

    if (HaveADVNameTag) {
      Renderer->DrawSprite(ADVNameTag::LeftSprite, ADVNameTag::Position, col);

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

void DialogueBox::UpdateControlButtons(float dt) {
  for (auto button : ControlButtons) {
    button->UpdateInput();
    button->Update(dt);
  }
}

void DialogueBox::RenderControlButtons(glm::vec4 col) {
  for (auto button : ControlButtons) {
    button->Tint = col;
    button->Render();
  }
}

}  // namespace Impacto