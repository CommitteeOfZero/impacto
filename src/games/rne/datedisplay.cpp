#include "datedisplay.h"

#include "../../profile/scriptvars.h"
#include "../../mem.h"
#include "../../renderer/renderer.h"
#include "../../profile/hud/datedisplay.h"

#include <ctime>

namespace Impacto {
namespace RNE {

using namespace Impacto::Profile::DateDisplay;
using namespace Impacto::Profile::ScriptVars;

void DateDisplay::Update(float dt) {
  FadeAnimation.Update(dt);
  if (FadeAnimation.IsOut()) {
    if (ScrWork[LR_DATE] != 0 && GetFlag(SF_DATEDISPLAY)) {
      FadeAnimation.StartIn();
      Year = (ScrWork[LR_DATE] / 10000);
      Month = (ScrWork[LR_DATE] - 10000 * Year) / 100;
      Day = (ScrWork[LR_DATE] - 10000 * Year) % 100;
      Week = CurrentDateTime().tm_wday;
    }
  } else if (FadeAnimation.IsIn() && !GetFlag(SF_DATEDISPLAY)) {
    FadeAnimation.StartOut();
  }
}

void DateDisplay::Render() {
  if (FadeAnimation.IsOut()) return;
  if (FadeAnimation.Progress > 0.0f) {
    float smoothedFade = glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress);

    glm::vec4 col(1.0f);
    col.a = smoothedFade;

    Renderer->DrawSprite(
        BackgroundSprite,
        glm::mix(BackgroundStartPos, BackgroundEndPos, smoothedFade), col);

    glm::vec2 pos(DateStartX, YearWeekY);
    Renderer->DrawSprite(CloseBracketSprite, pos, col);

    pos.x -= WeekSprites[Week].ScaledWidth() + Spacing;
    Renderer->DrawSprite(WeekSprites[Week], pos, col);

    pos.x -= OpenBracketSprite.ScaledWidth() + Spacing;
    Renderer->DrawSprite(OpenBracketSprite, pos, col);

    pos.x -= YearNumSprites[Year % 10].ScaledWidth();
    Renderer->DrawSprite(YearNumSprites[Year % 10], pos, col);
    pos.x -= YearNumSprites[Year / 10].ScaledWidth();
    Renderer->DrawSprite(YearNumSprites[Year / 10], pos, col);
    pos.x -= YearNumSprites[0].ScaledWidth();
    Renderer->DrawSprite(YearNumSprites[0], pos, col);
    pos.x -= YearNumSprites[2].ScaledWidth();
    Renderer->DrawSprite(YearNumSprites[2], pos, col);

    pos.x -= DYSeparatorSprite.ScaledWidth();
    Renderer->DrawSprite(DYSeparatorSprite, pos, col);

    pos.y = MonthDayY;
    pos.x -= DayNumSprites[Day % 10].ScaledWidth() + Spacing;
    Renderer->DrawSprite(DayNumSprites[Day % 10], pos, col);
    pos.x -= DayNumSprites[Day / 10].ScaledWidth();
    Renderer->DrawSprite(DayNumSprites[Day / 10], pos, col);

    pos.x -= MDSeparatorSprite.ScaledWidth();
    Renderer->DrawSprite(MDSeparatorSprite, pos, col);

    pos.x -= MonthNumSprites[Month % 10].ScaledWidth() + Spacing;
    Renderer->DrawSprite(MonthNumSprites[Month % 10], pos, col);
    if (Month / 10 != 0) {
      pos.x -= MonthNumSprites[Month / 10].ScaledWidth();
      Renderer->DrawSprite(MonthNumSprites[Month / 10], pos, col);
    }
  }
}

}  // namespace RNE
}  // namespace Impacto