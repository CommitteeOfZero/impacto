#include "saveicondisplay.h"

#include "../impacto.h"
#include "../renderer2d.h"
#include "../game.h"
#include "../spriteanimation.h"

#include "../profile/hud/saveicon.h"

namespace Impacto {
namespace SaveIconDisplay {

static glm::vec2 Position;

static SpriteAnimation SaveIconForeground;
static Animation FadeAnimation;

void Init() {
  Profile::SaveIcon::Configure();
  FadeAnimation.DurationIn = Profile::SaveIcon::FadeInDuration;
  FadeAnimation.DurationOut = Profile::SaveIcon::FadeOutDuration;
  SaveIconForeground = Profile::SaveIcon::ForegroundAnimation.Instantiate();
  SaveIconForeground.LoopMode = ALM_Loop;
}

void Hide() { FadeAnimation.StartOut(); }

void Show() { ShowAt(Profile::SaveIcon::DefaultPosition); }
void ShowAt(glm::vec2 pos) {
  // TODO how do we handle Show/Hide during in-progress animation?

  Position = pos;
  SaveIconForeground.StartIn();
  FadeAnimation.StartIn();
}

void Update(float dt) {
  FadeAnimation.Update(dt);
  if (FadeAnimation.IsOut()) return;
  SaveIconForeground.Update(dt);
}

void Render() {
  if (FadeAnimation.IsOut()) return;

  glm::vec4 col(1.0f);
  col.a = glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress);
  Renderer2D::DrawSprite(
      Profile::SaveIcon::BackgroundSprite,
      Position + Profile::SaveIcon::BackgroundOffset,
      col * glm::vec4(1.0f, 1.0f, 1.0f, Profile::SaveIcon::BackgroundMaxAlpha));
  Renderer2D::DrawSprite(SaveIconForeground.CurrentSprite(), Position, col);
}

}  // namespace SaveIconDisplay
}  // namespace Impacto