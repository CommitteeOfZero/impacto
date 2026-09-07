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

  Bounds = GetTextBounds(Page.Glyphs);
  for (const RubyChunk& chunk : Page.RubyChunks) {
    Bounds = RectF::Coalesce(Bounds, GetTextBounds(chunk.Text));
  }
  Position.x = Bounds.X;  // X position should not take name into account
  if (!Page.Name.empty()) {
    Bounds = RectF::Coalesce(Bounds, GetTextBounds(Page.Name));
  }
  Position.y = Bounds.Y;  // Y position should take name into account

  MoveToAnchor = {0.0f, Position.y};
  MoveTo(pos);
}

void BacklogEntry::UpdateInput(float dt) {
  using namespace Impacto::Vm::Interface;

  if (!Enabled) return;
  const RectF entryHoverBounds =
      RectF(Profile::BacklogMenu::HoverBounds.X, Bounds.Y,
            Profile::BacklogMenu::HoverBounds.Width, Bounds.Height);
  const bool useCursor = (Input::CurrentInputDevice == Input::Device::Mouse ||
                          Input::CurrentInputDevice == Input::Device::Touch);

  if (useCursor && Input::PrevMousePos != Input::CurMousePos) {
    Hovered = entryHoverBounds.ContainsPoint(Input::CurMousePos) &&
              Profile::BacklogMenu::HoverBounds.Y <= Bounds.Y &&
              (Bounds.Y + Bounds.Height) <=
                  (Profile::BacklogMenu::HoverBounds.Y +
                   Profile::BacklogMenu::HoverBounds.Height);
  }
  const bool isClicked =
      GetControlState(ControlType::OK, InputDownType::WentDown);

  if (HasFocus && isClicked &&
      (!useCursor || (Hovered && Bounds.ContainsPoint(Input::InitMousePos)))) {
    OnClickHandler(this);
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