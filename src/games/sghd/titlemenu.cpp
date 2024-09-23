#include "titlemenu.h"

#include <functional>

#include "../../spritesheet.h"

#include "../../profile/ui/titlemenu.h"
#include "../../profile/games/sghd/titlemenu.h"
#include "../../spritesheet.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../vm/interface/input.h"

namespace Impacto {
namespace UI {
namespace SGHD {

using namespace Impacto::Profile::TitleMenu;
using namespace Impacto::Profile::SGHD::TitleMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::Vm::Interface;

static glm::vec2 MainMenuButtonPos(int id) {
  // Buttons are arranged top-to-bottom in one column
  // All buttons are the same size
  const RectF bounds = MenuEntriesSprites[0].Bounds;
  const float offsetY = (bounds.Height + MenuEntriesSpacingY) * id;
  return glm::vec2(MenuX, MenuY + offsetY);
}

void TitleMenu::MenuButtonOnClick(Widgets::Button* target) {
  ScrWork[SW_TITLECUR1] = target->Id;
  SetFlag(SF_TITLEEND, 1);
  AllowsScriptInput = true;
}

TitleMenu::TitleMenu() {
  MainItems = new Widgets::Group(this, glm::vec2(MenuX, MenuY));
  MainItems->Hide();

  auto onClick =
      std::bind(&TitleMenu::MenuButtonOnClick, this, std::placeholders::_1);

  // Main menu buttons

  // Steins;Gate main menu buttons don't distinguish between "highlighted" and
  // "focused". Therefore, both are passed as the same.

  // Start new game button
  int id = MenuStartId;
  Start = new TitleButton(id, MenuEntriesSprites[id], MenuEntriesHSprites[id],
                          MenuEntriesHSprites[id], MainMenuButtonPos(id));
  Start->OnClickHandler = onClick;
  MainItems->Add(Start, FDIR_DOWN);

  // Load menu button
  id = MenuLoadId;
  Load = new TitleButton(id, MenuEntriesSprites[id], MenuEntriesHSprites[id],
                         MenuEntriesHSprites[id], MainMenuButtonPos(id));
  // TODO: Replace w/ proper handler
  Load->OnClickHandler = onClick;
  MainItems->Add(Load, FDIR_DOWN);

  // Extras menu button
  id = MenuExtraId;
  Extra = new TitleButton(id, MenuEntriesSprites[id], MenuEntriesHSprites[id],
                          MenuEntriesHSprites[id], MainMenuButtonPos(id));
  // TODO: Replace w/ proper handler
  Extra->OnClickHandler = onClick;
  MainItems->Add(Extra, FDIR_DOWN);

  // Config menu button
  id = MenuConfigId;
  Config = new TitleButton(id, MenuEntriesSprites[id], MenuEntriesHSprites[id],
                           MenuEntriesHSprites[id], MainMenuButtonPos(id));
  // TODO: Replace w/ proper handler
  Config->OnClickHandler = onClick;
  MainItems->Add(Config, FDIR_DOWN);

  // Help menu button
  id = MenuHelpId;
  Help = new TitleButton(id, MenuEntriesSprites[id], MenuEntriesHSprites[id],
                         MenuEntriesHSprites[id], MainMenuButtonPos(id));
  // TODO: Replace w/ proper handler
  Help->OnClickHandler = onClick;
  MainItems->Add(Help, FDIR_DOWN);

  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);
  PressToStartAnimation = Animation();
  PressToStartAnimation.DurationIn =
      Profile::TitleMenu::PressToStartAnimDurationIn;
  PressToStartAnimation.DurationOut =
      Profile::TitleMenu::PressToStartAnimDurationOut;
  PressToStartAnimation.LoopMode = ALM_ReverseDirection;
  PressToStartAnimation.StartIn();
  PressToStartAnimation = Animation();
  PressToStartAnimation.DurationIn =
      Profile::TitleMenu::PressToStartAnimDurationIn;
  PressToStartAnimation.DurationOut =
      Profile::TitleMenu::PressToStartAnimDurationOut;
  PressToStartAnimation.LoopMode = ALM_ReverseDirection;
  // TODO: Remove
  PressToStartAnimation.StartIn();
}

void TitleMenu::Show() {
  if (State != Shown) {
    State = Shown;
    MainItems->Show();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
    AllowsScriptInput = true;
    // The "Press ENTER" animation only plays before menu is shown
    if (PressToStartAnimation.State == AS_Playing) {
      PressToStartAnimation.LoopMode = ALM_Stop;
    }
  }
}

void TitleMenu::Hide() {
  if (State != Hidden) {
    State = Hidden;
    MainItems->Hide();
    if (LastFocusedMenu != nullptr) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
    AllowsScriptInput = true;
    if (PressToStartAnimation.State == AS_Stopped) {
      PressToStartAnimation.StartIn();
    }
  }
}

void TitleMenu::UpdateInput() {
  Menu::UpdateInput();
  switch (State) {
    case Hidden: {
      // Any input should fade in the main title menu buttons
      // TODO: ESC (and maybe (B)) should display quit game sysmesbox instead
      if (PADinputMouseWentDown || PADinputButtonWentDown) {
        TitleMenu::Show();
      }
    } break;
    case Shown: {
      // Return to the "Press ENTER" screen
      if (PADinputButtonWentDown & PAD1B) {
        TitleMenu::Hide();
        return;
      }
    }
    default: {
      // TODO: During transitionary states, we should probably not accept any
      // input (or cancel the transition, but that'd be more complicated)
    } break;
  }
}

void TitleMenu::Update(float dt) {
  UpdateInput();

  PressToStartAnimation.Update(dt);
  MainItems->Update(dt);

  /*
    if (GetFlag(SF_TITLEMODE)) {
      Show();
    } else {
      Hide();
    }


    // These are copied from C;C, unsure if correct
    if (State != Hidden && GetFlag(SF_TITLEMODE)) {
      switch (ScrWork[SW_TITLEMODE]) {
        case 1: {  // Press to start
          PressToStartAnimation.DurationIn = PressToStartAnimDurationIn;
          PressToStartAnimation.DurationOut = PressToStartAnimDurationOut;
        } break;
        case 2: {  // Transition between Press to start and menus
          PressToStartAnimation.DurationIn = PressToStartAnimFastDurationIn;
          PressToStartAnimation.DurationOut = PressToStartAnimFastDurationOut;
        } break;
        default: {
          ImpLog(LL_Warning, LC_Render, "Unknown SW_TITLEMODE %i",
    SW_TITLEMODE); } break;
      }
    }
    */
  if (GetFlag(SF_TITLEMODE)) {
    Show();
  } else {
    Hide();
  }
  PressToStartAnimation.Update(dt);

  if (GetFlag(SF_TITLEMODE)) {
    Show();
  } else {
    Hide();
  }

  // These are copied from C;C, unsure if correct
  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
    switch (ScrWork[SW_TITLEMODE]) {
      case 1: {  // Press to start
        PressToStartAnimation.DurationIn = PressToStartAnimDurationIn;
        PressToStartAnimation.DurationOut = PressToStartAnimDurationOut;
      } break;
      case 2: {  // Transition between Press to start and menus
        PressToStartAnimation.DurationIn = PressToStartAnimFastDurationIn;
        PressToStartAnimation.DurationOut = PressToStartAnimFastDurationOut;
      } break;
      default: {
        ImpLog(LL_Warning, LC_Render, "Unknown SW_TITLEMODE %i", SW_TITLEMODE);
      } break;
    }
  }
}

void TitleMenu::Render() {
  DrawMainBackground(1.0f);
  DrawStartButton();
  MainItems->Render();
}

inline void TitleMenu::DrawMainBackground(float opacity) {
  glm::vec4 col = glm::vec4(1.0f);
  col.a = 1.0f;
  Renderer->DrawSprite(BackgroundSprite, glm::vec2(BackgroundX, BackgroundY),
                       col);
}

inline void TitleMenu::DrawStartButton() {
  glm::vec4 col = glm::vec4(1.0f);
  col.a = glm::smoothstep(0.0f, 1.0f, PressToStartAnimation.Progress);
  Renderer->DrawSprite(PressToStartSprite,
                       glm::vec2(PressToStartX, PressToStartY), col);
}

}  // namespace SGHD
}  // namespace UI
}  // namespace Impacto
