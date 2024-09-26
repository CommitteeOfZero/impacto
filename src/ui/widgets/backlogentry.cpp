#include "backlogentry.h"
#include "../../renderer/renderer.h"
#include "../../inputsystem.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../vm/interface/input.h"
#include "../../vm/thread.h"
#include "../../profile/dialogue.h"
#include "../../profile/ui/backlogmenu.h"

namespace Impacto {
namespace UI {
namespace Widgets {

using namespace Impacto::Profile::BacklogMenu;

BacklogEntry::BacklogEntry(int id, uint8_t* str, int audioId, glm::vec2 pos,
                           const RectF& hoverBounds)
    : Id(id), AudioId(audioId), Position(pos), HoverBounds(hoverBounds) {
  Enabled = true;

  BacklogPage = new DialoguePage();
  BacklogPage->Glyphs.reserve(512);
  BacklogPage->Clear();
  BacklogPage->Mode = DPM_REV;

  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  // CHLCC uses DPM_REV for the Erin DialogueBox
  if (Profile::Dialogue::DialogueBoxCurrentType != +DialogueBoxType::CHLCC) {
    Profile::Dialogue::REVBounds.X = pos.x;
    Profile::Dialogue::REVBounds.Y = pos.y;
  }
  BacklogPage->AddString(&dummy);

  TextLength = BacklogPage->Length;

  Bounds = !BacklogPage->Glyphs.empty() ? BacklogPage->Glyphs.begin()->DestRect
                                        : RectF(pos.x, pos.y, 0, 0);
  for (const ProcessedTextGlyph& glyph : BacklogPage->Glyphs) {
    Bounds = RectF::Coalesce(Bounds, glyph.DestRect);
  }
  Position = glm::vec2(Bounds.X, Bounds.Y);
  for (const ProcessedTextGlyph& glyph : BacklogPage->Name) {
    Bounds = RectF::Coalesce(Bounds, glyph.DestRect);
  }
  TextHeight = Bounds.Height;

  switch (BacklogPage->Alignment) {
    default:
    case TextAlignment::Left:
      break;
    case TextAlignment::Center: {
      const RectF& revBounds = Profile::Dialogue::REVBounds;
      pos.x = revBounds.X + (revBounds.Width - BacklogPage->Dimensions.x) / 2;
      break;
    }
  }
  MoveTo(pos);
}

BacklogEntry::~BacklogEntry() { delete BacklogPage; }

void BacklogEntry::UpdateInput() {
  if (Enabled) {
    if (Input::PrevMousePos != Input::CurMousePos) {
      RectF entryHoverBounds =
          RectF(HoverBounds.X, Bounds.Y, HoverBounds.Width, Bounds.Height);

      Hovered =
          entryHoverBounds.ContainsPoint(Input::CurMousePos) &&
          HoverBounds.Y <= Bounds.Y &&
          (Bounds.Y + Bounds.Height) <= (HoverBounds.Y + HoverBounds.Height);
    }
    if (HasFocus &&
        ((Hovered &&
          Vm::Interface::PADinputMouseWentDown & Vm::Interface::PAD1A) ||
         (Vm::Interface::PADinputButtonWentDown & Vm::Interface::PAD1A))) {
      OnClickHandler(this);
    }
  }
}

void BacklogEntry::Move(glm::vec2 relativePosition) {
  Position += relativePosition;
  Widget::Move(relativePosition);
  ProcessedTextGlyph::Move(BacklogPage->Glyphs, relativePosition);
  if (BacklogPage->HasName)
    ProcessedTextGlyph::Move(BacklogPage->Name, relativePosition);
}

void BacklogEntry::MoveTo(glm::vec2 position) {
  glm::vec2 relativePosition = position - Position;
  Move(relativePosition);
}

void BacklogEntry::Render() {
  if (AudioId != -1) {
    Renderer->DrawSprite(
        VoiceIcon,
        glm::vec2(Bounds.X - VoiceIcon.ScaledWidth(), Bounds.Y) +
            VoiceIconOffset,
        Tint);
  }

  if (BacklogPage->HasName) {
    Renderer->DrawProcessedText(BacklogPage->Name,
                                Profile::Dialogue::DialogueFont, Tint.a,
                                Profile::Dialogue::REVNameOutlineMode);
  }

  Renderer->DrawProcessedText(BacklogPage->Glyphs,
                              Profile::Dialogue::DialogueFont, Tint.a,
                              Profile::Dialogue::REVOutlineMode);
}

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto