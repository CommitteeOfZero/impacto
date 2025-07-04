#include "scene2d.h"
#include "../../profile/scriptvars.h"
#include "../../profile/vm.h"
#include "../../mem.h"
#include "../../profile/game.h"
// #include "../../window.h"
#include "../../renderer/renderer.h"
#include "../../voicetable.h"
#include "../../text.h"

namespace Impacto {
namespace Vm {
namespace Interface {

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::Vm;

void UpdateBackground2D() {
  for (int bgId = 0; bgId < Backgrounds.size(); bgId++) {
    int bufId = ScrWork[SW_BG1SURF + bgId];

    if (Profile::UseScreenCapEffects) {
      if (GetScriptBufferId(bgId) == ScrWork[SW_EFF_CAP_BUF]) {
        Backgrounds2D[bufId] = &Screencaptures[0];
      } else if (GetScriptBufferId(bgId) == ScrWork[SW_EFF_CAP_BUF2]) {
        Backgrounds2D[bufId] = &Screencaptures[1];
      } else if (Backgrounds2D[bufId]->BgSprite.Sheet.IsScreenCap) {
        Backgrounds2D[bufId] = &Backgrounds[bufId];
      }
    }

    Backgrounds2D[bufId]->UpdateState(bgId);
  }
}

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto