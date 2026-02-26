#include "nametagdisplay.h"

#include "../../profile/games/chlcc/dialoguebox.h"
#include "../../profile/dialogue.h"
#include "../../profile/scriptvars.h"
#include "../../renderer/renderer.h"

namespace Impacto {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::DialogueBox;
using namespace Impacto::Profile::Dialogue;
using namespace Impacto::Profile::ScriptVars;

void NametagDisplay::Render(const NameInfo& nameInfo, glm::vec4 tint) {
  if (!nameInfo.RenderWindow) return;

  Renderer->DrawSprite(
      ScrWork[SW_MESWINDOW_COLOR] == 1 ? SecondaryNametagSprite : NametagSprite,
      NametagPosition, tint);

  Renderer->DrawProcessedText(nameInfo.Name, DialogueFont, tint.a,
                              RendererOutlineMode::Full);
}

}  // namespace CHLCC
}  // namespace Impacto
