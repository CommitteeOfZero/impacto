#include "titlemenu.h"

#include "../../profile/ui/titlemenu.h"
#include "../../profile/games/rne/titlemenu.h"
#include "../../renderer/renderer.h"
#include "../../spritesheet.h"
#include "../../games/rne/tilebackground.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../background2d.h"

namespace RNETitleProfile = Impacto::Profile::RNE::TitleMenu;

namespace Impacto {
namespace UI {
namespace RNE {

using namespace Impacto::Profile::TitleMenu;
using namespace Impacto::Profile::RNE::TitleMenu;
using namespace Impacto::Profile::ScriptVars;

static int constexpr TitleBgBufferId = 3;

TitleMenu::TitleMenu() {}

void TitleMenu::Show() {
  if (State == Hidden) {
    if (RNETitleProfile::BackgroundAnimation) {
      Impacto::RNE::TileBackground* background =
          (Impacto::RNE::TileBackground*)RNETitleProfile::BackgroundAnimation;
      Backgrounds2D[TitleBgBufferId]->BgSprite.BaseScale =
          glm::vec2(1280.0f / 960.0f, 720.0f / 544.0f);
      background->BackgroundSprite = Backgrounds2D[TitleBgBufferId]->BgSprite;
      RNETitleProfile::BackgroundAnimation->StartIn();
    }
    State = Showing;
  }
}
void TitleMenu::Hide() {
  if (State == Shown) {
    if (RNETitleProfile::BackgroundAnimation)
      RNETitleProfile::BackgroundAnimation->StartOut();
    State = Hiding;
  }
}
void TitleMenu::Update(float dt) {
  UpdateInput(dt);

  if (RNETitleProfile::BackgroundAnimation)
    RNETitleProfile::BackgroundAnimation->Update(dt);
  PreTitleItemsAnimation.Update(dt);
  PressToStartAnimation.Update(dt);
  if (GetFlag(SF_TITLEMODE)) {
    Show();
  } else {
    Hide();
  }

  if (State == Showing) {
    if (RNETitleProfile::BackgroundAnimation->IsIn()) {
      if (PreTitleItemsAnimation.IsOut()) PreTitleItemsAnimation.StartIn();
      if (PreTitleItemsAnimation.IsIn() &&
          PressToStartAnimation.State == AnimationState::Stopped)
        PressToStartAnimation.StartIn();
    }
    if (PreTitleItemsAnimation.IsIn()) State = Shown;

    LineSprite.Bounds.Width = (LineWidth * PreTitleItemsAnimation.Progress);
    CopyrightSprite.Bounds.Width =
        (CopyrightWidth * PreTitleItemsAnimation.Progress);
    LogoSprite.Bounds.Width = (LogoWidth * PreTitleItemsAnimation.Progress);
    EliteSprite.Bounds.Height = (EliteHeight * PreTitleItemsAnimation.Progress);

  } else if (State == Hiding) {
    if (RNETitleProfile::BackgroundAnimation->IsOut()) State = Hidden;
  }
}
void TitleMenu::Render() {
  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
    if (RNETitleProfile::BackgroundAnimation)
      RNETitleProfile::BackgroundAnimation->Render();
    if (RNETitleProfile::BackgroundAnimation->IsIn()) {
      glm::vec4 col = glm::vec4(1.0f);
      col.a = glm::smoothstep(0.0f, 1.0f, PressToStartAnimation.Progress);
      Renderer->DrawSprite(PressToStartSprite, PressToStartPos, col);

      Renderer->DrawSprite(LineSprite, glm::vec2(LineX, LineY),
                           glm::vec4(1.0f));
      Renderer->DrawSprite(CopyrightSprite, glm::vec2(CopyrightX, CopyrightY),
                           glm::vec4(1.0f));
      Renderer->DrawSprite(EliteSprite, glm::vec2(EliteX, EliteY),
                           glm::vec4(1.0f));
      Renderer->DrawSprite(LogoSprite, {LogoX, LogoY}, glm::vec4(1.0f),
                           ScrWork[SW_TITLECGNO] == 542);
    }
  }
}

}  // namespace RNE
}  // namespace UI
}  // namespace Impacto