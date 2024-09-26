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

BacklogEntry::BacklogEntry(int id, uint8_t* str, int audioId, glm::vec2 pos,
                           const RectF& hoverBounds)
    : Widgets::BacklogEntry(id, str, audioId, pos, hoverBounds) {}

void BacklogEntry::Render() {
  if (AudioId != -1) {
    // TODO: Use transparency mask
    Renderer->DrawSprite(
        VoiceIcon,
        glm::vec2(Bounds.X - VoiceIcon.ScaledWidth(), Bounds.Y) +
            VoiceIconOffset,
        Tint);
  }

  if (BacklogPage->HasName) {
    Renderer->DrawProcessedText(
        BacklogPage->Name, Profile::Dialogue::DialogueFont, Tint.a,
        Profile::Dialogue::REVNameOutlineMode, true, &BacklogMaskSheet);
  }

  Renderer->DrawProcessedText(
      BacklogPage->Glyphs, Profile::Dialogue::DialogueFont, Tint.a,
      Profile::Dialogue::REVOutlineMode, true, &BacklogMaskSheet);
}

}  // namespace CC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto