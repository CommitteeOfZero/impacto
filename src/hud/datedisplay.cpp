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
    Fade -= dt / Profile::DateDisplay::FadeOutDuration;
    if (Fade <= 0.0f) {
      Fade = 0.0f;
      AnimState = Hidden;
    }
  } else if (AnimState == Showing) {
    Fade += dt / Profile::DateDisplay::FadeInDuration;
    if (Fade >= 1.0f) {
      Fade = 1.0f;
      AnimState = Shown;
    }
  }
}

void Render() {
  if (AnimState == Hidden) return;
  if (Fade > 0.0f) {
    glm::vec4 col(1.0f);
    col.a = glm::smoothstep(0.0f, 1.0f, Fade);

    Renderer2D::DrawSprite(
        Profile::DateDisplay::BackgroundSprite,
        glm::vec2(Profile::DateDisplay::BackgroundStartX - (col.a * 256.0f),
                  Profile::DateDisplay::BackgroundY),
        col);

    float dateCurrentX = Profile::DateDisplay::DateStartX;
    Renderer2D::DrawSprite(
        Profile::DateDisplay::CloseBracketSprite,
        glm::vec2(dateCurrentX, Profile::DateDisplay::YearWeekY), col);

    dateCurrentX -= Profile::DateDisplay::WeekSprites[Week].ScaledWidth() +
                    Profile::DateDisplay::Spacing;
    Renderer2D::DrawSprite(
        Profile::DateDisplay::WeekSprites[Week],
        glm::vec2(dateCurrentX, Profile::DateDisplay::YearWeekY), col);

    dateCurrentX -= Profile::DateDisplay::OpenBracketSprite.ScaledWidth() +
                    Profile::DateDisplay::Spacing;
    Renderer2D::DrawSprite(
        Profile::DateDisplay::OpenBracketSprite,
        glm::vec2(dateCurrentX, Profile::DateDisplay::YearWeekY), col);

    dateCurrentX -=
        Profile::DateDisplay::YearNumSprites[Year % 10].ScaledWidth();
    Renderer2D::DrawSprite(
        Profile::DateDisplay::YearNumSprites[Year % 10],
        glm::vec2(dateCurrentX, Profile::DateDisplay::YearWeekY), col);
    dateCurrentX -=
        Profile::DateDisplay::YearNumSprites[Year / 10].ScaledWidth();
    Renderer2D::DrawSprite(
        Profile::DateDisplay::YearNumSprites[Year / 10],
        glm::vec2(dateCurrentX, Profile::DateDisplay::YearWeekY), col);
    dateCurrentX -= Profile::DateDisplay::YearNumSprites[0].ScaledWidth();
    Renderer2D::DrawSprite(
        Profile::DateDisplay::YearNumSprites[0],
        glm::vec2(dateCurrentX, Profile::DateDisplay::YearWeekY), col);
    dateCurrentX -= Profile::DateDisplay::YearNumSprites[2].ScaledWidth();
    Renderer2D::DrawSprite(
        Profile::DateDisplay::YearNumSprites[2],
        glm::vec2(dateCurrentX, Profile::DateDisplay::YearWeekY), col);

    dateCurrentX -= Profile::DateDisplay::DYSeparatorSprite.ScaledWidth();
    Renderer2D::DrawSprite(
        Profile::DateDisplay::DYSeparatorSprite,
        glm::vec2(dateCurrentX, Profile::DateDisplay::YearWeekY), col);

    dateCurrentX -=
        Profile::DateDisplay::DayNumSprites[Day % 10].ScaledWidth() +
        Profile::DateDisplay::Spacing;
    Renderer2D::DrawSprite(
        Profile::DateDisplay::DayNumSprites[Day % 10],
        glm::vec2(dateCurrentX, Profile::DateDisplay::MonthDayY), col);
    dateCurrentX -= Profile::DateDisplay::DayNumSprites[Day / 10].ScaledWidth();
    Renderer2D::DrawSprite(
        Profile::DateDisplay::DayNumSprites[Day / 10],
        glm::vec2(dateCurrentX, Profile::DateDisplay::MonthDayY), col);

    dateCurrentX -= Profile::DateDisplay::MDSeparatorSprite.ScaledWidth();
    Renderer2D::DrawSprite(
        Profile::DateDisplay::MDSeparatorSprite,
        glm::vec2(dateCurrentX, Profile::DateDisplay::MonthDayY), col);

    dateCurrentX -=
        Profile::DateDisplay::MonthNumSprites[Month % 10].ScaledWidth() +
        Profile::DateDisplay::Spacing;
    Renderer2D::DrawSprite(
        Profile::DateDisplay::MonthNumSprites[Month % 10],
        glm::vec2(dateCurrentX, Profile::DateDisplay::MonthDayY), col);
    if (Month / 10 != 0) {
      dateCurrentX -=
          Profile::DateDisplay::MonthNumSprites[Month / 10].ScaledWidth();
      Renderer2D::DrawSprite(
          Profile::DateDisplay::MonthNumSprites[Month / 10],
          glm::vec2(dateCurrentX, Profile::DateDisplay::MonthDayY), col);
    }
  }
}

}  // namespace DateDisplay
}  // namespace Impacto