#include "../../impacto.h"
#include "../../profile/scriptvars.h"
#include "../../mem.h"
#include "../../background2d.h"
#include "../../profile/game.h"

namespace Impacto {
namespace Vm {
namespace Interface {

using namespace Impacto::Profile::ScriptVars;

void UpdateBackground2D() {
  for (int i = 0; i < MaxBackgrounds2D; i++) {
    int bufId = ScrWork[SW_BG1SURF + i];
    Backgrounds2D[bufId].Layer = ScrWork[SW_BG1PRI + 40 * i];
    Backgrounds2D[bufId].Show = GetFlag(SF_BG1DISP + i);

    // ScrWork coordinates assume everything is 1280x720 regardless of design
    // resolution, that's why that's hardcoded here
    switch (ScrWork[SW_BG1DISPMODE + 40 * i]) {
      case 0: {
        Backgrounds2D[bufId].DisplayCoords = glm::vec2(
            -(ScrWork[SW_BG1POSX + 40 * i] * (Profile::DesignWidth / 1280.0f)),
            -(ScrWork[SW_BG1POSY + 40 * i] * (Profile::DesignHeight / 720.0f)));
        Backgrounds2D[bufId].BgSprite.BaseScale = glm::vec2(1.0f, 1.0f);
      } break;
      case 1: {
        Backgrounds2D[bufId].BgSprite.BaseScale = glm::vec2(
            1280.0f /
                (ScrWork[SW_BG1LX + 40 * i] + ScrWork[SW_BG1LX_OFS + 10 * i]),
            720.0f /
                (ScrWork[SW_BG1LY + 40 * i] + ScrWork[SW_BG1LY_OFS + 10 * i]));
        Backgrounds2D[bufId].DisplayCoords = glm::vec2(
            -((ScrWork[SW_BG1SX + 40 * i] + ScrWork[SW_BG1SX_OFS + 10 * i]) *
              (Profile::DesignWidth / 1280.0f)) *
                Backgrounds2D[bufId].BgSprite.BaseScale.x,
            -((ScrWork[SW_BG1SY + 40 * i] + ScrWork[SW_BG1SY_OFS + 10 * i]) *
              (Profile::DesignHeight / 720.0f)) *
                Backgrounds2D[bufId].BgSprite.BaseScale.y);
      } break;
      case 2: {
        float scale =
            (ScrWork[SW_BG1SIZE + 40 * i] + ScrWork[SW_BG1SIZE_OFS + 10 * i]) /
            1000.0f;
        Backgrounds2D[bufId].BgSprite.BaseScale = glm::vec2(scale, scale);
        Backgrounds2D[bufId].DisplayCoords = glm::vec2(
            ScrWork[SW_BG1POSX + 40 * i] * (Profile::DesignWidth / 1280.0f) -
                (Backgrounds2D[bufId].BgSprite.ScaledWidth() / 2.0f),
            ScrWork[SW_BG1POSY + 40 * i] * (Profile::DesignHeight / 720.0f) -
                (Backgrounds2D[bufId].BgSprite.ScaledHeight() / 2.0f));
      } break;
    }
  }
}

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto