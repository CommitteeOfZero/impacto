#include "../../impacto.h"
#include "../../profile/scriptvars.h"
#include "../../profile/vm.h"
#include "../../mem.h"
#include "../../background2d.h"
#include "../../character2d.h"
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

// Because yes
int GetBufferId(int bufIdByScript) {
  switch (bufIdByScript) {
    case 1:
    case 2:
      return bufIdByScript;
    case 4:
      return 3;
    case 8:
      return 4;
    case 16:
      return 5;
    case 32:
      return 6;
    case 64:
      return 7;
    case 128:
      return 8;
    case 256:
      return 9;
    case 512:
      return 10;
    case 1024:
      return 11;
    case 2048:
      return 12;
    case 4096:
      return 13;
    case 8192:
      return 14;
    case 16384:
      return 15;
    case 32768:
      return 16;
    default:
      return 0;
      break;
  }
}

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto