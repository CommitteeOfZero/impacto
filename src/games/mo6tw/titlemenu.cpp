#include "titlemenu.h"

#include "../../profile/hud/titlemenu.h"
#include "../../profile/games/mo6tw/titlemenu.h"
#include "../../renderer2d.h"
#include "../../texture/texture.h"
#include "../../spritesheet.h"
#include "../../io/vfs.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../profile/game.h"
#include "../../inputsystem.h"

namespace Impacto {
namespace MO6TW {

using namespace Impacto::Profile::TitleMenu;
using namespace Impacto::Profile::MO6TW::TitleMenu;
using namespace Impacto::Profile::ScriptVars;

int SecondaryChoiceCount = 0;

void TitleMenu::Show() {
  if (State != Shown) {
    State = Shown;
  }
}
void TitleMenu::Hide() {
  if (State != Hidden) {
    State = Hidden;
  }
}
void TitleMenu::Update(float dt) {
  PressToStartAnimation.Update(dt);
  if (GetFlag(SF_TITLEMODE)) {
    Show();
  } else {
    Hide();
  }

  if (State == Shown) {
    if (PressToStartAnimation.State == AS_Stopped)
      PressToStartAnimation.StartIn();
  }

  if (Input::KeyboardButtonWentDown[SDL_SCANCODE_DOWN]) {
    if (ScrWork[SW_TITLEDISPCT] >= 7) {
      SecondaryChoice++;
      if (SecondaryChoice > SecondaryChoiceCount) SecondaryChoice = 0;
    } else {
      CurrentChoice++;
      if (CurrentChoice > 6) CurrentChoice = 0;
    }
  } else if (Input::KeyboardButtonWentDown[SDL_SCANCODE_UP]) {
    if (ScrWork[SW_TITLEDISPCT] >= 7) {
      SecondaryChoice--;
      if (SecondaryChoice < 0) SecondaryChoice = SecondaryChoiceCount;
    } else {
      CurrentChoice--;
      if (CurrentChoice < 0) CurrentChoice = 6;
    }
  }
}
void TitleMenu::Render() {
  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
    int maskAlpha = ScrWork[SW_TITLEMASKALPHA];
    glm::vec4 col = ScrWorkGetColor(SW_TITLEMASKCOLOR);
    col.a = glm::min(maskAlpha / 255.0f, 1.0f);
    Renderer2D::DrawRect(
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight), col);

    if (ScrWork[SW_MENUCT] != 100) {
      switch (ScrWork[SW_TITLEDISPCT]) {
        case 0:  // Initial animation
          break;
        case 1: {  // Press to start
          DrawTitleMenuBackGraphics();
          glm::vec4 col = glm::vec4(1.0f);
          col.a = glm::smoothstep(0.0f, 1.0f, PressToStartAnimation.Progress);
          Renderer2D::DrawSprite(PressToStartSprite,
                                 glm::vec2(PressToStartX, PressToStartY), col);
        } break;
        case 2: {  // Transition between Press to start and menus
        } break;
        case 3: {  // MenuItems Fade In
          DrawTitleMenuItems();
        } break;
        case 4: {  // Main Menu
          DrawTitleMenuItems();
        } break;
        case 7: {  // Secondary menu LOAD Fade In
          DrawTitleMenuItems();
        } break;
        case 8: {  // Secondary menu LOAD
          DrawTitleMenuItemsLoad();
          DrawTitleMenuItems();
        } break;
        case 9: {  // Secondary menu EXTRAS Fade In
          DrawTitleMenuItems();
        } break;
        case 10: {  // Secondary menu EXTRAS
          DrawTitleMenuItems();
        } break;
        case 11: {  // Secondary menu SYSTEM Fade In
          DrawTitleMenuItems();
        } break;
        case 12: {  // Secondary menu SYSTEM
          DrawTitleMenuItems();
        } break;
      }
    }
  }
}

void TitleMenu::DrawTitleMenuBackGraphics() {
  Renderer2D::DrawSprite(BackgroundSprite, glm::vec2(0.0f));
  Renderer2D::DrawSprite(LogoSprite, glm::vec2(LogoX, LogoY));
  Renderer2D::DrawSprite(CopyrightSprite, glm::vec2(CopyrightX, CopyrightY));
}

void TitleMenu::DrawTitleMenuItems() {
  Renderer2D::DrawSprite(MenuBackgroundSprite, glm::vec2(0.0f));
  glm::vec4 black(0.0f);
  black.a = 1.0f;
  for (int i = 0; i < MenuEntriesNum; i++) {
    if ((i == 1 && !GetFlag(867)) ||
        (i == 3 && !(GetFlag(860) || GetFlag(861) || GetFlag(862) ||
                     GetFlag(863) || GetFlag(864)))) {
      Renderer2D::DrawSprite(
          MenuItemLockedSprite,
          glm::vec2(MenuEntriesX,
                    MenuEntriesFirstY + (i * MenuEntriesYPadding) + 1.0f),
          black);
      Renderer2D::DrawSprite(
          MenuItemLockedSprite,
          glm::vec2(MenuEntriesX,
                    MenuEntriesFirstY + (i * MenuEntriesYPadding)));
    } else {
      Renderer2D::DrawSprite(
          MenuEntriesSprites[i],
          glm::vec2(MenuEntriesX,
                    MenuEntriesFirstY + (i * MenuEntriesYPadding) + 1.0f),
          black);
      Renderer2D::DrawSprite(
          MenuEntriesSprites[i],
          glm::vec2(MenuEntriesX,
                    MenuEntriesFirstY + (i * MenuEntriesYPadding)));
    }
  }

  if ((CurrentChoice == 1 && !GetFlag(867)) ||
      (CurrentChoice == 3 && !(GetFlag(860) || GetFlag(861) || GetFlag(862) ||
                               GetFlag(863) || GetFlag(864)))) {
    Renderer2D::DrawSprite(
        MenuItemLockedSpriteH,
        glm::vec2(MenuEntriesX,
                  MenuEntriesFirstY + (CurrentChoice * MenuEntriesYPadding)));
  } else {
    Renderer2D::DrawSprite(
        MenuEntriesHSprites[CurrentChoice],
        glm::vec2(MenuEntriesX,
                  MenuEntriesFirstY + (CurrentChoice * MenuEntriesYPadding)));
  }
}

void TitleMenu::DrawTitleMenuItemsLoad() {}
void TitleMenu::DrawTitleMenuItemsExtras() {}
void TitleMenu::DrawTitleMenuItemsSystem() {}

}  // namespace MO6TW
}  // namespace Impacto