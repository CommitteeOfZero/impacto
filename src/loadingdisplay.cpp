#include "loadingdisplay.h"

#include "impacto.h"
#include "renderer2d.h"
#include "game.h"
#include "mem.h"
#include "scriptvars.h"

namespace Impacto {
namespace LoadingDisplay {

float LoadingOpacity = 0.0f;
float LoadingAnim = 0.0f;
float LoadingAnimBg = 0.0f;
float LoadingAnimBgMax = 0.0f;

LoadingAnimState AnimState = Hidden;

void Update(float dt) {
  if (AnimState == Hidden) {
    if (GetFlag(SF_LOADINGFROMSAVE)) {
      AnimState = Showing;
      LoadingAnimBgMax = 4.0f;
    } else if (GetFlag(SF_LOADING)) {
      AnimState = Showing;
      LoadingAnimBgMax = 7.0f;
    }
  } else if (AnimState == Shown && !GetFlag(SF_LOADINGFROMSAVE) &&
             !GetFlag(SF_LOADING)) {
    AnimState = Hiding;
  }

  if (AnimState != Hidden) {
    LoadingAnim += 7.0f * dt;
    if (LoadingAnim > 5.0f) LoadingAnim = 0.0f;
    LoadingAnimBg += 7.0f * dt;
    if (LoadingAnimBg > LoadingAnimBgMax) LoadingAnimBg = 0.0f;
  }

  if (AnimState == Hiding) {
    if (LoadingOpacity > 0.0f)
      LoadingOpacity -= 3.0f * dt;
    else {
      AnimState = Hidden;
    }
  } else if (AnimState == Showing) {
    LoadingOpacity += 1.5f * dt;
    if (LoadingOpacity >= 1.0f) {
      LoadingOpacity = 1.0f;
      AnimState = Shown;
    }
  }
}

void Render() {
  if (LoadingOpacity > 0.0f) {
    glm::vec4 col;
    col.r = 1.0f;
    col.g = 1.0f;
    col.b = 1.0f;
    col.a = glm::smoothstep(0.0f, 1.0f, LoadingOpacity);

    float spriteX;
    float spriteY;
    Sprite loadingBg;
    if (GetFlag(SF_LOADINGFROMSAVE)) {
      spriteX = 667.0f;
      spriteY = 83.0f * glm::floor(LoadingAnimBg) - 166.0f;
      if (LoadingAnimBg < 2.0f) {
        spriteX = 388.0f;
        spriteY = 83.0f * glm::floor(LoadingAnimBg);
      }

      loadingBg.Sheet = Profile::Dlg.DataSpriteSheet;
      loadingBg.Bounds = RectF(spriteX, spriteY, 277.0f, 81.0f);
      Renderer2D::DrawSprite(loadingBg, glm::vec2(1019.0f, 598.0f), col);
    } else if (GetFlag(SF_LOADING)) {
      spriteY = (60.0f * glm::floor(LoadingAnimBg) + 544.0f);
      loadingBg.Sheet = Profile::Dlg.DataSpriteSheet;
      loadingBg.Bounds = RectF(555.0f, spriteY, 206.0f, 58.0f);
      Renderer2D::DrawSprite(loadingBg, glm::vec2(1074.0f, 611.0f), col);
    }

    if (LoadingAnim >= 3.0f)
      spriteX = 62.0f * (4.0f - glm::floor(LoadingAnim)) + 172.0f;
    else
      spriteX = 62.0f * glm::floor(LoadingAnim) + 172.0f;
    Sprite loadingDiskIcon;
    loadingDiskIcon.Sheet = Profile::Dlg.DataSpriteSheet;
    loadingDiskIcon.Bounds = RectF(spriteX, 1.0f, 60.0f, 60.0f);
    spriteY = 23.0f * glm::floor(LoadingAnim) + 91.0f;
    Sprite loadingText;
    loadingText.Sheet = Profile::Dlg.DataSpriteSheet;
    loadingText.Bounds = RectF(173.0f, spriteY, 214.0f, 21.0f);

    Renderer2D::DrawSprite(loadingDiskIcon, glm::vec2(986.0f, 608.0f), col);
    Renderer2D::DrawSprite(loadingText, glm::vec2(1025.0f, 628.0f), col);
  }
}

}  // namespace LoadingDisplay
}  // namespace Impacto