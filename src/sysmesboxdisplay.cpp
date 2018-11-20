#include "sysmesboxdisplay.h"

#include "impacto.h"
#include "renderer2d.h"
#include "game.h"
#include "mem.h"
#include "scriptvars.h"

namespace Impacto {
namespace SysMesBoxDisplay {

static float BoxAnimCount = 0.0f;
int MessageCount;
float BoxOpacity = 0.0f;
SysMesBoxAnimState AnimState = Hidden;

void Update(float dt) {
  if (AnimState == Hiding) {
    BoxAnimCount -= 55.0f * dt;
    if (BoxAnimCount <= 0.0f) {
      BoxAnimCount = 0.0f;
      AnimState = Hidden;
    }
  } else if (AnimState == Showing) {
    BoxAnimCount += 55.0f * dt;
    if (BoxAnimCount >= ScrWork[SW_SYSMESANIMCTF]) {
      BoxAnimCount = ScrWork[SW_SYSMESANIMCTF];
      AnimState = Shown;
    }
  }
  ScrWork[SW_SYSMESANIMCTCUR] = BoxAnimCount;
}

void Render() {
  if (BoxOpacity) {
    glm::vec4 col;
    col.r = 1.0f;
    col.g = 1.0f;
    col.b = 1.0f;
    col.a = glm::smoothstep(0.0f, 1.0f, BoxOpacity);
    Sprite sysMesBoxPart1;
    sysMesBoxPart1.Sheet = Profile::Dlg.DataSpriteSheet;
    sysMesBoxPart1.Bounds = RectF(1383.0f, 107.0f, 592.0f, 8.0f);
    sysMesBoxPart1.BaseScale = glm::vec2(1.0f);

    int posParam;

    if (ScrWork[SW_SYSMESANIMCTCUR] > 9) {
      if ((ScrWork[SW_SYSMESANIMCTCUR] - 9) >= 2 * MessageCount + 8)
        posParam = 28 * MessageCount + 112;
      else
        posParam = 14 * (ScrWork[SW_SYSMESANIMCTCUR] - 9);

      Renderer2D::DrawRect(RectF(344.0f, (360.0f - (posParam / 2.0f)) - 1.0f,
                                 592.0f, posParam + 2.0f),
                           glm::vec4(1.0f, 1.0f, 1.0f, 0.75f));
      Renderer2D::DrawSprite(
          sysMesBoxPart1,
          RectF(344.0f, (360.0f - (posParam / 2.0f)) - 3.0f, 592.0f, 8.0f),
          col);

      sysMesBoxPart1.Bounds = RectF(1383.0f, 117.0f, 592.0f, 8.0f);
      Renderer2D::DrawSprite(
          sysMesBoxPart1,
          RectF(344.0f, ((360.0f - (posParam / 2.0f)) + posParam) - 3.0f,
                592.0f, 8.0f),
          col);

      if ((ScrWork[SW_SYSMESANIMCTCUR] - 9) > 2 * MessageCount + 8) {
        if (posParam > 56) {
          Sprite sysMesBoxPart2;
          sysMesBoxPart2.Sheet = Profile::Dlg.DataSpriteSheet;
          sysMesBoxPart2.Bounds = RectF(1383.0f, 127.0f, 592.0f, 2.0f);
          sysMesBoxPart2.BaseScale = glm::vec2(1.0f);
          Renderer2D::DrawSprite(
              sysMesBoxPart2,
              RectF(344.0f, (360.0f - (posParam / 2.0f)) + 36.0f, 592.0f, 2.0f),
              col);
          Renderer2D::DrawSprite(
              sysMesBoxPart2,
              RectF(344.0f, ((360.0f - (posParam / 2.0f)) + posParam) - 36.0f,
                    592.0f, 2.0f),
              col);
        }

        Sprite sysMesBoxPart3;
        sysMesBoxPart3.Sheet = Profile::Dlg.DataSpriteSheet;
        sysMesBoxPart3.Bounds = RectF(
            1574.0f -
                (12 *
                 ((ScrWork[SW_SYSMESANIMCTCUR] - 9) - 2 * MessageCount - 8)) +
                1.0f,
            131.0f,
            (12 * ((ScrWork[SW_SYSMESANIMCTCUR] - 9) - 2 * MessageCount - 8)) -
                2.0f,
            31.0f);
        sysMesBoxPart3.BaseScale = glm::vec2(1.0f);
        Renderer2D::DrawSprite(
            sysMesBoxPart3,
            RectF(344.0f, (360.0f - (posParam / 2.0f)) + 3.0f,
                  (12 *
                   ((ScrWork[SW_SYSMESANIMCTCUR] - 9) - 2 * MessageCount - 8)) -
                      2.0f,
                  31.0f),
            col);

        // TODO: Draw Yes/No/OK buttons here

        // TODO: Draw text here
      }

    } else {
      float linePosX = 1622.0f;
      float linePosDispX = 114.0f;
      if (ScrWork[SW_SYSMESANIMCTCUR] > 1) {
        linePosDispX = 60.0f * ScrWork[SW_SYSMESANIMCTCUR] + 54.0f;
        linePosX = 1652.0f - 30.0f * ScrWork[SW_SYSMESANIMCTCUR];
      }
      Sprite sysMesBoxLine;
      sysMesBoxLine.Sheet = Profile::Dlg.DataSpriteSheet;
      sysMesBoxLine.Bounds =
          RectF(linePosX + 1.0f, 107.0f, linePosDispX + 2.0f, 8.0f);
      sysMesBoxLine.BaseScale = glm::vec2(1.0f);
      Renderer2D::DrawSprite(sysMesBoxLine,
                             RectF(639.0f - (linePosDispX / 2.0f), 360.0f,
                                   linePosDispX + 2.0f, 8.0f),
                             col);
      sysMesBoxLine.Bounds =
          RectF(linePosX + 1.0f, 117.0f, linePosDispX + 2.0f, 8.0f);
      Renderer2D::DrawSprite(sysMesBoxLine,
                             RectF(639.0f - (linePosDispX / 2.0f) + 1.0f,
                                   352.0f, linePosDispX + 2.0f, 8.0f),
                             col);
    }
  }
}

}  // namespace SysMesBoxDisplay
}  // namespace Impacto