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
    const glm::vec2 textPos = Page.Name.empty()
                                  ? Page.Glyphs[0].DestRect.GetPos()
                                  : Page.Name[0].DestRect.GetPos();
    const glm::vec2 voiceIconPos =
        textPos - glm::vec2(VoiceIcon.ScaledWidth(), 0.0f) + VoiceIconOffset;

    Renderer->DrawMaskedSpriteNoAlpha(
        {VoiceIcon, BacklogMaskSheet, voiceIconPos, Tint},
        {.Alpha = (int)(Tint.a * 255), .FadeRange = 256, .IsInverted = false});
  }

  Profile::Dialogue::DialogueFont->DrawProcessedText(
      Page.Name, Tint.a, Profile::Dialogue::REVNameOutlineMode,
      &BacklogMaskSheet);

  for (RubyChunk& chunk : Page.RubyChunks) {
    Profile::Dialogue::DialogueFont->DrawProcessedText(
        chunk.Text, Tint.a, Profile::Dialogue::REVOutlineMode,
        &BacklogMaskSheet);
  }

  Profile::Dialogue::DialogueFont->DrawProcessedText(
      Page.Glyphs, Tint.a, Profile::Dialogue::REVOutlineMode,
      &BacklogMaskSheet);
}

}  // namespace CC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto