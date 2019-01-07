#include "mainmenu.h"

#include "../../profile/hud/mainmenu.h"
#include "../../profile/games/rne/mainmenu.h"
#include "../../renderer2d.h"
#include "../../games/rne/tilebackground.h"
#include "../../mem.h"
#include "../../scriptvars.h"

namespace Impacto {
namespace RNE {

using namespace Impacto::Profile::MainMenu;
using namespace Impacto::Profile::RNE::MainMenu;

void MainMenu::Show() {
  if (State != Shown) {
    if (BackgroundAnimation) BackgroundAnimation->StartIn();
    State = Showing;
  }
}
void MainMenu::Hide() {
  if (State != Hidden) {
    HighlightAnimation.StartOut();
    State = Hiding;
  }
}
void MainMenu::Update(float dt) {
  SkyMoveAnimation.Update(dt);
  EntriesMoveAnimation.Update(dt);
  HighlightAnimation.Update(dt);
  if (BackgroundAnimation) BackgroundAnimation->Update(dt);

  if (State == Showing) {
    if (BackgroundAnimation->Progress >= SkyInStartProgress) {
      if (SkyMoveAnimation.IsOut()) SkyMoveAnimation.StartIn();
      if (EntriesMoveAnimation.IsOut()) EntriesMoveAnimation.StartIn();
    }
    if (SkyMoveAnimation.IsIn()) {
      if (HighlightAnimation.IsOut()) HighlightAnimation.StartIn();
    }
    if (HighlightAnimation.IsIn()) State = Shown;
  } else if (State == Hiding) {
    if (HighlightAnimation.IsOut()) {
      if (BackgroundAnimation && BackgroundAnimation->IsIn())
        BackgroundAnimation->StartOut();
    }
    if (BackgroundAnimation->Progress <= SkyOutStartProgress) {
      if (SkyMoveAnimation.IsIn()) SkyMoveAnimation.StartOut();
      if (EntriesMoveAnimation.IsIn()) EntriesMoveAnimation.StartOut();
    }
    if (BackgroundAnimation->IsOut()) State = Hidden;
  }
  ButtonBackgroundSprite.Bounds.X =
      ButtonBackgroundSprStartX -
      (ButtonBackgroundTargetWidth * SkyMoveAnimation.Progress);
  ButtonBackgroundSprite.Bounds.Width =
      ButtonBackgroundTargetWidth * SkyMoveAnimation.Progress;

  MenuEntriesHSprites[0].Bounds.Width =
      MenuEntriesTargetWidth * HighlightAnimation.Progress;
}
void MainMenu::Render() {
  if (State != Hidden && ScrWork[SW_SYSMENUALPHA] > 0) {
    if (BackgroundAnimation) BackgroundAnimation->Render();
    glm::vec4 colSky(1.0f);
    colSky.a = glm::smoothstep(0.0f, 1.0f, SkyMoveAnimation.Progress);

    glm::vec2 destSky = glm::vec2(
        SkyBackgroundBeginX +
            (SkyMoveAnimation.Progress * std::abs(SkyBackgroundBeginX)),
        SkyBackgroundY);
    Renderer2D::DrawSprite(SkyBackgroundSprite, destSky, colSky);
    Renderer2D::DrawSprite(SkyArrowSprite, destSky, colSky);

    Renderer2D::DrawSprite(
        SkyTextSprite,
        glm::vec2(SkyTextBeginX + (SkyMoveAnimation.Progress *
                                   std::abs(SkyBackgroundBeginX)),
                  SkyTextY),
        colSky);

    Renderer2D::DrawSprite(
        ButtonBackgroundSprite,
        glm::vec2(ButtonBackgroundStartX -
                      (ButtonBackgroundStartX * SkyMoveAnimation.Progress),
                  ButtonBackgroundY),
        glm::vec4(1.0f));

    glm::vec4 black(0.0f);
    black.a = glm::smoothstep(0.0f, 1.0f, EntriesMoveAnimation.Progress);
    for (int i = 0; i < MenuEntriesNum; i++) {
      black.a = glm::smoothstep(
          0.0f, 1.0f, 1.0f - (i + 1) * (1.0f - EntriesMoveAnimation.Progress));
      Renderer2D::DrawSprite(
          MenuEntriesSprites[i],
          glm::vec2(((i * MenuEntriesXSkew) + MenuEntriesXOffset) *
                        (1.0f - EntriesMoveAnimation.Progress),
                    MenuEntriesFirstY + (i * MenuEntriesYPadding)),
          black);
    }

    if (SkyMoveAnimation.IsIn()) {
      Renderer2D::DrawSprite(ButtonPromptsSprite,
                             glm::vec2(ButtonBackgroundX, ButtonBackgroundY),
                             glm::vec4(1.0f));
      Renderer2D::DrawSprite(MenuEntriesHSprites[0],
                             glm::vec2(MenuEntriesX, MenuEntriesFirstY));
    }
  }
}

}  // namespace RNE
}  // namespace Impacto