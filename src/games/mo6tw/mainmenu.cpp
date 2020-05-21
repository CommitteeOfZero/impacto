#include "mainmenu.h"

#include "../../profile/hud/mainmenu.h"
#include "../../profile/games/mo6tw/mainmenu.h"
#include "../../renderer2d.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../inputsystem.h"

namespace Impacto {
namespace MO6TW {

using namespace Impacto::Profile::MainMenu;
using namespace Impacto::Profile::MO6TW::MainMenu;
using namespace Impacto::Profile::ScriptVars;

int CurrentChoice = 0;

void MainMenu::Show() {
  if (State != Shown) {
    State = Showing;
  }
}
void MainMenu::Hide() {
  if (State != Hidden) {
    State = Hidden;
  }
}
void MainMenu::Update(float dt) {
  if (GetFlag(SF_TITLEMODE)) {
    Show();
  } else {
    Hide();
  }
  if (ScrWork[SW_SYSMENUALPHA] == 0 && State == Shown) {
    Hide();
  }

  if (ScrWork[SW_SYSMENUALPHA] == 256) State = Shown;

  if (State == Shown) {
    if (Input::KeyboardButtonWentDown[SDL_SCANCODE_DOWN])
      CurrentChoice++;
    else if (Input::KeyboardButtonWentDown[SDL_SCANCODE_UP])
      CurrentChoice--;
    if (CurrentChoice < 0)
      CurrentChoice = 6;
    else if (CurrentChoice > 6)
      CurrentChoice = 0;
    ScrWork[SW_SYSMENUCNO] = CurrentChoice;
  }
}
void MainMenu::Render() {
  if (State != Hidden && ScrWork[SW_SYSMENUALPHA] > 0) {
    Renderer2D::DrawSprite(MainMenuBackgroundSprite,
                           glm::vec2(MainMenuX, MainMenuY));
    Renderer2D::DrawSprite(
        MenuEntriesHighlightedSprite,
        glm::vec2(
            MenuEntriesX + 8.0f,
            MenuEntriesFirstY + (CurrentChoice * MenuEntriesYPadding) + 3.0f),
        glm::vec4(1.0f),
        glm ::vec2(MenuEntriesSprites[0].ScaledWidth() /
                       MenuEntriesHighlightedSprite.ScaledWidth(),
                   1.0f));
    // for (int i = 0; i < 3; i++) {
    //  Renderer2D::DrawSprite(
    //      MenuEntriesHighlightedSprite,
    //      glm::vec2(MenuEntriesX + i * 76 + 17,
    //                MenuEntriesFirstY + (CurrentChoice *
    //                MenuEntriesYPadding)));
    //}
    for (int i = 0; i < MenuEntriesNum; i++) {
      Renderer2D::DrawSprite(
          MenuEntriesSprites[i],
          glm::vec2(MenuEntriesX,
                    MenuEntriesFirstY + (i * MenuEntriesYPadding)));
    }
  }
}

}  // namespace MO6TW
}  // namespace Impacto