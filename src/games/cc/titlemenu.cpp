#include "titlemenu.h"

#include "../../texture/texture.h"
#include "../../spritesheet.h"
#include "../../io/vfs.h"

#include "../../profile/ui/titlemenu.h"
#include "../../profile/games/cc/titlemenu.h"
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
#include "../../inputsystem.h"
#include "../../ui/widgets/label.h"
#include "../../vm/interface/input.h"

namespace Impacto {
namespace UI {
namespace CC {

using namespace Impacto::Profile::TitleMenu;
using namespace Impacto::Profile::CC::TitleMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets::CC;

void TitleMenu::MenuButtonOnClick(Widgets::Button* target) {
  ScrWork[SW_TITLECUR1] = target->Id;
  SetFlag(SF_TITLEEND, 1);
  AllowsScriptInput = true;
}

void TitleMenu::ContinueButtonOnClick(Widgets::Button* target) {
  CurrentSubMenu = ContinueItems;
  AllowsScriptInput = false;
}

void TitleMenu::ExtraButtonOnClick(Widgets::Button* target) {
  CurrentSubMenu = ExtraItems;
  AllowsScriptInput = false;
}

TitleMenu::TitleMenu() {
  MainItems = new Widgets::Group(this);
  ContinueItems = new Widgets::Group(this);
  ContinueItems->WrapFocus = false;
  ExtraItems = new Widgets::Group(this);
  ExtraItems->WrapFocus = false;

  auto onClick =
      std::bind(&TitleMenu::MenuButtonOnClick, this, std::placeholders::_1);
  auto continueOnClick =
      std::bind(&TitleMenu::ContinueButtonOnClick, this, std::placeholders::_1);
  auto extraOnClick =
      std::bind(&TitleMenu::ExtraButtonOnClick, this, std::placeholders::_1);

  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);

  // NewGame menu button
  NewGame = new TitleButton(
      0, MenuEntriesSprites[0], MenuEntriesHSprites[0], ItemHighlightSprite,
      glm::vec2(((ItemHighlightOffsetX)-1.0f) + ItemHighlightOffsetX,
                (ItemYBase + (0 * ItemPadding))));
  NewGame->OnClickHandler = onClick;
  MainItems->Add(NewGame, FDIR_DOWN);

  // Continue menu button
  Continue = new TitleButton(
      1, MenuEntriesSprites[1], MenuEntriesHSprites[1], ItemHighlightSprite,
      glm::vec2(((ItemHighlightOffsetX)) + ItemHighlightOffsetX,
                (ItemYBase + (1 * ItemPadding))));
  Continue->OnClickHandler = continueOnClick;
  MainItems->Add(Continue, FDIR_DOWN);

  // Extra menu button
  Extra = new TitleButton(
      2, MenuEntriesSprites[2], MenuEntriesHSprites[2], ItemHighlightSprite,
      glm::vec2(((ItemHighlightOffsetX)) + ItemHighlightOffsetX,
                (ItemYBase + (2 * ItemPadding))));
  Extra->OnClickHandler = extraOnClick;
  MainItems->Add(Extra, FDIR_DOWN);

  // Config menu button
  Config = new TitleButton(
      20, MenuEntriesSprites[3], MenuEntriesHSprites[3], ItemHighlightSprite,
      glm::vec2(((ItemHighlightOffsetX)) + ItemHighlightOffsetX,
                (ItemYBase + (3 * ItemPadding))));
  Config->OnClickHandler = onClick;
  MainItems->Add(Config, FDIR_DOWN);

  // Help menu button
  Help = new TitleButton(
      40, MenuEntriesSprites[4], MenuEntriesHSprites[4], ItemHighlightSprite,
      glm::vec2(((ItemHighlightOffsetX)) + ItemHighlightOffsetX,
                (ItemYBase + (4 * ItemPadding))));
  Help->OnClickHandler = onClick;
  MainItems->Add(Help, FDIR_DOWN);

  // Load secondary Continue menu button
  Load = new TitleButton(10, LoadSprite, LoadHighlightSprite, nullSprite,
                         glm::vec2(((SecondaryFirstItemHighlightOffsetX)) +
                                       SecondaryFirstItemHighlightOffsetX,
                                   (ItemYBase + (2 * ItemPadding))));
  Load->OnClickHandler = onClick;
  Load->IsSubButton = true;
  ContinueItems->Add(Load, FDIR_RIGHT);

  // QuickLoad secondary Continue menu button
  QuickLoad =
      new TitleButton(11, QuickLoadSprite, QuickLoadHighlightSprite, nullSprite,
                      glm::vec2(((SecondarySecondItemHighlightOffsetX)) +
                                    SecondarySecondItemHighlightOffsetX,
                                (ItemYBase + (2 * ItemPadding))));
  QuickLoad->OnClickHandler = onClick;
  QuickLoad->IsSubButton = true;
  ContinueItems->Add(QuickLoad, FDIR_RIGHT);

  // Tips secondary Extra menu button
  Tips = new TitleButton(30, TipsSprite, TipsHighlightSprite, nullSprite,
                         glm::vec2(((SecondaryFirstItemHighlightOffsetX)) +
                                       SecondaryFirstItemHighlightOffsetX,
                                   (ItemYBase + (3 * ItemPadding))));
  Tips->OnClickHandler = onClick;
  Tips->IsSubButton = true;
  ExtraItems->Add(Tips, FDIR_RIGHT);

  // Library secondary Extra menu button
  Library =
      new TitleButton(31, LibrarySprite, LibraryHighlightSprite, nullSprite,
                      glm::vec2(((SecondarySecondItemHighlightOffsetX)) +
                                    SecondarySecondItemHighlightOffsetX,
                                (ItemYBase + (3 * ItemPadding))));
  Library->OnClickHandler = onClick;
  Library->IsSubButton = true;
  ExtraItems->Add(Library, FDIR_RIGHT);

  // EndingList secondary Extra menu button
  EndingList = new TitleButton(
      32, EndingListSprite, EndingListHighlightSprite, nullSprite,
      glm::vec2(((SecondaryThirdItemHighlightOffsetX)) +
                    SecondaryThirdItemHighlightOffsetX,
                (ItemYBase + (3 * ItemPadding))));
  EndingList->OnClickHandler = onClick;
  EndingList->IsSubButton = true;
  ExtraItems->Add(EndingList, FDIR_RIGHT);
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
    AllowsScriptInput = true;
    if (PressToStartAnimation.State == AS_Stopped) {
      PressToStartAnimation.StartIn();
      SmokeAnimation.StartIn();
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
    AllowsScriptInput = true;
  }
}

void TitleMenu::UpdateInput() {
  Menu::UpdateInput();
  if (CurrentSubMenu) {
    if ((PADinputButtonWentDown & PAD1B || PADinputMouseWentDown & PAD1B) &&
        CurrentSubMenu->IsShown) {
      if (CurrentSubMenu == ContinueItems) {
        // HideContinueItems();
      }
      if (CurrentSubMenu == ExtraItems) {
        // HideExtraItems();
      }
      AllowsScriptInput = true;
    }
  }
}

void TitleMenu::Update(float dt) {
  UpdateInput();

  PressToStartAnimation.Update(dt);
  SmokeAnimation.Update(dt);
  MoveLeftAnimation.Update(dt);
  if (GetFlag(SF_TITLEMODE)) {
    Show();
  } else {
    Hide();
  }

  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
    switch (ScrWork[SW_TITLEMODE]) {
      case 1: {
        PressToStartAnimation.DurationIn = PressToStartAnimDurationIn;
        PressToStartAnimation.DurationOut = PressToStartAnimDurationOut;
      } break;
      case 2: {
        PressToStartAnimation.DurationIn = PressToStartAnimFastDurationIn;
        PressToStartAnimation.DurationOut = PressToStartAnimFastDurationOut;
      } break;
      case 3: {
        MainItems->Update(dt);
        ContinueItems->Update(dt);
        ExtraItems->Update(dt);
        if (MoveLeftAnimation.IsOut() && ScrWork[SW_TITLEDISPCT] > 0) {
          MoveLeftAnimation.StartIn();
        } else if (MoveLeftAnimation.IsIn() && ScrWork[SW_TITLEDISPCT] < 90) {
          MoveLeftAnimation.StartOut();
        }
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
        DrawMainBackground(true);
        DrawStartButton();
        Renderer->DrawSprite(
            OverlaySprite,
            RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight));
        DrawSmoke(SmokeOpacityNormal);
        Renderer->DrawRect(
            RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
            glm::vec4(1.0f, 1.0f, 1.0f,
                      1.0f - ScrWork[SW_TITLEDISPCT] / 60.0f));
      } break;
      case 2: {  // Transition between Press to start and menus
        DrawMainBackground(true);
        DrawStartButton();
        Renderer->DrawSprite(
            OverlaySprite,
            RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight));
        DrawSmoke(SmokeOpacityNormal);
      } break;
      case 3: {  // Main Menu Fade In
        float backgroundBoundsY = BackgroundBoundsYNormal,
              fenceBoundsY = FenceBoundsYNormal;
        if (GetFlag(SF_CLR_TRUE_CC)) {
          backgroundBoundsY = BackgroundBoundsYTrue;
          fenceBoundsY = FenceBoundsYTrue;
        }
        BackgroundSprite.Bounds = RectF(
            BackgroundBoundsX -
                (BackgroundBoundsX * MoveLeftAnimation.Progress),
            backgroundBoundsY, BackgroundBoundsWidth, BackgroundBoundsHeight);
        FenceSprite.Bounds =
            RectF(FenceBoundsX - (FenceBoundsX * MoveLeftAnimation.Progress),
                  fenceBoundsY, FenceBoundsWidth, FenceBoundsHeight);
        Renderer->DrawSprite(BackgroundSprite,
                               glm::vec2(BackgroundX, BackgroundY));
        Renderer->DrawSprite(FenceSprite, glm::vec2(FenceX, FenceY));
        Renderer->DrawSprite(
            CopyrightSprite,
            glm::vec2(CopyrightX +
                          (CopyrightXMoveOffset * MoveLeftAnimation.Progress),
                      CopyrightY));
        Renderer->DrawSprite(
            OverlaySprite,
            RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight));
        Renderer->DrawSprite(MenuSprite, glm::vec2(MenuX, MenuY));
        if (!GetFlag(SF_CLR_TRUE_CC)) {
          DrawSmoke(SmokeOpacityNormal);
        }
        MainItems->Render();
        ContinueItems->Render();
        ExtraItems->Render();
      } break;
      case 4: {
      } break;
      case 7:
      case 8: {
      } break;
      case 11: {  // Initial Fade In
        DrawMainBackground(ScrWork[SW_TITLEDISPCT] / 32.0f);
        Renderer->DrawSprite(
            OverlaySprite,
            RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight));
        DrawSmoke(ScrWork[SW_TITLEDISPCT] / 128.0f);
      } break;
      case 12: {
      } break;
    }
  }
}

void TitleMenu::DrawMainBackground(float opacity) {
  glm::vec4 col = glm::vec4(1.0f);
  col.a = opacity;
  Renderer->DrawSprite(BackgroundSprite, glm::vec2(BackgroundX, BackgroundY),
                         col);
  Renderer->DrawSprite(FenceSprite, glm::vec2(FenceX, FenceY), col);
  Renderer->DrawSprite(CopyrightSprite, glm::vec2(CopyrightX, CopyrightY),
                         col);
}

void TitleMenu::DrawStartButton() {
  glm::vec4 col = glm::vec4(1.0f);
  col.a = glm::smoothstep(0.0f, 1.0f, PressToStartAnimation.Progress);
  Renderer->DrawSprite(PressToStartSprite,
                         glm::vec2(PressToStartX, PressToStartY), col);
}

void TitleMenu::DrawSmoke(float opacity) {
  glm::vec4 col = glm::vec4(1.0f);
  col.a = opacity;
  SmokeSprite.Bounds = RectF(
      SmokeBoundsWidth - (SmokeAnimationBoundsXMax * SmokeAnimation.Progress) +
          SmokeAnimationBoundsXOffset,
      SmokeBoundsY,
      SmokeBoundsWidth -
          (SmokeAnimationBoundsXMax * (1.0f - SmokeAnimation.Progress)),
      SmokeBoundsHeight);
  Renderer->DrawSprite(SmokeSprite, glm::vec2(SmokeX, SmokeY), col);
  SmokeSprite.Bounds = RectF(
      SmokeBoundsX, SmokeBoundsY,
      SmokeBoundsWidth - (SmokeAnimationBoundsXMax * SmokeAnimation.Progress),
      SmokeBoundsHeight);
  Renderer->DrawSprite(
      SmokeSprite,
      glm::vec2(SmokeBoundsWidth - (SmokeAnimationBoundsXMax *
                                    (1.0f - SmokeAnimation.Progress)),
                SmokeY),
      col);
}

}  // namespace CC
}  // namespace UI
}  // namespace Impacto