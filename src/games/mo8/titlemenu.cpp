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
}

TitleMenu::TitleMenu() {
  MainItems = new Widgets::Group(this);

  auto onClick =
      std::bind(&TitleMenu::MenuButtonOnClick, this, std::placeholders::_1);

  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);

  // New Game menu button
  NewGame = new Button(0, MenuEntriesSprites[NewGameSpriteIndex],
                       MenuEntriesHSprites[NewGameSpriteIndex],
                       MenuEntriesHSprites[NewGameSpriteIndex + 1],
                       glm::vec2(MenuEntriesX, MenuEntriesFirstY));
  NewGame->OnClickHandler = onClick;
  MainItems->Add(NewGame, FDIR_DOWN);
  // Continue menu button
  Continue = new Button(
      0, MenuEntriesSprites[ContinueSpriteIndex],
      MenuEntriesHSprites[ContinueSpriteIndex],
      MenuEntriesHSprites[NewGameSpriteIndex + 1],
      glm::vec2(MenuEntriesX, MenuEntriesFirstY + (1 * MenuEntriesYPadding)));
  Continue->OnClickHandler = onClick;
  MainItems->Add(Continue, FDIR_DOWN);
  // Options menu button
  Options = new Button(
      20, MenuEntriesSprites[OptionSpriteIndex],
      MenuEntriesHSprites[OptionSpriteIndex],
      MenuEntriesHSprites[NewGameSpriteIndex + 1],
      glm::vec2(MenuEntriesX, MenuEntriesFirstY + (2 * MenuEntriesYPadding)));
  Options->OnClickHandler = onClick;
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
      case 2: {
        MainItems->Show();
      } break;
    }
  }
}

void TitleMenu::Render() {
  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
    Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f));
    Renderer->DrawSprite(LogoSprite, glm::vec2(LogoPositionX, LogoPositionY));
    switch (ScrWork[SW_TITLEMODE]) {
      case 1: {  // Press to start
        glm::vec4 col = glm::vec4(1.0f);
        if (PressToStartAnimated) {
          col.a = glm::smoothstep(0.0f, 1.0f, PressToStartAnimation.Progress);
        }
        Renderer->DrawSprite(PressToStartSprite,
                             glm::vec2(PressToStartX, PressToStartY), col);
      } break;
      case 5: {
        MainItems->Render();
      } break;
    }
  }
}

}  // namespace MO8
}  // namespace UI
}  // namespace Impacto