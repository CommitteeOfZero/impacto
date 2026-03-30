#include "backlogentry.h"
#include "../../../renderer/renderer.h"
#include "../../../profile/dialogue.h"
#include "../../../profile/ui/backlogmenu.h"
#include "../../../profile/games/cc/backlogmenu.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CC {

using namespace Impacto::Profile::BacklogMenu;
using namespace Impacto::Profile::CC::BacklogMenu;

void BacklogEntry::Render() {
  if (AudioId.has_value()) {
    RectF bounds = RectF(Bounds.X - VoiceIcon.ScaledWidth() + VoiceIconOffset.x,
                         Bounds.Y + VoiceIconOffset.y, VoiceIcon.ScaledWidth(),
                         VoiceIcon.ScaledHeight());
    Sprite mask;
    mask.Sheet = BacklogMaskSheet;
    mask.Bounds = bounds;

    Renderer->DrawMaskedSpriteOverlay(VoiceIcon, mask, bounds,
                                      (int)(Tint.a * 255), 256, glm::mat4(1.0f),
                                      Tint, false, false);
  }

  Renderer->DrawProcessedText(Page->Name, Profile::Dialogue::DialogueFont,
                              Tint.a, Profile::Dialogue::REVNameOutlineMode,
                              true, &BacklogMaskSheet);

  for (RubyChunk& chunk : Page->RubyChunks) {
    Renderer->DrawProcessedText(chunk.Text, Profile::Dialogue::DialogueFont,
                                Tint.a, Profile::Dialogue::REVNameOutlineMode,
                                true, &BacklogMaskSheet);
  }

  Renderer->DrawProcessedText(Page->Glyphs, Profile::Dialogue::DialogueFont,
                              Tint.a, Profile::Dialogue::REVOutlineMode, true,
                              &BacklogMaskSheet);
}

}  // namespace CC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto