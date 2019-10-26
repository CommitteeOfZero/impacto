#include "titlemenu.h"

#include "../../profile/hud/titlemenu.h"
#include "../../profile/games/chlcc/titlemenu.h"
#include "../../renderer2d.h"
#include "../../texture/texture.h"
#include "../../spritesheet.h"
#include "../../io/vfs.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../background2d.h"

namespace Impacto {
namespace CHLCC {

using namespace Impacto::Profile::TitleMenu;
using namespace Impacto::Profile::CHLCC::TitleMenu;
using namespace Impacto::Profile::ScriptVars;

Animation SpinningCircleAnimation;

void TitleMenu::Show() {
  if (State != Shown) {
    State = Showing;
  }
}
void TitleMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
  }
}
void TitleMenu::Update(float dt) {
  PressToStartAnimation.Update(dt);
  SpinningCircleAnimation.Update(dt);

  if (ScrWork[1014] == 1 && State != Shown) {
    State = Showing;
  } else if (ScrWork[1014] == 0 && State != Hidden) {
    State = Hidden;
  }

  if (State == Showing) {
    if (PressToStartAnimation.State == AS_Stopped)
      PressToStartAnimation.StartIn();
    if (SpinningCircleAnimation.State == AS_Stopped) {
      SpinningCircleAnimation.LoopMode = ALM_Loop;
      SpinningCircleAnimation.DurationIn = 15.0f;
      SpinningCircleAnimation.DurationOut = 15.0f;
      SpinningCircleAnimation.StartIn();
    }
  }
}
void TitleMenu::Render() {
  if (State != Hidden && GetFlag(SF_TITLEMODE) && ScrWork[1014] >= 1) {
    Renderer2D::DrawSprite(BackgroundSprite, glm::vec2(0.0f));
    if (ScrWork[1014] == 1) {
      glm::vec4 col = glm::vec4(1.0f);
      col.a = glm::smoothstep(0.0f, 1.0f, PressToStartAnimation.Progress);
      Renderer2D::DrawSprite(PressToStartSprite,
                             glm::vec2(PressToStartX, PressToStartY), col);
    }

    Renderer2D::DrawSprite(SpinningCircleSprite,
                           glm::vec2(SpinningCircleX, SpinningCircleY),
                           glm::vec4(1.0f), glm::vec2(2.0f),
                           -SpinningCircleAnimation.Progress * 2.0f * M_PI);
    Renderer2D::DrawSprite(DelusionADVUnderSprite,
                           glm::vec2(DelusionADVUnderX, DelusionADVUnderY));
    Renderer2D::DrawSprite(DelusionADVSprite,
                           glm::vec2(DelusionADVX, DelusionADVY));
    Renderer2D::DrawSprite(SeiraUnderSprite,
                           glm::vec2(SeiraUnderX, SeiraUnderY));
    Renderer2D::DrawSprite(SeiraSprite, glm::vec2(SeiraX, SeiraY));
    Renderer2D::DrawSprite(CHLogoSprite, glm::vec2(CHLogoX, CHLogoY));
    Renderer2D::DrawSprite(LCCLogoUnderSprite,
                           glm::vec2(LCCLogoUnderX, LCCLogoUnderY));
    Renderer2D::DrawSprite(ChuLeftLogoSprite,
                           glm::vec2(ChuLeftLogoX, ChuLeftLogoY));
    Renderer2D::DrawSprite(ChuRightLogoSprite,
                           glm::vec2(ChuRightLogoX, ChuRightLogoY));
    Renderer2D::DrawSprite(LoveLogoSprite, glm::vec2(LoveLogoX, LoveLogoY));
    Renderer2D::DrawSprite(StarLogoSprite, glm::vec2(StarLogoX, StarLogoY));
    Renderer2D::DrawSprite(ExclMarkLogoSprite,
                           glm::vec2(ExclMarkLogoX, ExclMarkLogoY));
    Renderer2D::DrawSprite(CopyrightTextSprite,
                           glm::vec2(CopyrightTextX, CopyrightTextY));
  }
}

}  // namespace CHLCC
}  // namespace Impacto