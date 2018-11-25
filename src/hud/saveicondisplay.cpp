#include "saveicondisplay.h"

#include "../impacto.h"
#include "../renderer2d.h"
#include "../game.h"
#include "../spriteanimation.h"

#include "../profile/hud/saveicon.h"

namespace Impacto {
namespace SaveIconDisplay {

enum SaveIconAnimState { Hidden, Hiding, Showing, Shown };

static float Fade = 0.0f;

static glm::vec2 Position;
SaveIconAnimState AnimState = Hidden;

static SpriteAnimation SaveIconForeground;

void Hide() { AnimState = Hiding; }

void Show() { ShowAt(Profile::SaveIcon::DefaultPosition); }
void ShowAt(glm::vec2 pos) {
  Position = pos;
  AnimState = Showing;
  SaveIconForeground = Profile::SaveIcon::ForegroundAnimation.Instantiate();
}

void Update(float dt) {
  if (AnimState == Hiding) {
    Fade -= dt / Profile::SaveIcon::FadeOutDuration;
    if (Fade <= 0.0f) {
      Fade = 0.0f;
      AnimState = Hidden;
    }
  } else if (AnimState == Showing) {
    Fade += dt / Profile::SaveIcon::FadeInDuration;
    if (Fade >= 1.0f) {
      Fade = 1.0f;
      AnimState = Shown;
    }
  }

  if (AnimState == Hidden) return;

  SaveIconForeground.Update(dt);
}

void Render() {
  if (AnimState == Hidden) return;
  if (Fade > 0.0f) {
    glm::vec4 col(1.0f);
    col.a = glm::smoothstep(0.0f, 1.0f, Fade);
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