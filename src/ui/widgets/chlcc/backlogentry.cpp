#include "backlogentry.h"
#include "../../../renderer/renderer.h"
#include "../../../profile/dialogue.h"
#include "../../../profile/ui/backlogmenu.h"
#include "../../../profile/games/chlcc/backlogmenu.h"

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

void BacklogEntry::Render() {
  if (AudioId != -1) {
    Renderer->DrawSprite(
        VoiceIcon,
        glm::vec2(Profile::CHLCC::BacklogMenu::RenderingBounds.X,
                  Bounds.Y + VoiceIconOffset.y),
        Tint);
  }

  if (BacklogPage->HasName) {
    auto* beforeNametagLabel = new Label();
    auto* nametagLabel = new Label();
    auto* afterNametagLabel = new Label();

    beforeNametagLabel->SetText(
        NametagCommonStrings[0], Profile::Dialogue::REVNameFontSize,
        Profile::Dialogue::REVNameOutlineMode, Profile::Dialogue::REVNameColor);
    nametagLabel->SetText(BacklogPage->Name,
                          Profile::Dialogue::REVNameOutlineMode);
    afterNametagLabel->SetText(
        NametagCommonStrings[1], Profile::Dialogue::REVNameFontSize,
        Profile::Dialogue::REVNameOutlineMode, Profile::Dialogue::REVNameColor);
    beforeNametagLabel->MoveTo({Bounds.X, Bounds.Y});
    nametagLabel->MoveTo(beforeNametagLabel->Bounds.GetPos() +
                         glm::vec2(beforeNametagLabel->Bounds.Width, 0.0f));

    afterNametagLabel->MoveTo(nametagLabel->Bounds.GetPos() +
                              glm::vec2(nametagLabel->Bounds.Width, 0.0f));

    beforeNametagLabel->Render();
    nametagLabel->Render();
    afterNametagLabel->Render();
  }
  Renderer->DrawProcessedText(BacklogPage->Glyphs,
                              Profile::Dialogue::DialogueFont, Tint.a,
                              Profile::Dialogue::REVOutlineMode, true);
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto