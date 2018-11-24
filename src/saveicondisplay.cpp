#include "saveicondisplay.h"

#include "impacto.h"
#include "renderer2d.h"
#include "game.h"
#include "spriteanimation.h"

#include "profile/saveicon.h"

namespace Impacto {
namespace SaveIconDisplay {

enum SaveIconAnimState { Hidden, Hiding, Showing, Shown };

static float Opacity = 0.0f;

static glm::vec2 Position;
SaveIconAnimState AnimState = Hidden;

static SpriteAnimation SaveIconForeground;

void Hide() {
  AnimState = Hidden;
  Opacity = 0.0f;
}

void Show() { ShowAt(Profile::SaveIcon::DefaultPosition); }
void ShowAt(glm::vec2 pos) {
  Position = pos;
  AnimState = Showing;
  SaveIconForeground = Profile::SaveIcon::ForegroundAnimation.Instantiate();
}

void Update(float dt) {
  if (AnimState == Hidden) return;

  SaveIconForeground.Update(dt);

  if (AnimState == Hiding) {
    Opacity -= 4.0f * dt;
    if (Opacity <= 0.0f) {
      Opacity = 0.0f;
      AnimState = Hidden;
    }
  } else if (AnimState == Showing) {
    Opacity += 1.8f * dt;
    if (Opacity >= 1.0f) {
      Opacity = 1.0f;
      AnimState = Shown;
    }
  }
}

void Render() {
  if (AnimState == Hidden) return;
  if (Opacity > 0.0f) {
    glm::vec4 col(1.0f);
    col.a = glm::smoothstep(0.0f, 1.0f, Opacity);
    Renderer2D::DrawSprite(
        Profile::SaveIcon::BackgroundSprite,
        Position + Profile::SaveIcon::BackgroundOffset,
        col *
            glm::vec4(1.0f, 1.0f, 1.0f, Profile::SaveIcon::BackgroundMaxAlpha));
    Renderer2D::DrawSprite(SaveIconForeground.CurrentSprite(), Position, col);
  }
}

}  // namespace SaveIconDisplay
}  // namespace Impacto