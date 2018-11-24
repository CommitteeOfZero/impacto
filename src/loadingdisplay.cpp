#include "loadingdisplay.h"

#include "impacto.h"
#include "renderer2d.h"
#include "game.h"
#include "mem.h"
#include "scriptvars.h"
#include "spriteanimation.h"
#include "profile/loadingdisplay.h"

namespace Impacto {
namespace LoadingDisplay {

enum LoadingAnimState { Hidden, Hiding, Showing, Shown };

static float Fade = 0.0f;

static LoadingAnimState AnimState = Hidden;
static bool IsResourceLoad = false;

static SpriteAnimation ResourceLoadBg;
static SpriteAnimation SaveLoadBg;
static SpriteAnimation LoadingIcon;
static SpriteAnimation LoadingText;

void Update(float dt) {
  if (AnimState == Hidden) {
    if (GetFlag(SF_LOADINGFROMSAVE)) {
      AnimState = Showing;
      IsResourceLoad = false;
      Fade = 0.0f;

      SaveLoadBg = Profile::LoadingDisplay::SaveLoadBgAnim.Instantiate();
      LoadingIcon = Profile::LoadingDisplay::LoadingIconAnim.Instantiate();
      LoadingText = Profile::LoadingDisplay::LoadingTextAnim.Instantiate();

    } else if (GetFlag(SF_LOADING)) {
      AnimState = Showing;
      IsResourceLoad = true;
      Fade = 0.0f;

      ResourceLoadBg =
          Profile::LoadingDisplay::ResourceLoadBgAnim.Instantiate();
      LoadingIcon = Profile::LoadingDisplay::LoadingIconAnim.Instantiate();
      LoadingText = Profile::LoadingDisplay::LoadingTextAnim.Instantiate();
    }
  } else if (AnimState == Shown && !GetFlag(SF_LOADINGFROMSAVE) &&
             !GetFlag(SF_LOADING)) {
    AnimState = Hiding;
  }

  if (AnimState != Hidden) {
    LoadingIcon.Update(dt);
    LoadingText.Update(dt);
    if (IsResourceLoad) {
      ResourceLoadBg.Update(dt);
    } else {
      SaveLoadBg.Update(dt);
    }
  }

  if (AnimState == Hiding) {
    Fade -= dt / Profile::LoadingDisplay::FadeOutDuration;
    if (Fade <= 0.0f) {
      Fade = 0.0f;
      AnimState = Hidden;
    }
  } else if (AnimState == Showing) {
    Fade += dt / Profile::LoadingDisplay::FadeInDuration;
    if (Fade >= 1.0f) {
      Fade = 1.0f;
      AnimState = Shown;
    }
  }
}

void Render() {
  if (AnimState == Hidden) return;
  if (Fade > 0.0f) {
    glm::vec4 col(1.0f);
    col.a = glm::smoothstep(0.0f, 1.0f, Fade);

    if (IsResourceLoad) {
      Renderer2D::DrawSprite(ResourceLoadBg.CurrentSprite(),
                             Profile::LoadingDisplay::ResourceBgPos, col);
    } else {
      Renderer2D::DrawSprite(SaveLoadBg.CurrentSprite(),
                             Profile::LoadingDisplay::SaveBgPos, col);
    }

    Renderer2D::DrawSprite(LoadingIcon.CurrentSprite(),
                           Profile::LoadingDisplay::IconPos, col);
    Renderer2D::DrawSprite(LoadingText.CurrentSprite(),
                           Profile::LoadingDisplay::TextPos, col);
  }
}

}  // namespace LoadingDisplay
}  // namespace Impacto