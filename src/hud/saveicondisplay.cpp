#include "saveicondisplay.h"

#include "../impacto.h"
#include "../renderer/renderer.h"
#include "../game.h"
#include "../spriteanimation.h"

#include "../profile/hud/saveicon.h"

namespace Impacto {
namespace SaveIconDisplay {

static glm::vec2 Position;

static SpriteAnimation SaveIconForeground;
static Animation FadeAnimation;
static Animation Timer;

static bool IsTimed = false;

void Init() {
  Profile::SaveIcon::Configure();
  FadeAnimation.DurationIn = Profile::SaveIcon::FadeInDuration;
  FadeAnimation.DurationOut = Profile::SaveIcon::FadeOutDuration;
  SaveIconForeground = Profile::SaveIcon::ForegroundAnimation.Instantiate();
  SaveIconForeground.LoopMode = ALM_Loop;
}

void Hide() {
  FadeAnimation.StartOut();
  IsTimed = false;
}

void Show() { ShowAt(Profile::SaveIcon::DefaultPosition); }
void ShowAt(glm::vec2 pos) {
  // TODO how do we handle Show/Hide during in-progress animation?

  Position = pos;
  SaveIconForeground.StartIn();
  FadeAnimation.StartIn();
}
void ShowFor(float seconds) {
  Timer.LoopMode = ALM_Stop;
  Timer.DurationIn = seconds;
  Timer.DurationOut = seconds;
  IsTimed = true;
  Timer.StartIn(true);
  Show();
}

void Update(float dt) {
  FadeAnimation.Update(dt);
  Timer.Update(dt);
  if (IsTimed && Timer.IsIn()) Hide();
  if (FadeAnimation.IsOut()) return;
  SaveIconForeground.Update(dt);
}

void Render() {
  if (FadeAnimation.IsOut()) return;

  glm::vec4 col(1.0f);
  col.a = glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress);
  Renderer->DrawSprite(
      Profile::SaveIcon::BackgroundSprite,
      Position + Profile::SaveIcon::BackgroundOffset,
      col * glm::vec4(1.0f, 1.0f, 1.0f, Profile::SaveIcon::BackgroundMaxAlpha));
  Renderer->DrawSprite(SaveIconForeground.CurrentSprite(), Position, col);
}

}  // namespace SaveIconDisplay
}  // namespace Impacto