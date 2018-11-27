#include "datedisplay.h"

#include "../scriptvars.h"
#include "../impacto.h"
#include "../mem.h"
#include <ctime>
#include "../renderer2d.h"
#include "../game.h"
#include "../profile/hud/datedisplay.h"

namespace Impacto {
namespace DateDisplay {

using namespace Impacto::Profile::DateDisplay;

enum DateAnimState { Hidden, Hiding, Showing, Shown };

static float Fade = 0.0f;

static DateAnimState AnimState = Hidden;

static int Day = 0;
static int Month = 0;
static int Year = 0;
static int Week = 0;

void Update(float dt) {
  if (AnimState == Hidden) {
    if (ScrWork[LR_DATE] != 0 && GetFlag(SF_DATEDISPLAY)) {
      AnimState = Showing;
      Year = (ScrWork[LR_DATE] / 10000);
      Month = (ScrWork[LR_DATE] - 10000 * Year) / 100;
      Day = (ScrWork[LR_DATE] - 10000 * Year) % 100;
      std::tm time_in = {0, 0, 0, Day, Month - 1, Year + 100};
      std::time_t time_temp = std::mktime(&time_in);
      const std::tm* time_out = std::localtime(&time_temp);
      Week = time_out->tm_wday;
    }
  } else if (AnimState == Shown && !GetFlag(SF_DATEDISPLAY)) {
    AnimState = Hiding;
  }

  if (AnimState == Hiding) {
    Fade -= dt / FadeOutDuration;
    if (Fade <= 0.0f) {
      Fade = 0.0f;
      AnimState = Hidden;
    }
  } else if (AnimState == Showing) {
    Fade += dt / FadeInDuration;
    if (Fade >= 1.0f) {
      Fade = 1.0f;
      AnimState = Shown;
    }
  }
}

void Render() {
  if (AnimState == Hidden) return;
  if (Fade > 0.0f) {
    float smoothedFade = glm::smoothstep(0.0f, 1.0f, Fade);

    glm::vec4 col(1.0f);
    col.a = smoothedFade;

    Renderer2D::DrawSprite(
        BackgroundSprite,
        glm::mix(BackgroundStartPos, BackgroundEndPos, smoothedFade), col);

    glm::vec2 pos(DateStartX, YearWeekY);
    Renderer2D::DrawSprite(CloseBracketSprite, pos, col);

    pos.x -= WeekSprites[Week].ScaledWidth() + Spacing;
    Renderer2D::DrawSprite(WeekSprites[Week], pos, col);

    pos.x -= OpenBracketSprite.ScaledWidth() + Spacing;
    Renderer2D::DrawSprite(OpenBracketSprite, pos, col);

    pos.x -= YearNumSprites[Year % 10].ScaledWidth();
    Renderer2D::DrawSprite(YearNumSprites[Year % 10], pos, col);
    pos.x -= YearNumSprites[Year / 10].ScaledWidth();
    Renderer2D::DrawSprite(YearNumSprites[Year / 10], pos, col);
    pos.x -= YearNumSprites[0].ScaledWidth();
    Renderer2D::DrawSprite(YearNumSprites[0], pos, col);
    pos.x -= YearNumSprites[2].ScaledWidth();
    Renderer2D::DrawSprite(YearNumSprites[2], pos, col);

    pos.x -= DYSeparatorSprite.ScaledWidth();
    Renderer2D::DrawSprite(DYSeparatorSprite, pos, col);

    pos.y = MonthDayY;
    pos.x -= DayNumSprites[Day % 10].ScaledWidth() + Spacing;
    Renderer2D::DrawSprite(DayNumSprites[Day % 10], pos, col);
    pos.x -= DayNumSprites[Day / 10].ScaledWidth();
    Renderer2D::DrawSprite(DayNumSprites[Day / 10], pos, col);

    pos.x -= MDSeparatorSprite.ScaledWidth();
    Renderer2D::DrawSprite(MDSeparatorSprite, pos, col);

    pos.x -= MonthNumSprites[Month % 10].ScaledWidth() + Spacing;
    Renderer2D::DrawSprite(MonthNumSprites[Month % 10], pos, col);
    if (Month / 10 != 0) {
      pos.x -= MonthNumSprites[Month / 10].ScaledWidth();
      Renderer2D::DrawSprite(MonthNumSprites[Month / 10], pos, col);
    }
  }
}

}  // namespace DateDisplay
}  // namespace Impacto