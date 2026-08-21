#include "backlogentry.h"
#include "../../../renderer/renderer.h"
#include "../../../profile/dialogue.h"
#include "../../../profile/ui/backlogmenu.h"
#include "../../../profile/games/chlcc/backlogmenu.h"
#include "../backlogentry.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

using namespace Impacto::Profile::BacklogMenu;
using namespace Impacto::Profile::CHLCC::BacklogMenu;

constexpr std::array<std::string_view, 2> NametagCommonStrings = {
    "【　",
    "　】",
};

BacklogEntry::BacklogEntry(Vm::BufferOffsetContext scrCtx,
                           std::optional<int> audioId, int characterId,
                           glm::vec2 pos)
    : Widgets::BacklogEntry(scrCtx, audioId, characterId, pos) {
  if (!Page.Name.empty()) {
    const float nameFontSize =
        TextModesInfo[Profile::Dialogue::REVMessageModeIdx].NameGlyphSize.y;
    BeforeNametagLabel.SetText(
        NametagCommonStrings[0], {0.0f, 0.0f}, nameFontSize,
        Profile::Dialogue::REVNameOutlineMode, Profile::Dialogue::REVNameColor);
    NametagLabel.SetText(Page.Name, {0.0f, 0.0f},
                         Profile::Dialogue::REVNameOutlineMode);
    AfterNametagLabel.SetText(
        NametagCommonStrings[1], {0.0f, 0.0f}, nameFontSize,
        Profile::Dialogue::REVNameOutlineMode, Profile::Dialogue::REVNameColor);
  }
}

void BacklogEntry::Render() {
  if (AudioId.has_value()) {
    Renderer->DrawSprite(
        VoiceIcon, glm::vec2(RenderingBounds.X, Bounds.Y + VoiceIconOffset.y),
        Tint);
  }

  if (!Page.Name.empty()) {
    BeforeNametagLabel.MoveTo({Bounds.X, Bounds.Y});
    NametagLabel.MoveTo(BeforeNametagLabel.Bounds.TopRight());
    AfterNametagLabel.MoveTo(NametagLabel.Bounds.TopRight());

    BeforeNametagLabel.Render();
    NametagLabel.Render();
    AfterNametagLabel.Render();
  }

  Profile::Dialogue::DialogueFont->DrawProcessedText(
      Page.Glyphs, Tint.a, Profile::Dialogue::REVOutlineMode);
  for (RubyChunk& chunk : Page.RubyChunks) {
    Profile::Dialogue::DialogueFont->DrawProcessedText(
        chunk.Text, Tint.a, Profile::Dialogue::REVOutlineMode);
  }
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto