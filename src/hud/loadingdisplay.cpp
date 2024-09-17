#include "loadingdisplay.h"

#include "../impacto.h"
#include "../renderer/renderer.h"
#include "../game.h"
#include "../mem.h"
#include "../profile/scriptvars.h"
#include "../spriteanimation.h"
#include "../profile/hud/loadingdisplay.h"

namespace Impacto {
namespace LoadingDisplay {

using namespace Impacto::Profile::ScriptVars;

static bool IsResourceLoad = false;

static SpriteAnimation ResourceLoadBg;
static SpriteAnimation SaveLoadBg;
static SpriteAnimation LoadingIcon;
static SpriteAnimation LoadingText;
static Animation FadeAnimation;

void Init() {
  Profile::LoadingDisplay::Configure();
  FadeAnimation.DurationIn = Profile::LoadingDisplay::FadeInDuration;
  FadeAnimation.DurationOut = Profile::LoadingDisplay::FadeOutDuration;

  SaveLoadBg = Profile::LoadingDisplay::SaveLoadBgAnim.Instantiate();
  SaveLoadBg.LoopMode = ALM_Loop;
  ResourceLoadBg = Profile::LoadingDisplay::ResourceLoadBgAnim.Instantiate();
  ResourceLoadBg.LoopMode = ALM_Loop;
  LoadingIcon = Profile::LoadingDisplay::LoadingIconAnim.Instantiate();
  LoadingIcon.LoopMode = ALM_Loop;
  LoadingText = Profile::LoadingDisplay::LoadingTextAnim.Instantiate();
  LoadingText.LoopMode = ALM_Loop;
}

void Update(float dt) {
  FadeAnimation.Update(dt);

  if (FadeAnimation.IsOut()) {
    if (GetFlag(SF_LOADINGFROMSAVE)) {
      FadeAnimation.StartIn();
      IsResourceLoad = false;

      SaveLoadBg.StartIn(true);
      LoadingIcon.StartIn(true);
      LoadingText.StartIn(true);
    } else if (GetFlag(SF_LOADING)) {
      FadeAnimation.StartIn();
      IsResourceLoad = true;

      ResourceLoadBg.StartIn(true);
      LoadingIcon.StartIn(true);
      LoadingText.StartIn(true);
    }
  } else if (FadeAnimation.IsIn() && !GetFlag(SF_LOADINGFROMSAVE) &&
             !GetFlag(SF_LOADING)) {
    FadeAnimation.StartOut();
  }

  if (!FadeAnimation.IsOut()) {
    LoadingIcon.Update(dt);
    LoadingText.Update(dt);
    if (IsResourceLoad) {
      ResourceLoadBg.Update(dt);
    } else {
      SaveLoadBg.Update(dt);
    }
  }
}

void Render() {
  if (FadeAnimation.IsOut()) return;

  glm::vec4 col(1.0f);
  col.a = glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress);

  if (IsResourceLoad) {
    Renderer->DrawSprite(ResourceLoadBg.CurrentSprite(),
                         Profile::LoadingDisplay::ResourceBgPos, col);
  } else {
    Renderer->DrawSprite(SaveLoadBg.CurrentSprite(),
                         Profile::LoadingDisplay::SaveBgPos, col);
  }

  Renderer->DrawSprite(LoadingIcon.CurrentSprite(),
                       Profile::LoadingDisplay::IconPos, col);
  Renderer->DrawSprite(LoadingText.CurrentSprite(),
                       Profile::LoadingDisplay::TextPos, col);
}

}  // namespace LoadingDisplay
}  // namespace Impacto