#include "../impacto.h"
#include "../game.h"
#include "gamespecific_rne.h"
#include <ctime>

#include "scriptvars.h"

namespace Impacto {
namespace Vm {

float DateOpacity = 0.0f;

void UpdateCharacterRot(Game* gameCtx, int charId) {
  int pose = gameCtx->ScrWork[30 * charId + SW_CHA1POSE] - 30;

  if (pose >= 0) {
    float targetX = (int)gameCtx->ScrWork[20 * pose + 5500] / 1000.0f;
    float targetY = ((int)gameCtx->ScrWork[20 * pose + 5501] / 1000.0f) + 12.5f;
    float targetZ = (int)gameCtx->ScrWork[20 * pose + 5502] / 1000.0f;
    float objectX = (int)gameCtx->ScrWork[30 * charId + SW_CHA1POSX] / 1000.0f;
    float objectY =
        ((int)gameCtx->ScrWork[30 * charId + SW_CHA1POSY] / 1000.0f) +
        ((int)gameCtx->ScrWork[30 * charId + 5111] / 1000.0f);
    float objectZ = (int)gameCtx->ScrWork[30 * charId + SW_CHA1POSZ] / 1000.0f;
    glm::vec3 lookat = LookAtEulerZYX(glm::vec3(objectX, objectY, objectZ),
                                      glm::vec3(targetX, targetY, targetZ));
    lookat.x = 0.0f;
    gameCtx->Scene3D->Characters[charId].ModelTransform.SetRotationFromEuler(
        lookat);
    gameCtx->ScrWork[30 * charId + SW_CHA1ROTY] =
        -((lookat.y / (2 * M_PI)) * 360) * 1000.0f;
  }
}

void UpdateCharacterPos(Game* gameCtx, int charId) {}

void UpdateCharacters(Game* gameCtx) {
  for (int i = 0; i <= 5; i++) {
    if (gameCtx->Scene3D->Characters[i].Status == LS_Loaded) {
      UpdateCharacterRot(gameCtx, i);
      UpdateCharacterPos(gameCtx, i);
      gameCtx->Scene3D->Characters[i].IsVisible =
          gameCtx->GetFlag(SF_CHA1DISP + i);
      gameCtx->Scene3D->Characters[i].ModelTransform.Position.x =
          (int)gameCtx->ScrWork[SW_CHA1POSX + i * 30] / 1000.0f;
      gameCtx->Scene3D->Characters[i].ModelTransform.Position.y =
          ((int)gameCtx->ScrWork[SW_CHA1POSY + i * 30]) / 1000.0f;
      gameCtx->Scene3D->Characters[i].ModelTransform.Position.z =
          (int)gameCtx->ScrWork[SW_CHA1POSZ + i * 30] / 1000.0f;
    }
  }
  if (gameCtx->Scene3D->Backgrounds[0].Status == LS_Loaded) {
    gameCtx->Scene3D->Backgrounds[0].IsVisible = gameCtx->GetFlag(SF_CHA1DISP);
  }
}

void UpdateCamera(Game* gameCtx) {
  // Update position
  // gameCtx->Scene3D->MainCamera.Move(glm::vec3(0.0f, 12.5f, 23.0f));

  // Update lookat
  gameCtx->Scene3D->MainCamera.LookAt(glm::vec3(0.0f, 12.5f, 0.0f));

  // Update fov
  float hFovRad =
      ((int)gameCtx->ScrWork[SW_IRUOCAMERAHFOV] / 1000.0f) * M_PI / 180.0f;
  gameCtx->Scene3D->MainCamera.Fov =
      2 * atan(tan(hFovRad / 2.0f) * (9.0f / 16.0f));

  // Update lighting
  uint32_t lightColor = gameCtx->ScrWork[SW_MAINLIGHTTINT];
  glm::vec4 lightC = RgbaIntToFloat(lightColor);
  lightC.a = (int)gameCtx->ScrWork[SW_MAINLIGHTINT] / 1000.0f;
  float lightX = (int)gameCtx->ScrWork[SW_MAINLIGHTPOSX] / 1000.0f;
  float lightY = (int)gameCtx->ScrWork[SW_MAINLIGHTPOSY] / 1000.0f;
  float lightZ = (int)gameCtx->ScrWork[SW_MAINLIGHTPOSZ] / 1000.0f;
  gameCtx->Scene3D->Tint = lightC;
  gameCtx->Scene3D->LightPosition = glm::vec3(lightX, lightY, lightZ);
  gameCtx->Scene3D->DarkMode = (bool)gameCtx->ScrWork[SW_MAINLIGHTDARKMODE];
}

void SetDateDisplay(Game* gameCtx) {
  if (gameCtx->ScrWork[LR_DATE] == 0 || !gameCtx->GetFlag(SF_DATEDISPLAY)) {
    return;
  }
  int year = (gameCtx->ScrWork[LR_DATE] / 10000);
  int month = (gameCtx->ScrWork[LR_DATE] - 10000 * year) / 100;
  int day = (gameCtx->ScrWork[LR_DATE] - 10000 * year) % 100;
  std::tm time_in = {0, 0, 0, day, month - 1, year + 100};
  std::time_t time_temp = std::mktime(&time_in);
  const std::tm* time_out = std::localtime(&time_temp);
  int weekDay = time_out->tm_wday;

  Sprite dateLine;
  dateLine.Sheet = gameCtx->Config.Dlg.DataSpriteSheet;
  dateLine.Bounds = RectF(1525.0f, 1.0f, 450.0f, 28.0f);
  dateLine.BaseScale = glm::vec2(1.0f);
  glm::vec4 col;
  col.r = 1.0f;
  col.g = 1.0f;
  col.b = 1.0f;
  if (DateOpacity < 1.0f) DateOpacity += 0.01;
  col.a = glm::smoothstep(0.0f, 1.0f, DateOpacity);
  gameCtx->R2D->DrawSprite(
      dateLine, RectF(1087.0f - (col.a * 256.0f) + 1.0f, 73.0f, 450.0f, 28.0f),
      col);

  Sprite dateBracketClose;
  dateBracketClose.Sheet = gameCtx->Config.Dlg.DataSpriteSheet;
  dateBracketClose.Bounds = RectF(1776.0f, 63.0f, 8.0f, 20.0f);
  dateBracketClose.BaseScale = glm::vec2(1.0f);
  gameCtx->R2D->DrawSprite(dateBracketClose, RectF(1167.0f, 60.0f, 8.0f, 20.0f),
                           col);

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
  dateWeek.Sheet = gameCtx->Config.Dlg.DataSpriteSheet;
  dateWeek.Bounds =
      RectF(weekDayX + 1.0f, weekDayY + 1.0f, weekDayDX - 2.0f, 20.0f);
  dateWeek.BaseScale = glm::vec2(1.0f);
  gameCtx->R2D->DrawSprite(
      dateWeek, RectF(1167.0f - weekDayDX, 60.0f, weekDayDX - 2.0f, 20.0f),
      col);

  Sprite dateBracketOpen;
  dateBracketOpen.Sheet = gameCtx->Config.Dlg.DataSpriteSheet;
  dateBracketOpen.Bounds = RectF(1766.0f, 63.0f, 8.0f, 20.0f);
  dateBracketOpen.BaseScale = glm::vec2(1.0f);
  gameCtx->R2D->DrawSprite(dateBracketOpen,
                           RectF(1157.0f - weekDayDX, 60.0f, 8.0f, 20.0f), col);

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
  dateYear1.Sheet = gameCtx->Config.Dlg.DataSpriteSheet;
  dateYear1.Bounds = RectF(year1X, 63.0f, year1Y - 2.0f, 20.0f);
  dateYear1.BaseScale = glm::vec2(1.0f);
  gameCtx->R2D->DrawSprite(
      dateYear1, RectF(year1DX + 1.0f, 60.0f, year1Y - 2.0f, 20.0f), col);

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
  dateYear2.Sheet = gameCtx->Config.Dlg.DataSpriteSheet;
  dateYear2.Bounds = RectF(year2X, 63.0f, year2Y - 2.0f, 20.0f);
  dateYear2.BaseScale = glm::vec2(1.0f);
  gameCtx->R2D->DrawSprite(
      dateYear2, RectF(year2DX + 1.0f, 60.0f, year2Y - 2.0f, 20.0f), col);
  Sprite dateYear3;
  dateYear3.Sheet = gameCtx->Config.Dlg.DataSpriteSheet;
  dateYear3.Bounds = RectF(1525.0f, 63.0f, 23.0f, 20.0f);
  dateYear3.BaseScale = glm::vec2(1.0f);
  gameCtx->R2D->DrawSprite(dateYear3,
                           RectF(year2DX - 24.0f, 60.0f, 23.0f, 20.0f), col);
  Sprite dateYear4;
  dateYear4.Sheet = gameCtx->Config.Dlg.DataSpriteSheet;
  dateYear4.Bounds = RectF(1558.0f, 63.0f, 23.0f, 20.0f);
  dateYear4.BaseScale = glm::vec2(1.0f);
  gameCtx->R2D->DrawSprite(dateYear4,
                           RectF(year2DX - 49.0f, 60.0f, 23.0f, 20.0f), col);
  Sprite dateYearDot;
  dateYearDot.Sheet = gameCtx->Config.Dlg.DataSpriteSheet;
  dateYearDot.Bounds = RectF(1758.0f, 63.0f, 8.0f, 20.0f);
  dateYearDot.BaseScale = glm::vec2(1.0f);
  gameCtx->R2D->DrawSprite(dateYearDot,
                           RectF(year2DX - 57.0f, 60.0f, 8.0f, 20.0f), col);

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
  dateDay1.Sheet = gameCtx->Config.Dlg.DataSpriteSheet;
  dateDay1.Bounds = RectF(day1X, 33.0f, day1Y - 2.0f, 28.0f);
  dateDay1.BaseScale = glm::vec2(1.0f);
  gameCtx->R2D->DrawSprite(
      dateDay1, RectF(day1DX + 1.0f, 52.0f, day1Y - 2.0f, 28.0f), col);

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
  dateDay2.Sheet = gameCtx->Config.Dlg.DataSpriteSheet;
  dateDay2.Bounds = RectF(day2X, 33.0f, day2Y - 2.0f, 28.0f);
  dateDay2.BaseScale = glm::vec2(1.0f);
  gameCtx->R2D->DrawSprite(
      dateDay2, RectF(day2DX + 1.0f, 52.0f, day2Y - 2.0f, 28.0f), col);
  Sprite dateDayDot;
  dateDayDot.Sheet = gameCtx->Config.Dlg.DataSpriteSheet;
  dateDayDot.Bounds = RectF(1897.0f, 33.0f, 10.0f, 28.0f);
  dateDayDot.BaseScale = glm::vec2(1.0f);
  gameCtx->R2D->DrawSprite(dateDayDot,
                           RectF(day2DX - 11.0f, 52.0f, 10.0f, 28.0f), col);

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
  dateMonth1.Sheet = gameCtx->Config.Dlg.DataSpriteSheet;
  dateMonth1.Bounds = RectF(month1X, 33.0f, month1Y - 2.0f, 28.0f);
  dateMonth1.BaseScale = glm::vec2(1.0f);
  gameCtx->R2D->DrawSprite(
      dateMonth1, RectF(month1DX + 1.0f, 52.0f, month1Y - 2.0f, 28.0f), col);

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
    dateMonth2.Sheet = gameCtx->Config.Dlg.DataSpriteSheet;
    dateMonth2.Bounds = RectF(month2X, 33.0f, month2Y - 2.0f, 28.0f);
    dateMonth2.BaseScale = glm::vec2(1.0f);
    gameCtx->R2D->DrawSprite(
        dateMonth2, RectF(month2DX + 1.0f, 52.0f, month2Y - 2.0f, 28.0f), col);
  }
}

}  // namespace Vm
}  // namespace Impacto
