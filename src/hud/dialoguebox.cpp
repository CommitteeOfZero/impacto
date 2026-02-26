#include "dialoguebox.h"

#include "mo6tw/dialoguebox.h"
#include "chlcc/dialoguebox.h"
#include "cc/dialoguebox.h"

#include "../text/dialoguepage.h"
#include "../renderer/renderer.h"
#include "../profile/dialogue.h"
#include "../profile/game.h"
#include "../mem.h"
#include "../log.h"
#include "../profile/scriptvars.h"
#include "../profile/vm.h"
#include "../vm/vm.h"
#include "../character2d.h"

namespace Impacto {

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::Dialogue;

std::unique_ptr<DialogueBox> DialogueBox::Create() {
  switch (DialogueBoxCurrentType) {
    default:
      ImpLog(LogLevel::Warning, LogChannel::General,
             "Attempting to create unexpected DialogueBox type {:d}; "
             "defaulting to Void",
             static_cast<int>(DialogueBoxCurrentType));
      [[fallthrough]];
    case DialogueBoxType::None:
      return std::make_unique<VoidDialogueBox>();

    case DialogueBoxType::MO6TW:
      return std::make_unique<MO6TW::DialogueBox>();

    case DialogueBoxType::CHLCC:
      return std::make_unique<CHLCC::DialogueBox>();

    case DialogueBoxType::CC:
      return std::make_unique<CC::DialogueBox>();

    case DialogueBoxType::Plain:
      return std::make_unique<PlainDialogueBox>();
  }
}

PlainDialogueBox::PlainDialogueBox() {
  using namespace UI::Widgets;

  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);

  if (HasAutoButton) {
    ControlButtons.emplace_back(std::make_unique<Button>(
        0, AutoButtonSprite, AutoButtonSprite, nullSprite, AutoButtonPosition));
  }

  if (HasSkipButton) {
    ControlButtons.emplace_back(std::make_unique<Button>(
        0, SkipButtonSprite, SkipButtonSprite, nullSprite, SkipButtonPosition));
  }

  if (HasBacklogButton) {
    ControlButtons.emplace_back(
        std::make_unique<Button>(0, BacklogButtonSprite, BacklogButtonSprite,
                                 nullSprite, BacklogButtonPosition));
  }

  if (HasMenuButton) {
    ControlButtons.emplace_back(std::make_unique<Button>(
        0, MenuButtonSprite, MenuButtonSprite, nullSprite, MenuButtonPosition));
  }
}

void PlainDialogueBox::Show() {
  DialogueBox::Show();
  VisibilityState = Shown;
}

void PlainDialogueBox::Hide() {
  DialogueBox::Hide();
  VisibilityState = Hidden;
}

void PlainDialogueBox::UpdateInput(float dt) {
  DialogueBox::UpdateInput(dt);
  UpdateControlButtons(dt);
}

void PlainDialogueBox::Render(const DialoguePageMode mode,
                              const NameInfo& nameInfo, glm::vec4 tint) {
  tint = glm::vec4(glm::vec3(ScrWorkGetColor(SW_MESWINDOW_COLOR)), tint.a);

  if (mode == DPM_ADV) {
    Renderer->DrawSprite(ADVBoxSprite, ADVBoxPos, tint);
    RenderControlButtons(tint);

    if (!nameInfo.Name.empty()) {
      if (HasSpeakerPortraits) {
        // Draw Face
        for (int i = 0; i < std::ssize(SpeakerPortraits); i++) {
          int bufId = ScrWork[SW_FACE1SURF + i];
          SpeakerPortraits[bufId].UpdateState(i);

          SpeakerPortraits[bufId].Tint = tint;
          SpeakerPortraits[bufId].Position +=
              glm::vec2(SpeakerPortraitBaseOffsetX, SpeakerPortraitBaseOffsetY);

          SpeakerPortraits[bufId].Render(-1);
        }
      }
    }

    NametagDisplayInst->Render(nameInfo, tint);

  } else {
    assert(mode == DPM_REV);

    glm::vec4 nvlBoxTint(0.0f, 0.0f, 0.0f, tint.a * NVLBoxMaxOpacity);
    Renderer->DrawQuad(RectF(0, 0, Profile::DesignWidth, Profile::DesignHeight),
                       nvlBoxTint);
  }
}

void PlainDialogueBox::UpdateControlButtons(float dt) {
  for (auto& button : ControlButtons) {
    button->UpdateInput(dt);
    button->Update(dt);
  }
}

void PlainDialogueBox::RenderControlButtons(glm::vec4 col) {
  for (auto& button : ControlButtons) {
    button->Tint = col;
    button->Render();
  }
}

void VoidDialogueBox::Show() {
  DialogueBox::Show();
  VisibilityState = Shown;
}

void VoidDialogueBox::Hide() {
  DialogueBox::Hide();
  VisibilityState = Hidden;
}

}  // namespace Impacto