#include "saveicondisplay.h"

#include "impacto.h"
#include "renderer2d.h"
#include "game.h"

namespace Impacto {
namespace SaveIconDisplay {

static float SaveIconAnim = 0.0f;
static float SaveIconOpacity = 0.0f;
float PositionX = 1153.0f;
float PositionY = 23.0f;
SaveIconAnimState AnimState = Hidden;

void Update(float dt) {
  SaveIconAnim += 16.6f * dt;
  if (SaveIconAnim > 7.0f) SaveIconAnim = 0.0f;

  if (AnimState == Hiding) {
    if (SaveIconOpacity > 0.0f)
      SaveIconOpacity -= 1.8f * dt;
    else {
      PositionX = 1153.0f;
      PositionY = 23.0f;
      AnimState = Hidden;
    }
  } else if (AnimState == Showing) {
    SaveIconOpacity += 1.8f * dt;
    if (SaveIconOpacity >= 1.0f) {
      SaveIconOpacity = 1.0f;
      AnimState = Shown;
    }
  }
}

void Render() {
  if (SaveIconOpacity > 0.0f) {
    glm::vec4 col;
    col.r = 1.0f;
    col.g = 1.0f;
    col.b = 1.0f;
    col.a = glm::smoothstep(0.0f, 1.0f, SaveIconOpacity);
    Sprite saveIconBg;
    saveIconBg.Sheet = Profile::Dlg.DataSpriteSheet;
    saveIconBg.Bounds = RectF(1439.0f, 1.0f, 84.0f, 84.0f);
    saveIconBg.BaseScale = glm::vec2(1.0f);
    Sprite saveIcon;
    saveIcon.Sheet = Profile::Dlg.DataSpriteSheet;
    saveIcon.Bounds =
        RectF(1977.0f, (glm::floor(SaveIconAnim) * 72.0f) + 1.0f, 70.0f, 70.0f);
    saveIcon.BaseScale = glm::vec2(1.0f);
    Renderer2D::DrawSprite(
        saveIconBg, RectF(PositionX - 7.0f, PositionY - 4.0f, 84.0f, 84.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, col.a / 2.0f));
    Renderer2D::DrawSprite(saveIcon, RectF(PositionX, PositionY, 70.0f, 70.0f),
                           col);
  }
}

}  // namespace SaveIconDisplay
}  // namespace Impacto