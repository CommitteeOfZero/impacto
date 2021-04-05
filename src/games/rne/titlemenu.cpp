#include "titlemenu.h"

#include "../../profile/ui/titlemenu.h"
#include "../../profile/games/rne/titlemenu.h"
#include "../../renderer2d.h"
#include "../../texture/texture.h"
#include "../../spritesheet.h"
#include "../../games/rne/tilebackground.h"
#include "../../audio/audiosystem.h"
#include "../../audio/audiostream.h"
#include "../../audio/audiochannel.h"
#include "../../io/vfs.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../background2d.h"

namespace Impacto {
namespace UI {
namespace RNE {

using namespace Impacto::Profile::TitleMenu;
using namespace Impacto::Profile::RNE::TitleMenu;
using namespace Impacto::Profile::ScriptVars;

static int const TitleBgBufferId = 3;

TitleMenu::TitleMenu() {}

void TitleMenu::Show() {
  if (State == Hidden) {
    if (BackgroundAnimation) {
      Impacto::RNE::TileBackground* background =
          (Impacto::RNE::TileBackground*)BackgroundAnimation;
      Backgrounds2D[TitleBgBufferId].BgSprite.BaseScale =
          glm::vec2(1280.0f / 960.0f, 720.0f / 544.0f);
      background->BackgroundSprite = Backgrounds2D[TitleBgBufferId].BgSprite;
      BackgroundAnimation->StartIn();
    }
    State = Showing;
  }
}
void TitleMenu::Hide() {
  if (State == Shown) {
    if (BackgroundAnimation) BackgroundAnimation->StartOut();
    State = Hiding;
  }
}
void TitleMenu::Update(float dt) {
  if (BackgroundAnimation) BackgroundAnimation->Update(dt);
  PreTitleItemsAnimation.Update(dt);
  PressToStartAnimation.Update(dt);
  if (GetFlag(SF_TITLEMODE)) {
    Show();
  } else {
    Hide();
  }

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
void TitleMenu::Render() {
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
      if (ScrWork[SW_TITLECGNO] == 542) {
        Renderer2D::DrawSprite(LogoSprite, glm::vec2(LogoX, LogoY),
                               glm::vec4(1.0f), glm::vec2(1.0f), 0.0f, true);
      } else {
        Renderer2D::DrawSprite(LogoSprite, glm::vec2(LogoX, LogoY),
                               glm::vec4(1.0f));
      }
    }
  }
}

}  // namespace RNE
}  // namespace UI
}  // namespace Impacto