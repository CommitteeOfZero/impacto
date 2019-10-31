#include "scene2d.h"
#include "../../profile/scriptvars.h"
#include "../../profile/vm.h"
#include "../../mem.h"
#include "../../profile/game.h"

namespace Impacto {
namespace Vm {
namespace Interface {

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::Vm;

void UpdateBackground2D() {
  for (int i = 0; i < MaxBackgrounds2D; i++) {
    int bufId = ScrWork[SW_BG1SURF + i];

    Backgrounds2D[bufId].Layer = ScrWork[SW_BG1PRI + ScrWorkBgStructSize * i];
    Backgrounds2D[bufId].Show = GetFlag(SF_BG1DISP + i);

    // HACK: In this case it's supposed to show a screen cap from surfaceIds
    // 200/201, but...
    if ((GetScriptBufferId(i) == ScrWork[SW_EFF_CAP_BUF]) ||
        (GetScriptBufferId(i) == ScrWork[SW_EFF_CAP_BUF2])) {
      Backgrounds2D[bufId].Show = false;
      continue;
    }

    // ScrWork coordinates assume everything is 1280x720 regardless of design
    // resolution, that's why that's hardcoded here
    switch (ScrWork[SW_BG1DISPMODE + ScrWorkBgStructSize * i]) {
      case 0: {
        Backgrounds2D[bufId].DisplayCoords =
            glm::vec2(-(ScrWork[SW_BG1POSX + ScrWorkBgStructSize * i] *
                        (Profile::DesignWidth / 1280.0f)),
                      -(ScrWork[SW_BG1POSY + ScrWorkBgStructSize * i] *
                        (Profile::DesignHeight / 720.0f)));
        Backgrounds2D[bufId].BgSprite.BaseScale = glm::vec2(1.0f, 1.0f);
      } break;
      case 1: {
        Backgrounds2D[bufId].BgSprite.BaseScale =
            glm::vec2(1280.0f / (ScrWork[SW_BG1LX + ScrWorkBgStructSize * i] +
                                 ScrWork[SW_BG1LX_OFS + 10 * i]),
                      720.0f / (ScrWork[SW_BG1LY + ScrWorkBgStructSize * i] +
                                ScrWork[SW_BG1LY_OFS + 10 * i]));
        Backgrounds2D[bufId].DisplayCoords =
            glm::vec2(-((ScrWork[SW_BG1SX + ScrWorkBgStructSize * i] +
                         ScrWork[SW_BG1SX_OFS + 10 * i]) *
                        (Profile::DesignWidth / 1280.0f)) *
                          Backgrounds2D[bufId].BgSprite.BaseScale.x,
                      -((ScrWork[SW_BG1SY + ScrWorkBgStructSize * i] +
                         ScrWork[SW_BG1SY_OFS + 10 * i]) *
                        (Profile::DesignHeight / 720.0f)) *
                          Backgrounds2D[bufId].BgSprite.BaseScale.y);
      } break;
      case 2: {
        float scale = (ScrWork[SW_BG1SIZE + ScrWorkBgStructSize * i] +
                       ScrWork[SW_BG1SIZE_OFS + 10 * i]) /
                      1000.0f;
        Backgrounds2D[bufId].BgSprite.BaseScale = glm::vec2(scale, scale);
        Backgrounds2D[bufId].DisplayCoords = glm::vec2(
            ScrWork[SW_BG1POSX + ScrWorkBgStructSize * i] *
                    (Profile::DesignWidth / 1280.0f) -
                (Backgrounds2D[bufId].BgSprite.ScaledWidth() / 2.0f),
            ScrWork[SW_BG1POSY + ScrWorkBgStructSize * i] *
                    (Profile::DesignHeight / 720.0f) -
                (Backgrounds2D[bufId].BgSprite.ScaledHeight() / 2.0f));
      } break;
    }
  }
}

void UpdateCharacter2D() {
  for (int i = 0; i < MaxCharacters2D; i++) {
    int bufId = ScrWork[SW_CHA1SURF + i];
    Characters2D[bufId].Layer = ScrWork[SW_CHA1PRI + ScrWorkChaStructSize * i];
    Characters2D[bufId].Show = GetFlag(SF_CHA1DISP + i);
    Characters2D[bufId].CharaOffsetX =
        ScrWork[SW_CHA1POSX + ScrWorkChaStructSize * i];
    Characters2D[bufId].CharaOffsetY =
        ScrWork[SW_CHA1POSY + ScrWorkChaStructSize * i];
  }
}

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto