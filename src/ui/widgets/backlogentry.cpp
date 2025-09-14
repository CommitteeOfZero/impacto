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
using namespace Impacto::Profile::ScriptVars;

BacklogEntry::BacklogEntry(int id, Vm::BufferOffsetContext scrCtx, int audioId,
                           int characterId, glm::vec2 pos,
                           const RectF& hoverBounds)
    : Id(id),
      AudioId(audioId),
      CharacterId(characterId),
      Position(pos),
      HoverBounds(hoverBounds) {
  Enabled = true;

  BacklogPage = new DialoguePage();
  BacklogPage->Glyphs.reserve(512);
  BacklogPage->Clear();
  BacklogPage->Mode = DPM_REV;

  Impacto::Vm::Sc3VmThread dummy;
  dummy.IpOffset = scrCtx.IpOffset;
  dummy.ScriptBufferId = scrCtx.ScriptBufferId;
  // CHLCC uses DPM_REV for the Erin DialogueBox
  if (ScrWork[SW_MESWIN0TYPE] == 0) {
    REVBounds.X = pos.x;
    REVBounds.Y = pos.y;
  }
  BacklogPage->AddString(&dummy);

  TextLength = BacklogPage->Length;

  Bounds = !BacklogPage->Glyphs.empty() ? BacklogPage->Glyphs.begin()->DestRect
                                        : RectF(pos.x, pos.y, 0, 0);
  for (const ProcessedTextGlyph& glyph : BacklogPage->Glyphs) {
    Bounds = RectF::Coalesce(Bounds, glyph.DestRect);
  }
  Position.x = Bounds.X;  // X position should not take name into account
  for (const ProcessedTextGlyph& glyph : BacklogPage->Name) {
    Bounds = RectF::Coalesce(Bounds, glyph.DestRect);
  }
  TextHeight = Bounds.Height;
  Position.y = Bounds.Y;  // Y position should

  switch (BacklogPage->Alignment) {
    default:
    case TextAlignment::Left:
      break;
    case TextAlignment::Center: {
      const RectF& revBounds = REVBounds;
      pos.x = revBounds.X + (revBounds.Width - BacklogPage->Dimensions.x) / 2;
      break;
    }
  }
  MoveTo(pos);
}

BacklogEntry::~BacklogEntry() { delete BacklogPage; }

void BacklogEntry::UpdateInput(float dt) {
  if (Enabled) {
    RectF entryHoverBounds =
        RectF(HoverBounds.X, Bounds.Y, HoverBounds.Width, Bounds.Height);
    if (Input::CurrentInputDevice == Input::Device::Mouse &&
        Input::PrevMousePos != Input::CurMousePos) {
      Hovered =
          entryHoverBounds.ContainsPoint(Input::CurMousePos) &&
          HoverBounds.Y <= Bounds.Y &&
          (Bounds.Y + Bounds.Height) <= (HoverBounds.Y + HoverBounds.Height);
    } else if (Input::CurrentInputDevice == Input::Device::Touch &&
               Input::TouchIsDown[0] &&
               Input::PrevMousePos != Input::CurTouchPos) {
      Hovered =
          entryHoverBounds.ContainsPoint(Input::CurTouchPos) &&
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
  BacklogPage->Move(relativePosition);
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