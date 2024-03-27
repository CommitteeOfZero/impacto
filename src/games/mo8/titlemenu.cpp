#include "titlemenu.h"

#include "../../profile/ui/titlemenu.h"
#include "../../profile/games/mo8/titlemenu.h"
#include "../../renderer/renderer.h"
#include "../../texture/texture.h"
#include "../../spritesheet.h"
#include "../../audio/audiosystem.h"
#include "../../audio/audiostream.h"
#include "../../audio/audiochannel.h"
#include "../../io/vfs.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../profile/game.h"
#include "../../background2d.h"

namespace Impacto {
namespace UI {
namespace MO8 {

using namespace Impacto::Profile::TitleMenu;
using namespace Impacto::Profile::MO8::TitleMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets;

void TitleMenu::MenuButtonOnClick(Widgets::Button* target) {
  ScrWork[SW_TITLECUR] = target->Id;
  SetFlag(SF_TITLEEND, 1);
  Audio::Channels[Audio::AC_SSE]->Play("sysse", 5, false, 0.0f);
}

void TitleMenu::ContinueButtonOnClick(Widgets::Button* target) {
  Audio::Channels[Audio::AC_SSE]->Play("sysse", 5, false, 0.0f);
}

TitleMenu::TitleMenu() {
  MainItems = new Widgets::Group(this);

  auto onClick =
      std::bind(&TitleMenu::MenuButtonOnClick, this, std::placeholders::_1);
  auto continueOnClick =
      std::bind(&TitleMenu::ContinueButtonOnClick, this, std::placeholders::_1);

  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);

  // New Game menu button
  NewGame = new Button(0, MenuEntriesSprites[NewGameSpriteIndex],
                       MenuEntriesHSprites[NewGameSpriteIndex],
                       MenuEntriesHSprites[NewGameSpriteIndex + 1],
                       glm::vec2(MenuEntriesX, MenuEntriesFirstY));
  NewGame->OnClickHandler = onClick;
  NewGame->HighlightOffset = glm::vec2(0.0f);
  MainItems->Add(NewGame, FDIR_DOWN);
  // Continue menu button
  Continue = new Button(
      0, MenuEntriesSprites[ContinueSpriteIndex],
      MenuEntriesHSprites[ContinueSpriteIndex],
      MenuEntriesHSprites[NewGameSpriteIndex + 1],
      glm::vec2(MenuEntriesX, MenuEntriesFirstY + (1 * MenuEntriesYPadding)));
  Continue->OnClickHandler = continueOnClick;
  Continue->HighlightOffset = glm::vec2(0.0f);
  MainItems->Add(Continue, FDIR_DOWN);
  // Options menu button
  Options = new Button(
      20, MenuEntriesSprites[OptionSpriteIndex],
      MenuEntriesHSprites[OptionSpriteIndex],
      MenuEntriesHSprites[NewGameSpriteIndex + 1],
      glm::vec2(MenuEntriesX, MenuEntriesFirstY + (2 * MenuEntriesYPadding)));
  Options->OnClickHandler = onClick;
  Options->HighlightOffset = glm::vec2(0.0f);
  MainItems->Add(Options, FDIR_DOWN);
}

void TitleMenu::Show() {
  if (State == Hidden) {
    State = Shown;
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
    if (PressToStartAnimated && PressToStartAnimation.State == AS_Stopped) {
      PressToStartAnimation.StartIn();
    }
  }
}
void TitleMenu::Hide() {
  if (State == Shown) {
    State = Hidden;
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

  PrimaryFadeAnimation.Update(dt);
  if (PressToStartAnimated) {
    PressToStartAnimation.Update(dt);
  }
  if (GetFlag(SF_TITLEMODE)) {
    Show();
  } else {
    Hide();
  }

  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
    MainItems->Update(dt);

    switch (ScrWork[SW_TITLEMODE]) {
      case 0:
      case 2: {
        if (PrimaryFadeAnimation.State == AS_Stopped &&
            ScrWork[SW_TITLEDISPCT] == 0)
          PrimaryFadeAnimation.StartOut(true);
      } break;
      case 5: {
        if (!MainItems->IsShown) MainItems->Show();
      } break;
      case 6: {
        if (PrimaryFadeAnimation.State == AS_Stopped &&
            ScrWork[SW_TITLEDISPCT] == 0)
          PrimaryFadeAnimation.StartIn(true);
      } break;
    }
  }
}

void TitleMenu::Render() {
  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
    Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f));
    Renderer->DrawSprite(LogoSprite, glm::vec2(LogoPositionX, LogoPositionY));
    switch (ScrWork[SW_TITLEMODE]) {
      case 0:
      case 1: {  // Press to start
        glm::vec4 col = glm::vec4(1.0f);
        if (PressToStartAnimated) {
          col.a = glm::smoothstep(0.0f, 1.0f, PressToStartAnimation.Progress);
        }
        Renderer->DrawSprite(PressToStartSprite,
                             glm::vec2(PressToStartX, PressToStartY), col);
        glm::vec4 black = glm::vec4(0.0f);
        black.a = glm::smoothstep(0.0f, 1.0f, PrimaryFadeAnimation.Progress);
        Renderer->DrawRect(
            RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
            black);
      } break;
      case 2: {  // Press to start fade
        glm::vec4 col = glm::vec4(1.0f);
        col.a = glm::smoothstep(0.0f, 1.0f, PrimaryFadeAnimation.Progress);
        Renderer->DrawSprite(PressToStartSprite,
                             glm::vec2(PressToStartX, PressToStartY), col);
      } break;
      case 5:
      case 6:
      case 7: {  // Main and main fade out
        MainItems->Render();
        glm::vec4 black = glm::vec4(0.0f);
        black.a = glm::smoothstep(0.0f, 1.0f, PrimaryFadeAnimation.Progress);
        Renderer->DrawRect(
            RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
            black);
      } break;
    }
  }
}

}  // namespace MO8
}  // namespace UI
}  // namespace Impacto