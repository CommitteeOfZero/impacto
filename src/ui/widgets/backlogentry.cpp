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

BacklogEntry::BacklogEntry(Vm::BufferOffsetContext scrCtx,
                           std::optional<int> audioId, int characterId,
                           glm::vec2 pos)
    : AudioId(audioId), CharacterId(characterId), Position(pos) {
  Enabled = true;

  Impacto::Vm::Sc3VmThread dummy;
  dummy.IpOffset = scrCtx.IpOffset;
  dummy.ScriptBufferId = scrCtx.ScriptBufferId;
  Page.AddString(&dummy);

  Bounds = !Page.Glyphs.empty() ? Page.Glyphs.begin()->DestRect
                                : RectF(pos.x, pos.y, 0, 0);
  for (const ProcessedTextGlyph& glyph : Page.Glyphs) {
    Bounds = RectF::Coalesce(Bounds, glyph.DestRect);
  }
  for (const RubyChunk& chunk : Page.RubyChunks) {
    for (const auto& glyph : chunk.Text) {
      Bounds = RectF::Coalesce(Bounds, glyph.DestRect);
    }
  }
  Position.x = Bounds.X;  // X position should not take name into account
  for (const ProcessedTextGlyph& glyph : Page.Name) {
    Bounds = RectF::Coalesce(Bounds, glyph.DestRect);
  }
  Position.y = Bounds.Y;  // Y position should take name into account

  MoveToAnchor = {0.0f, Position.y};
  MoveTo(pos);
}

void BacklogEntry::UpdateInput(float dt) {
  if (Enabled) {
    RectF entryHoverBounds =
        RectF(Profile::BacklogMenu::HoverBounds.X, Bounds.Y,
              Profile::BacklogMenu::HoverBounds.Width, Bounds.Height);
    if (Input::CurrentInputDevice == Input::Device::Mouse) {
      Hovered = entryHoverBounds.ContainsPoint(Input::CurMousePos) &&
                Profile::BacklogMenu::HoverBounds.Y <= Bounds.Y &&
                (Bounds.Y + Bounds.Height) <=
                    (Profile::BacklogMenu::HoverBounds.Y +
                     Profile::BacklogMenu::HoverBounds.Height);
    } else if (Input::CurrentInputDevice == Input::Device::Touch &&
               Input::TouchIsDown[0]) {
      Hovered = entryHoverBounds.ContainsPoint(Input::CurTouchPos) &&
                Profile::BacklogMenu::HoverBounds.Y <= Bounds.Y &&
                (Bounds.Y + Bounds.Height) <=
                    (Profile::BacklogMenu::HoverBounds.Y +
                     Profile::BacklogMenu::HoverBounds.Height);
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
  Page.Move(relativePosition);
}

void BacklogEntry::Render() {
  if (AudioId.has_value()) {
    Renderer->DrawSprite(
        VoiceIcon,
        glm::vec2(Bounds.X - VoiceIcon.ScaledWidth(), Bounds.Y) +
            VoiceIconOffset,
        Tint);
  }

  Page.Render(Tint.a, Profile::Dialogue::REVNameOutlineMode);
}

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto