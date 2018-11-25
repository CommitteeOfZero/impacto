#include "datedisplay.h"

#include "../scriptvars.h"
#include "../impacto.h"
#include "../mem.h"
#include <ctime>
#include "../renderer2d.h"
#include "../game.h"

namespace Impacto {
namespace DateDisplay {

static float DateOpacity = 0.0f;

void Render() {
  if (ScrWork[LR_DATE] == 0 || !GetFlag(SF_DATEDISPLAY)) {
    return;
  }
  int year = (ScrWork[LR_DATE] / 10000);
  int month = (ScrWork[LR_DATE] - 10000 * year) / 100;
  int day = (ScrWork[LR_DATE] - 10000 * year) % 100;
  std::tm time_in = {0, 0, 0, day, month - 1, year + 100};
  std::time_t time_temp = std::mktime(&time_in);
  const std::tm* time_out = std::localtime(&time_temp);
  int weekDay = time_out->tm_wday;

  Sprite dateLine;
  dateLine.Sheet = Profile::Dlg.DataSpriteSheet;
  dateLine.Bounds = RectF(1525.0f, 1.0f, 450.0f, 28.0f);
  glm::vec4 col;
  col.r = 1.0f;
  col.g = 1.0f;
  col.b = 1.0f;
  if (DateOpacity < 1.0f) DateOpacity += 0.01;
  col.a = glm::smoothstep(0.0f, 1.0f, DateOpacity);
  Renderer2D::DrawSprite(dateLine, glm::vec2(1086.0f - (col.a * 256.0f), 73.0f),
                         col);

  Sprite dateBracketClose;
  dateBracketClose.Sheet = Profile::Dlg.DataSpriteSheet;
  dateBracketClose.Bounds = RectF(1776.0f, 63.0f, 8.0f, 20.0f);
  Renderer2D::DrawSprite(dateBracketClose, glm::vec2(1167.0f, 60.0f), col);

  float weekDayX, weekDayY, weekDayDX;
  switch (weekDay) {
    case 0:
      weekDayX = 1785.0f;
      weekDayY = 62.0f;
      weekDayDX = 73.0f;
      break;
    case 1:
      weekDayX = 1858.0f;
      weekDayY = 62.0f;
      weekDayDX = 73.0f;
      break;
    case 2:
      weekDayX = 1524.0f;
      weekDayY = 84.0f;
      weekDayDX = 73.0f;
      break;
    case 3:
      weekDayX = 1597.0f;
      weekDayY = 84.0f;
      weekDayDX = 73.0f;
      break;
    case 4:
      weekDayX = 1670.0f;
      weekDayY = 84.0f;
      weekDayDX = 73.0f;
      break;
    case 5:
      weekDayX = 1743.0f;
      weekDayY = 84.0f;
      weekDayDX = 56.0f;
      break;
    case 6:
      weekDayX = 1799.0f;
      weekDayY = 84.0f;
      weekDayDX = 73.0f;
      break;
  }
  Sprite dateWeek;
  dateWeek.Sheet = Profile::Dlg.DataSpriteSheet;
  dateWeek.Bounds =
      RectF(weekDayX + 1.0f, weekDayY + 1.0f, weekDayDX - 2.0f, 20.0f);
  Renderer2D::DrawSprite(dateWeek, glm::vec2(1167.0f - weekDayDX, 60.0f), col);

  Sprite dateBracketOpen;
  dateBracketOpen.Sheet = Profile::Dlg.DataSpriteSheet;
  dateBracketOpen.Bounds = RectF(1766.0f, 63.0f, 8.0f, 20.0f);
  Renderer2D::DrawSprite(dateBracketOpen, glm::vec2(1157.0f - weekDayDX, 60.0f),
                         col);

  float year1X, year1Y, year1DX;
  if (year % 10 > 1)
    year1X = 25 * (year % 10) + 1508.0f;
  else
    year1X = 25 * (year % 10) + 1525.0f;
  if (year % 10 == 1) {
    year1Y = 8.0f;
    year1DX = 1157.0f - weekDayDX - 8.0f;
  } else {
    year1Y = 25.0f;
    year1DX = 1157.0f - weekDayDX - 25.0f;
  }

  Sprite dateYear1;
  dateYear1.Sheet = Profile::Dlg.DataSpriteSheet;
  dateYear1.Bounds = RectF(year1X, 63.0f, year1Y - 2.0f, 20.0f);
  Renderer2D::DrawSprite(dateYear1, glm::vec2(year1DX + 1.0f, 60.0f), col);

  float year2X, year2Y, year2DX;
  if (year / 10 > 1)
    year2X = 25 * (year / 10) + 1508.0f;
  else
    year2X = 25 * (year / 10) + 1525.0f;
  if (year / 10 == 1) {
    year2Y = 8.0f;
    year2DX = year1DX - 8.0f;
  } else {
    year2Y = 25.0f;
    year2DX = year1DX - 25.0f;
  }

  Sprite dateYear2;
  dateYear2.Sheet = Profile::Dlg.DataSpriteSheet;
  dateYear2.Bounds = RectF(year2X, 63.0f, year2Y - 2.0f, 20.0f);
  Renderer2D::DrawSprite(dateYear2, glm::vec2(year2DX + 1.0f, 60.0f), col);
  Sprite dateYear3;
  dateYear3.Sheet = Profile::Dlg.DataSpriteSheet;
  dateYear3.Bounds = RectF(1525.0f, 63.0f, 23.0f, 20.0f);
  Renderer2D::DrawSprite(dateYear3, glm::vec2(year2DX - 24.0f, 60.0f), col);
  Sprite dateYear4;
  dateYear4.Sheet = Profile::Dlg.DataSpriteSheet;
  dateYear4.Bounds = RectF(1558.0f, 63.0f, 23.0f, 20.0f);
  Renderer2D::DrawSprite(dateYear4, glm::vec2(year2DX - 49.0f, 60.0f), col);
  Sprite dateYearDot;
  dateYearDot.Sheet = Profile::Dlg.DataSpriteSheet;
  dateYearDot.Bounds = RectF(1758.0f, 63.0f, 8.0f, 20.0f);
  Renderer2D::DrawSprite(dateYearDot, glm::vec2(year2DX - 57.0f, 60.0f), col);

  float day1X, day1Y, day1DX;
  if (day % 10 > 1)
    day1X = 40 * (day % 10) + 1496.0f;
  else
    day1X = 40 * (day % 10) + 1524.0f;
  if (day % 10 == 1) {
    day1Y = 12.0f;
    day1DX = year2DX - 57.0f - 12.0f;
  } else {
    day1Y = 40.0f;
    day1DX = year2DX - 57.0f - 40.0f;
  }

  Sprite dateDay1;
  dateDay1.Sheet = Profile::Dlg.DataSpriteSheet;
  dateDay1.Bounds = RectF(day1X, 33.0f, day1Y - 2.0f, 28.0f);
  Renderer2D::DrawSprite(dateDay1, glm::vec2(day1DX + 1.0f, 52.0f), col);

  float day2X, day2Y, day2DX;
  if (day / 10 > 1)
    day2X = 40 * (day / 10) + 1496.0f;
  else
    day2X = 40 * (day / 10) + 1524.0f;
  if (day / 10 == 1) {
    day2Y = 12.0f;
    day2DX = day1DX - 12.0f;
  } else {
    day2Y = 40.0f;
    day2DX = day1DX - 40.0f;
  }

  Sprite dateDay2;
  dateDay2.Sheet = Profile::Dlg.DataSpriteSheet;
  dateDay2.Bounds = RectF(day2X, 33.0f, day2Y - 2.0f, 28.0f);
  Renderer2D::DrawSprite(dateDay2, glm::vec2(day2DX + 1.0f, 52.0f), col);
  Sprite dateDayDot;
  dateDayDot.Sheet = Profile::Dlg.DataSpriteSheet;
  dateDayDot.Bounds = RectF(1897.0f, 33.0f, 10.0f, 28.0f);
  Renderer2D::DrawSprite(dateDayDot, glm::vec2(day2DX - 11.0f, 52.0f), col);

  float month1X, month1Y, month1DX;
  if (month % 10 > 1)
    month1X = 40 * (month % 10) + 1496.0f;
  else
    month1X = 40 * (month % 10) + 1524.0f;
  if (month % 10 == 1) {
    month1Y = 12.0f;
    month1DX = day2DX - 11.0f - 12.0f;
  } else {
    month1Y = 40.0f;
    month1DX = day2DX - 11.0f - 40.0f;
  }
  Sprite dateMonth1;
  dateMonth1.Sheet = Profile::Dlg.DataSpriteSheet;
  dateMonth1.Bounds = RectF(month1X, 33.0f, month1Y - 2.0f, 28.0f);
  Renderer2D::DrawSprite(dateMonth1, glm::vec2(month1DX + 1.0f, 52.0f), col);

  if (month / 10 != 0) {
    float month2X, month2Y, month2DX;
    if (month / 10 > 1)
      month2X = 40 * (month / 10) + 1496.0f;
    else
      month2X = 40 * (month / 10) + 1524.0f;
    if (month / 10 == 1) {
      month2Y = 12.0f;
      month2DX = month1DX - 12.0f;
    } else {
      month2Y = 40.0f;
      month2DX = month1DX - 40.0f;
    }

    Sprite dateMonth2;
    dateMonth2.Sheet = Profile::Dlg.DataSpriteSheet;
    dateMonth2.Bounds = RectF(month2X, 33.0f, month2Y - 2.0f, 28.0f);
    Renderer2D::DrawSprite(dateMonth2, glm::vec2(month2DX + 1.0f, 52.0f), col);
  }
}

}  // namespace DateDisplay
}  // namespace Impacto