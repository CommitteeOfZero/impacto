#include "../../impacto.h"
#include "../../scriptvars.h"
#include "../../mem.h"
#include "../../background2d.h"

namespace Impacto {
namespace Vm {
namespace Interface {

void UpdateBackground2D() {
  for (int i = 0; i < MaxBackgrounds2D; i++) {
    int bufId = ScrWork[SW_BG1SURF + i];
    Backgrounds2D[bufId].Layer = ScrWork[SW_BG1PRI + 40 * i];
    Backgrounds2D[bufId].Show = GetFlag(SF_BG1DISP + i);
    if (ScrWork[SW_BG1LX + 40 * i] != 0 && ScrWork[SW_BG1LY + 40 * i] != 0) {
      Backgrounds2D[bufId].BgSprite.Bounds =
          RectF(ScrWork[SW_BG1SX + 40 * i] + ScrWork[SW_BG1SX_OFS + 10 * i],
                ScrWork[SW_BG1SY + 40 * i] + ScrWork[SW_BG1SY_OFS + 10 * i],
                ScrWork[SW_BG1LX + 40 * i] + ScrWork[SW_BG1LX_OFS + 10 * i],
                ScrWork[SW_BG1LY + 40 * i] + ScrWork[SW_BG1LY_OFS + 10 * i]);
    } else {
      Backgrounds2D[bufId].BgSprite.Bounds =
          RectF(0.0f, 0.0f, Backgrounds2D[bufId].BgSprite.Sheet.DesignWidth,
                Backgrounds2D[bufId].BgSprite.Sheet.DesignHeight);
    }
  }
}

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto