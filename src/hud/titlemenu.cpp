#include "titlemenu.h"

#include "../profile/hud/titlemenu.h"
#include "../renderer2d.h"
#include "../games/rne/tilebackground.h"
#include "../audio/audiosystem.h"
#include "../audio/audiostream.h"
#include "../audio/audiochannel.h"
#include "../io/vfs.h"
#include "../mem.h"
#include "../scriptvars.h"

namespace Impacto {
namespace TitleMenu {

using namespace Impacto::Profile::TitleMenu;

TitleMenuState State = Hidden;

Animation* BackgroundAnimation = 0;
static Animation PreTitleItemsAnimation;
static Animation PressToStartAnimation;

void Init() {
  Configure();
  PreTitleItemsAnimation.DurationIn = PreTitleAnimDurationIn;
  PreTitleItemsAnimation.DurationOut = PreTitleAnimDurationOut;
  PressToStartAnimation.DurationIn = PressToStartAnimDurationIn;
  PressToStartAnimation.DurationOut = PressToStartAnimDurationOut;
  PressToStartAnimation.LoopMode = ALM_ReverseDirection;
}

void Show() {
  if (State != Shown) {
    if (BackgroundAnimation) BackgroundAnimation->StartIn();
    State = Showing;
  }
}
void Hide() {
  if (State != Hidden) {
    if (BackgroundAnimation) BackgroundAnimation->StartOut();
    State = Hiding;
  }
}
void Update(float dt) {
  if (BackgroundAnimation) BackgroundAnimation->Update(dt);
  PreTitleItemsAnimation.Update(dt);
  PressToStartAnimation.Update(dt);

  if (State == Showing) {
    if (BackgroundAnimation->IsIn()) {
      if (PreTitleItemsAnimation.IsOut()) PreTitleItemsAnimation.StartIn();
      if (PreTitleItemsAnimation.IsIn() &&
          PressToStartAnimation.State == AS_Stopped)
        PressToStartAnimation.StartIn();
    }
    if (PreTitleItemsAnimation.IsIn()) State = Shown;

    LineSprite.Bounds.Width = (LineWidth * PreTitleItemsAnimation.Progress);
    CopyrightSprite.Bounds.Width =
        (CopyrightWidth * PreTitleItemsAnimation.Progress);
    LogoSprite.Bounds.Width = (LogoWidth * PreTitleItemsAnimation.Progress);
    EliteSprite.Bounds.Height = (EliteHeight * PreTitleItemsAnimation.Progress);

  } else if (State == Hiding) {
    if (BackgroundAnimation->IsOut()) State = Hidden;
  }
}
void Render() {
  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
    if (BackgroundAnimation) BackgroundAnimation->Render();
    if (BackgroundAnimation->IsIn()) {
      glm::vec4 col = glm::vec4(1.0f);
      col.a = glm::smoothstep(0.0f, 1.0f, PressToStartAnimation.Progress);
      Renderer2D::DrawSprite(PressToStartSprite,
                             glm::vec2(PressToStartX, PressToStartY), col);

      Renderer2D::DrawSprite(LineSprite, glm::vec2(LineX, LineY),
                             glm::vec4(1.0f));
      Renderer2D::DrawSprite(CopyrightSprite, glm::vec2(CopyrightX, CopyrightY),
                             glm::vec4(1.0f));
      Renderer2D::DrawSprite(EliteSprite, glm::vec2(EliteX, EliteY),
                             glm::vec4(1.0f));
      Renderer2D::DrawSprite(LogoSprite, glm::vec2(LogoX, LogoY),
                             glm::vec4(1.0f));
    }
  }
}

}  // namespace TitleMenu
}  // namespace Impacto