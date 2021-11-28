#include "titlemenu.h"

#include "../../texture/texture.h"
#include "../../spritesheet.h"
#include "../../io/vfs.h"

#include "../../renderer2d.h"
#include "../../mem.h"
#include "../../inputsystem.h"
#include "../../ui/widgets/label.h"
#include "../../profile/ui/titlemenu.h"
#include "../../profile/games/cclcc/titlemenu.h"
#include "../../profile/scriptvars.h"
#include "../../profile/game.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

using namespace Impacto::Profile::TitleMenu;
using namespace Impacto::Profile::CCLCC::TitleMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets::CCLCC;

void TitleMenu::MenuButtonOnClick(Widgets::Button* target) {
  ScrWork[SW_TITLECUR1] = target->Id;
  ChoiceMade = true;
}

void TitleMenu::SecondaryButtonOnClick(Widgets::Button* target) {
  ScrWork[SW_TITLECUR2] = target->Id;
  ChoiceMade = true;
}

TitleMenu::TitleMenu() {
  MainItems = new Widgets::Group(this);
  ContinueItems = new Widgets::Group(this);
  ExtraItems = new Widgets::Group(this);

  auto onClick =
      std::bind(&TitleMenu::MenuButtonOnClick, this, std::placeholders::_1);
  auto secondaryOnClick = std::bind(&TitleMenu::SecondaryButtonOnClick, this,
                                    std::placeholders::_1);

  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);

  // NewGame menu button
  NewGame = new TitleButton(
      0, MenuEntriesSprites[0], MenuEntriesHSprites[0], ItemHighlightSprite,
      glm::vec2(((ItemHighlightOffsetX * ItemsFadeInAnimation.Progress)) +
                    ItemHighlightOffsetX,
                ((ItemYBase - 1.0f) + (0 * ItemPadding))));
  NewGame->OnClickHandler = onClick;
  MainItems->Add(NewGame, FDIR_DOWN);

  // Continue menu button
  Continue = new TitleButton(
      1, MenuEntriesSprites[1], MenuEntriesHSprites[1], ItemHighlightSprite,
      glm::vec2(((ItemHighlightOffsetX * ItemsFadeInAnimation.Progress)) +
                    ItemHighlightOffsetX,
                ((ItemYBase - 1.0f) + (1 * ItemPadding))));
  Continue->OnClickHandler = onClick;
  MainItems->Add(Continue, FDIR_DOWN);

  // Extra menu button
  Extra = new TitleButton(
      2, MenuEntriesSprites[2], MenuEntriesHSprites[2], ItemHighlightSprite,
      glm::vec2(((ItemHighlightOffsetX * ItemsFadeInAnimation.Progress)) +
                    ItemHighlightOffsetX,
                ((ItemYBase - 1.0f) + (2 * ItemPadding))));
  Extra->OnClickHandler = onClick;
  MainItems->Add(Extra, FDIR_DOWN);

  // Config menu button
  Config = new TitleButton(
      3, MenuEntriesSprites[3], MenuEntriesHSprites[3], ItemHighlightSprite,
      glm::vec2(((ItemHighlightOffsetX * ItemsFadeInAnimation.Progress)) +
                    ItemHighlightOffsetX,
                ((ItemYBase - 1.0f) + (3 * ItemPadding))));
  Config->OnClickHandler = onClick;
  MainItems->Add(Config, FDIR_DOWN);

  // Help menu button
  Help = new TitleButton(
      4, MenuEntriesSprites[4], MenuEntriesHSprites[4], ItemHighlightSprite,
      glm::vec2(((ItemHighlightOffsetX * ItemsFadeInAnimation.Progress)) +
                    ItemHighlightOffsetX,
                ((ItemYBase - 1.0f) + (4 * ItemPadding))));
  Help->OnClickHandler = onClick;
  MainItems->Add(Help, FDIR_DOWN);
}

void TitleMenu::Show() {
  if (State != Shown) {
    State = Shown;
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
    if (PressToStartAnimation.State == AS_Stopped) {
      PressToStartAnimation.StartIn();
      SpinningCircleAnimation.StartIn();
    }
  }
}
void TitleMenu::Hide() {
  if (State != Hidden) {
    State = Hidden;
    MainItems->Hide();
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void TitleMenu::Update(float dt) {
  UpdateInput();

  PressToStartAnimation.Update(dt);
  PrimaryFadeAnimation.Update(dt);
  SecondaryFadeAnimation.Update(dt);

  if (GetFlag(SF_TITLEMODE)) {
    Show();
  } else {
    Hide();
  }

  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
    MainItems->Update(dt);
    /*MainItems->Tint.a =
        glm::smoothstep(0.0f, 1.0f, PrimaryFadeAnimation.Progress);
    ContinueItems->Tint.a =
        glm::smoothstep(0.0f, 1.0f, SecondaryFadeAnimation.Progress);
    ContinueItems->Update(dt);
    ExtraItems->Tint.a =
        glm::smoothstep(0.0f, 1.0f, SecondaryFadeAnimation.Progress);
    ExtraItems->Update(dt);*/

    switch (ScrWork[SW_TITLEMODE]) {
      case 3: {  // Main Menu Fade In
        if (!MainItems->IsShown) {
          MainItems->Show();
          CurrentlyFocusedElement = NewGame;
          NewGame->HasFocus = true;
        }
      } break;
    }
  }
}

void TitleMenu::Render() {
  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
    switch (ScrWork[SW_TITLEMODE]) {
      case 1: {  // Press to start
        DrawTitleMenuBackGraphics();
        glm::vec4 col = glm::vec4(1.0f);
        col.a = glm::smoothstep(0.0f, 1.0f, PressToStartAnimation.Progress);
        Renderer2D::DrawSprite(PressToStartSprite,
                               glm::vec2(PressToStartX, PressToStartY), col);
      } break;
      case 2: {  // Transition between Press to start and menus
        DrawTitleMenuBackGraphics();
      } break;
      case 3: {  // MenuItems Fade In
        if (ItemsFadeInAnimation.IsOut() &&
            ItemsFadeInAnimation.State != AS_Playing)
          ItemsFadeInAnimation.StartIn();
        else if (ItemsFadeInAnimation.State != AS_Playing)
          ItemsFadeInAnimation.StartOut();
        DrawMainMenuBackGraphics();
        MainItems->Render();
      } break;
      case 4: {  // Main Menu
        DrawTitleMenuBackGraphics();
        MainItems->Render();
      } break;
      case 11: {  // Initial Fade In
        DrawTitleMenuBackGraphics();
      } break;
    }

    int maskAlpha = ScrWork[SW_TITLEMASKALPHA];
    glm::vec4 col = ScrWorkGetColor(SW_TITLEMASKCOLOR);
    col.a = glm::min(maskAlpha / 255.0f, 1.0f);
    Renderer2D::DrawRect(
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight), col);
  }
}

inline void TitleMenu::DrawTitleMenuBackGraphics() {
  Renderer2D::DrawSprite(BackgroundSprite, glm::vec2(0.0f));
  Renderer2D::DrawSprite(CopyrightTextSprite,
                         glm::vec2(CopyrightTextX, CopyrightTextY));
  glm::vec4 col = glm::vec4(1.0f);
  col.a = glm::smoothstep(0.0f, 1.0f, PressToStartAnimation.Progress);
  Renderer2D::DrawSprite(PressToStartSprite,
                         glm::vec2(PressToStartX, PressToStartY), col);
}

inline void TitleMenu::DrawMainMenuBackGraphics() {
  Renderer2D::DrawSprite(MainBackgroundSprite, glm::vec2(0.0f));
  Renderer2D::DrawSprite(MenuSprite, glm::vec2(MenuX, MenuY));
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto