#include "titlemenu.h"

#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../inputsystem.h"
#include "../../ui/widgets/label.h"
#include "../../profile/ui/titlemenu.h"
#include "../../profile/games/mo6tw/titlemenu.h"
#include "../../profile/scriptvars.h"
#include "../../profile/game.h"
#include "../../mask2d.h"

namespace MO6TWTitleProfile = ::Impacto::Profile::MO6TW::TitleMenu;
namespace MO6TWCommonTitleProfile = ::Impacto::Profile::TitleMenu;

namespace Impacto {
namespace UI {
namespace MO6TW {

using namespace Impacto::Profile::TitleMenu;
using namespace Impacto::Profile::MO6TW::TitleMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets::MO6TW;

void TitleMenu::MenuButtonOnClick(Widgets::Button* target) {
  target->Hovered = false;
  ScrWork[SW_TITLECUR1] = target->Id;
  ChoiceMade = true;
}

void TitleMenu::SecondaryButtonOnClick(Widgets::Button* target) {
  target->Hovered = false;
  ScrWork[SW_TITLECUR2] = target->Id;
  ChoiceMade = true;
}

TitleMenu::TitleMenu() {
  MainItems = new Widgets::Group(this);
  ContinueItems = new Widgets::Group(this);
  SystemItems = new Widgets::Group(this);
  ExtraStoryItems = new Widgets::Group(this);
  MemoriesItems = new Widgets::Group(this);

  auto onClick = [this](auto* btn) { return MenuButtonOnClick(btn); };
  auto secondaryOnClick = [this](auto* btn) {
    return SecondaryButtonOnClick(btn);
  };

  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);

  // Initial Start menu button
  InitialStart = new TitleButton(
      0, MO6TWCommonTitleProfile::MenuEntriesSprites[0],
      MO6TWCommonTitleProfile::MenuEntriesHSprites[0], nullSprite,
      glm::vec2(MO6TWTitleProfile::MenuEntriesX,
                MO6TWTitleProfile::MenuEntriesFirstY));
  InitialStart->OnClickHandler = onClick;
  MainItems->Add(InitialStart, FDIR_DOWN);

  // Lockable Extra Story menu button
  ExtraStory = new TitleButton(
      1, MO6TWCommonTitleProfile::MenuEntriesSprites[1],
      MO6TWCommonTitleProfile::MenuEntriesHSprites[1], nullSprite,
      glm::vec2(MO6TWTitleProfile::MenuEntriesX,
                MO6TWTitleProfile::MenuEntriesFirstY +
                    (1 * MO6TWTitleProfile::MenuEntriesYPadding)));
  ExtraStory->Enabled = false;
  ExtraStory->OnClickHandler = onClick;
  ExtraStory->DisabledSprite = MenuItemLockedSprite;
  MainItems->Add(ExtraStory, FDIR_DOWN);

  // Continue menu button
  Continue = new TitleButton(
      2, MO6TWCommonTitleProfile::MenuEntriesSprites[2],
      MO6TWCommonTitleProfile::MenuEntriesHSprites[2], nullSprite,
      glm::vec2(MO6TWTitleProfile::MenuEntriesX,
                MO6TWTitleProfile::MenuEntriesFirstY +
                    (2 * MO6TWTitleProfile::MenuEntriesYPadding)));
  Continue->OnClickHandler = onClick;
  MainItems->Add(Continue, FDIR_DOWN);

  // Lockable Memories menu button
  Memories = new TitleButton(
      3, MO6TWCommonTitleProfile::MenuEntriesSprites[3],
      MO6TWCommonTitleProfile::MenuEntriesHSprites[3], nullSprite,
      glm::vec2(MO6TWTitleProfile::MenuEntriesX,
                MO6TWTitleProfile::MenuEntriesFirstY +
                    (3 * MO6TWTitleProfile::MenuEntriesYPadding)));
  Memories->Enabled = false;
  Memories->OnClickHandler = onClick;
  Memories->DisabledSprite = MenuItemLockedSprite;
  MainItems->Add(Memories, FDIR_DOWN);

  // Encyclopedia menu button
  Encyclopedia = new TitleButton(
      4, MO6TWCommonTitleProfile::MenuEntriesSprites[4],
      MO6TWCommonTitleProfile::MenuEntriesHSprites[4], nullSprite,
      glm::vec2(MO6TWTitleProfile::MenuEntriesX,
                MO6TWTitleProfile::MenuEntriesFirstY +
                    (4 * MO6TWTitleProfile::MenuEntriesYPadding)));
  Encyclopedia->OnClickHandler = onClick;
  MainItems->Add(Encyclopedia, FDIR_DOWN);

  // System menu button
  System = new TitleButton(
      5, MO6TWCommonTitleProfile::MenuEntriesSprites[5],
      MO6TWCommonTitleProfile::MenuEntriesHSprites[5], nullSprite,
      glm::vec2(MO6TWTitleProfile::MenuEntriesX,
                MO6TWTitleProfile::MenuEntriesFirstY +
                    (5 * MO6TWTitleProfile::MenuEntriesYPadding)));
  System->OnClickHandler = onClick;
  MainItems->Add(System, FDIR_DOWN);

  // Exit menu button
  Exit = new TitleButton(
      6, MO6TWCommonTitleProfile::MenuEntriesSprites[6],
      MO6TWCommonTitleProfile::MenuEntriesHSprites[6], nullSprite,
      glm::vec2(MO6TWTitleProfile::MenuEntriesX,
                MO6TWTitleProfile::MenuEntriesFirstY +
                    (6 * MO6TWTitleProfile::MenuEntriesYPadding)));
  Exit->OnClickHandler = onClick;
  MainItems->Add(Exit, FDIR_DOWN);

  // Load secondary Continue menu button
  Load = new TitleButton(
      0, MO6TWCommonTitleProfile::MenuEntriesSprites[10],
      MO6TWCommonTitleProfile::MenuEntriesHSprites[10], nullSprite,
      glm::vec2(MO6TWTitleProfile::MenuEntriesX,
                MO6TWTitleProfile::MenuEntriesFirstY +
                    (3 * MO6TWTitleProfile::MenuEntriesYPadding)));
  Load->OnClickHandler = secondaryOnClick;
  ContinueItems->Add(Load, FDIR_DOWN);

  // Quick Load secondary Continue menu button
  QuickLoad = new TitleButton(
      1, MO6TWCommonTitleProfile::MenuEntriesSprites[11],
      MO6TWCommonTitleProfile::MenuEntriesHSprites[11], nullSprite,
      glm::vec2(MO6TWTitleProfile::MenuEntriesX - 20.0f,
                MO6TWTitleProfile::MenuEntriesFirstY +
                    (4 * MO6TWTitleProfile::MenuEntriesYPadding)));
  QuickLoad->OnClickHandler = secondaryOnClick;
  ContinueItems->Add(QuickLoad, FDIR_DOWN);

  // Prologue secondary Extra Story menu button
  Prologue = new TitleButton(
      0, MO6TWCommonTitleProfile::MenuEntriesSprites[7],
      MO6TWCommonTitleProfile::MenuEntriesHSprites[7], nullSprite,
      glm::vec2(MO6TWTitleProfile::MenuEntriesX,
                MO6TWTitleProfile::MenuEntriesFirstY +
                    (2 * MO6TWTitleProfile::MenuEntriesYPadding)));
  Prologue->OnClickHandler = secondaryOnClick;
  ExtraStoryItems->Add(Prologue, FDIR_DOWN);

  // Otome Judge feat.Shin secondary Extra Story menu button
  OtomeJudgeShin = new TitleButton(
      1, MO6TWCommonTitleProfile::MenuEntriesSprites[8],
      MO6TWCommonTitleProfile::MenuEntriesHSprites[8], nullSprite,
      glm::vec2(MO6TWTitleProfile::MenuEntriesX - 20.0f,
                MO6TWTitleProfile::MenuEntriesFirstY +
                    (3 * MO6TWTitleProfile::MenuEntriesYPadding)));
  OtomeJudgeShin->OnClickHandler = secondaryOnClick;
  ExtraStoryItems->Add(OtomeJudgeShin, FDIR_DOWN);

  // Warning secondary Extra Story menu button
  Warning = new TitleButton(
      2, MO6TWCommonTitleProfile::MenuEntriesSprites[9],
      MO6TWCommonTitleProfile::MenuEntriesHSprites[9], nullSprite,
      glm::vec2(MO6TWTitleProfile::MenuEntriesX - 40.0f,
                MO6TWTitleProfile::MenuEntriesFirstY +
                    (4 * MO6TWTitleProfile::MenuEntriesYPadding)));
  Warning->OnClickHandler = secondaryOnClick;
  ExtraStoryItems->Add(Warning, FDIR_DOWN);

  // Warning secondary Extra Story menu button
  ClearList = new TitleButton(
      0, MO6TWCommonTitleProfile::MenuEntriesSprites[13],
      MO6TWCommonTitleProfile::MenuEntriesHSprites[13], nullSprite,
      glm::vec2(MO6TWTitleProfile::MenuEntriesX,
                MO6TWTitleProfile::MenuEntriesFirstY +
                    (4 * MO6TWTitleProfile::MenuEntriesYPadding)));
  ClearList->OnClickHandler = secondaryOnClick;
  MemoriesItems->Add(ClearList, FDIR_DOWN);

  // Warning secondary Extra Story menu button
  Album = new TitleButton(
      1, MO6TWCommonTitleProfile::MenuEntriesSprites[14],
      MO6TWCommonTitleProfile::MenuEntriesHSprites[14], nullSprite,
      glm::vec2(MO6TWTitleProfile::MenuEntriesX - 20.0f,
                MO6TWTitleProfile::MenuEntriesFirstY +
                    (5 * MO6TWTitleProfile::MenuEntriesYPadding)));
  Album->OnClickHandler = secondaryOnClick;
  MemoriesItems->Add(Album, FDIR_DOWN);

  // Warning secondary Extra Story menu button
  Music = new TitleButton(
      2, MO6TWCommonTitleProfile::MenuEntriesSprites[15],
      MO6TWCommonTitleProfile::MenuEntriesHSprites[15], nullSprite,
      glm::vec2(MO6TWTitleProfile::MenuEntriesX - 40.0f,
                MO6TWTitleProfile::MenuEntriesFirstY +
                    (6 * MO6TWTitleProfile::MenuEntriesYPadding)));
  Music->OnClickHandler = secondaryOnClick;
  MemoriesItems->Add(Music, FDIR_DOWN);

  // Warning secondary Extra Story menu button
  Movie = new TitleButton(
      3, MO6TWCommonTitleProfile::MenuEntriesSprites[16],
      MO6TWCommonTitleProfile::MenuEntriesHSprites[16], nullSprite,
      glm::vec2(MO6TWTitleProfile::MenuEntriesX - 60.0f,
                MO6TWTitleProfile::MenuEntriesFirstY +
                    (7 * MO6TWTitleProfile::MenuEntriesYPadding)));
  Movie->OnClickHandler = secondaryOnClick;
  MemoriesItems->Add(Movie, FDIR_DOWN);

  // Warning secondary Extra Story menu button
  ActorsVoice = new TitleButton(
      4, MO6TWCommonTitleProfile::MenuEntriesSprites[17],
      MO6TWCommonTitleProfile::MenuEntriesHSprites[17], nullSprite,
      glm::vec2(MO6TWTitleProfile::MenuEntriesX - 80.0f,
                MO6TWTitleProfile::MenuEntriesFirstY +
                    (8 * MO6TWTitleProfile::MenuEntriesYPadding)));
  ActorsVoice->OnClickHandler = secondaryOnClick;
  MemoriesItems->Add(ActorsVoice, FDIR_DOWN);

  // Option secondary System menu button
  Option = new TitleButton(
      0, MO6TWCommonTitleProfile::MenuEntriesSprites[18],
      MO6TWCommonTitleProfile::MenuEntriesHSprites[18], nullSprite,
      glm::vec2(MO6TWTitleProfile::MenuEntriesX,
                MO6TWTitleProfile::MenuEntriesFirstY +
                    (6 * MO6TWTitleProfile::MenuEntriesYPadding)));
  Option->OnClickHandler = secondaryOnClick;
  SystemItems->Add(Option, FDIR_DOWN);

  // System Save secondary System menu button
  SystemSave = new TitleButton(
      1, MO6TWCommonTitleProfile::MenuEntriesSprites[20],
      MO6TWCommonTitleProfile::MenuEntriesHSprites[20], nullSprite,
      glm::vec2(MO6TWTitleProfile::MenuEntriesX - 20.0f,
                MO6TWTitleProfile::MenuEntriesFirstY +
                    (7 * MO6TWTitleProfile::MenuEntriesYPadding)));
  SystemSave->OnClickHandler = secondaryOnClick;
  SystemItems->Add(SystemSave, FDIR_DOWN);
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
    if (PressToStartAnimation.State == AnimationState::Stopped)
      PressToStartAnimation.StartIn();
  }
}
void TitleMenu::Hide() {
  if (State == Shown) {
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
  UpdateInput(dt);

  PressToStartAnimation.Update(dt);
  PrimaryFadeAnimation.Update(dt);
  SecondaryFadeAnimation.Update(dt);
  if (GetFlag(SF_TITLEMODE)) {
    Show();
  } else if (ScrWork[SW_TITLECT] < 32) {
    Hide();
  }

  if (State == Shown && IsFocused) {
    ExtraStory->Enabled = GetFlag(867);
    Memories->Enabled = (GetFlag(860) || GetFlag(861) || GetFlag(862) ||
                         GetFlag(863) || GetFlag(864));

    // Do not update these when it's not the time to update them (press to
    // start portion)
    if (ScrWork[SW_TITLEDISPCT] != 1 && ScrWork[SW_TITLEDISPCT] != 20 &&
        ScrWork[SW_TITLEDISPCT] != 21) {
      MainItems->Tint.a =
          glm::smoothstep(0.0f, 1.0f, PrimaryFadeAnimation.Progress);
      MainItems->Update(dt);
      MainItems->UpdateInput(dt);
      ContinueItems->Tint.a =
          glm::smoothstep(0.0f, 1.0f, SecondaryFadeAnimation.Progress);
      ContinueItems->Update(dt);
      ContinueItems->UpdateInput(dt);
      SystemItems->Tint.a =
          glm::smoothstep(0.0f, 1.0f, SecondaryFadeAnimation.Progress);
      SystemItems->Update(dt);
      SystemItems->UpdateInput(dt);
      ExtraStoryItems->Tint.a =
          glm::smoothstep(0.0f, 1.0f, SecondaryFadeAnimation.Progress);
      ExtraStoryItems->Update(dt);
      ExtraStoryItems->UpdateInput(dt);
      MemoriesItems->Tint.a =
          glm::smoothstep(0.0f, 1.0f, SecondaryFadeAnimation.Progress);
      MemoriesItems->Update(dt);
      MemoriesItems->UpdateInput(dt);
    }

    switch (ScrWork[SW_TITLEDISPCT]) {
      case 0: {
        // When returning to title menu from loading a game we need to hide the
        // continue sub-menu and extra story sub-menu
        if (ContinueItems->VisibilityState != Hidden) {
          MainItems->HasFocus = true;
          SecondaryFadeAnimation.StartOut();
          Load->Move(-SecondaryMenuAnimTarget);
          QuickLoad->Move(
              glm::vec2(-(SecondaryMenuAnimTarget.x + SecondaryMenuPadding),
                        SecondaryMenuAnimTarget.y));

          Memories->Move(glm::vec2(
              SecondaryMenuAnimUnderX,
              -ContinueItemCount * MO6TWTitleProfile::MenuEntriesYPadding));
          Encyclopedia->Move(glm::vec2(
              SecondaryMenuAnimUnderX,
              -ContinueItemCount * MO6TWTitleProfile::MenuEntriesYPadding));
          System->Move(glm::vec2(
              SecondaryMenuAnimUnderX,
              -ContinueItemCount * MO6TWTitleProfile::MenuEntriesYPadding));
          Exit->Move(glm::vec2(
              SecondaryMenuAnimUnderX,
              -ContinueItemCount * MO6TWTitleProfile::MenuEntriesYPadding));
          ContinueItems->Hide();
        }
        if (ExtraStoryItems->VisibilityState != Hidden) {
          MainItems->HasFocus = true;
          SecondaryFadeAnimation.StartOut();
          Prologue->Move(-SecondaryMenuAnimTarget);
          OtomeJudgeShin->Move(
              glm::vec2(-(SecondaryMenuAnimTarget.x + SecondaryMenuPadding),
                        SecondaryMenuAnimTarget.y));
          Warning->Move(glm::vec2(
              -(SecondaryMenuAnimTarget.x + (2 * SecondaryMenuPadding)),
              SecondaryMenuAnimTarget.y));
          Continue->Move(glm::vec2(
              SecondaryMenuAnimUnderX,
              -ExtraStoryItemCount * MO6TWTitleProfile::MenuEntriesYPadding));
          Memories->Move(glm::vec2(
              SecondaryMenuAnimUnderX,
              -ExtraStoryItemCount * MO6TWTitleProfile::MenuEntriesYPadding));
          Encyclopedia->Move(glm::vec2(
              SecondaryMenuAnimUnderX,
              -ExtraStoryItemCount * MO6TWTitleProfile::MenuEntriesYPadding));
          System->Move(glm::vec2(
              SecondaryMenuAnimUnderX,
              -ExtraStoryItemCount * MO6TWTitleProfile::MenuEntriesYPadding));
          Exit->Move(glm::vec2(
              SecondaryMenuAnimUnderX,
              -ExtraStoryItemCount * MO6TWTitleProfile::MenuEntriesYPadding));
          ExtraStoryItems->Hide();
        }
      } break;
      case 3: {  // Main Menu Fade In
        if (MainItems->VisibilityState == Hidden && ScrWork[SW_TITLECT] == 0) {
          MainItems->Show();
          MainItems->Tint.a = 0.0f;
          PrimaryFadeAnimation.StartIn();
        }
      } break;
      case 5: {  // Secondary menu Extra Story Fade In
        if (ExtraStoryItems->VisibilityState != Shown &&
            ScrWork[SW_TITLECT] == 0) {
          ShowExtraStoryItems();
        } else if (ExtraStoryItems->VisibilityState != Hidden &&
                   ScrWork[SW_TITLECT] == 32) {
          HideExtraStoryItems();
        } else if (ScrWork[SW_TITLECT] == 0) {
          ExtraStoryItems->Hide();
          MainItems->HasFocus = true;
        }
      } break;
      case 7: {  // Secondary menu Continue Fade In
        if (ContinueItems->VisibilityState != Shown &&
            ScrWork[SW_TITLECT] == 0) {
          ShowContinueItems();
        } else if (ContinueItems->VisibilityState != Hidden &&
                   ScrWork[SW_TITLECT] == 32) {
          HideContinueItems();
        } else if (ScrWork[SW_TITLECT] == 0) {
          ContinueItems->Hide();
          MainItems->HasFocus = true;
        }
      } break;
      case 9: {  // Secondary menu Memories Fade In
        if (MemoriesItems->VisibilityState != Shown &&
            ScrWork[SW_TITLECT] == 0) {
          ShowMemoriesItems();
        } else if (MemoriesItems->VisibilityState != Hidden &&
                   ScrWork[SW_TITLECT] == 32) {
          HideMemoriesItems();
        } else if (ScrWork[SW_TITLECT] == 0) {
          MemoriesItems->Hide();
          MainItems->HasFocus = true;
        }
      } break;
      case 10: {
        if (MemoriesItems->VisibilityState != Hidden &&
            MainItems->VisibilityState == Hidden) {
          MainItems->VisibilityState = Shown;
        }
      } break;
      case 11: {  // Secondary menu System Fade In
        if (SystemItems->VisibilityState != Shown && ScrWork[SW_TITLECT] == 0) {
          ShowSystemItems();
        } else if (SystemItems->VisibilityState != Hidden &&
                   ScrWork[SW_TITLECT] == 32) {
          HideSystemItems();
        } else if (ScrWork[SW_TITLECT] == 0) {
          SystemItems->Hide();
          MainItems->HasFocus = true;
        }
      } break;
    }
  }
}

void TitleMenu::Render() {
  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
    if (ScrWork[SW_MENUCT] != 100) {
      switch (ScrWork[SW_TITLEDISPCT]) {
        case 0:  // Initial animation
          Renderer->DrawMaskedSprite(
              BackgroundSprite, BackgroundSprite,
              RectF(0.0f, 0.0f, BackgroundSprite.ScaledWidth(),
                    BackgroundSprite.ScaledHeight()),
              (ScrWork[SW_TITLECT] * 287) / 48, 32, glm::mat4(1.0f),
              glm::vec4(1.0f), false, true);
          if (ScrWork[SW_TITLECT] > 48) {
            Renderer->DrawMaskedSprite(
                LogoSprite, Masks2D[17].MaskSprite,
                RectF(LogoX, LogoY, LogoSprite.ScaledWidth(),
                      LogoSprite.ScaledHeight()),
                (ScrWork[SW_TITLECT] * 271 - 13008) / 32, 16);
          }
          break;
        case 1: {  // Press to start
          Renderer->DrawMaskedSprite(
              BackgroundSprite, BackgroundSprite,
              RectF(0.0f, 0.0f, BackgroundSprite.ScaledWidth(),
                    BackgroundSprite.ScaledHeight()),
              287, 32, glm::mat4(1.0f), glm::vec4(1.0f), false, true);
          Renderer->DrawSprite(LogoSprite, glm::vec2(LogoX, LogoY));
          Renderer->DrawSprite(CopyrightSprite,
                               glm::vec2(CopyrightX, CopyrightY));
          glm::vec4 col = glm::vec4(1.0f);
          col.a = glm::smoothstep(0.0f, 1.0f, PressToStartAnimation.Progress);
          Renderer->DrawSprite(PressToStartSprite, PressToStartPos, col);
        } break;
        case 2: {
          Renderer->DrawMaskedSprite(
              BackgroundSprite, BackgroundSprite,
              RectF(0.0f, 0.0f, BackgroundSprite.ScaledWidth(),
                    BackgroundSprite.ScaledHeight()),
              287, 32, glm::mat4(1.0f), glm::vec4(1.0f), false, true);
          Renderer->DrawSprite(LogoSprite, glm::vec2(LogoX, LogoY));
          Renderer->DrawSprite(CopyrightSprite,
                               glm::vec2(CopyrightX, CopyrightY));
        } break;
        case 3:    // Main Menu Fade In
        case 4: {  // Main Menu
          Renderer->DrawMaskedSprite(
              MenuBackgroundSprite, MenuBackgroundSprite,
              RectF(0.0f, 0.0f, MenuBackgroundSprite.ScaledWidth(),
                    MenuBackgroundSprite.ScaledHeight()),
              287, 32, glm::mat4(1.0f), glm::vec4(1.0f), false, true);
          MainItems->Render();
        } break;
        case 5:    // Secondary menu Extra story Fade In
        case 6: {  // Secondary menu Extra story
          Renderer->DrawMaskedSprite(
              MenuBackgroundSprite, MenuBackgroundSprite,
              RectF(0.0f, 0.0f, MenuBackgroundSprite.ScaledWidth(),
                    MenuBackgroundSprite.ScaledHeight()),
              287, 32, glm::mat4(1.0f), glm::vec4(1.0f), false, true);
          ExtraStoryItems->Render();
          MainItems->Render();
        } break;
        case 7:    // Secondary menu Continue Fade In
        case 8: {  // Secondary menu Continue
          Renderer->DrawMaskedSprite(
              MenuBackgroundSprite, MenuBackgroundSprite,
              RectF(0.0f, 0.0f, MenuBackgroundSprite.ScaledWidth(),
                    MenuBackgroundSprite.ScaledHeight()),
              287, 32, glm::mat4(1.0f), glm::vec4(1.0f), false, true);
          ContinueItems->Render();
          MainItems->Render();
        } break;
        case 9:     // Secondary menu Memories Fade In
        case 10: {  // Secondary menu Memories
          Renderer->DrawMaskedSprite(
              MenuBackgroundSprite, MenuBackgroundSprite,
              RectF(0.0f, 0.0f, MenuBackgroundSprite.ScaledWidth(),
                    MenuBackgroundSprite.ScaledHeight()),
              287, 32, glm::mat4(1.0f), glm::vec4(1.0f), false, true);
          MemoriesItems->Render();
          MainItems->Render();
        } break;
        case 11:    // Secondary menu System Fade In
        case 12: {  // Secondary menu System
          Renderer->DrawMaskedSprite(
              MenuBackgroundSprite, MenuBackgroundSprite,
              RectF(0.0f, 0.0f, MenuBackgroundSprite.ScaledWidth(),
                    MenuBackgroundSprite.ScaledHeight()),
              287, 32, glm::mat4(1.0f), glm::vec4(1.0f), false, true);
          SystemItems->Render();
          MainItems->Render();
        } break;
        case 20:
        case 21: {  // Transition between Press to start and menus
          Renderer->DrawMaskedSprite(
              BackgroundSprite, BackgroundSprite,
              RectF(0.0f, 0.0f, BackgroundSprite.ScaledWidth(),
                    BackgroundSprite.ScaledHeight()),
              287, 32, glm::mat4(1.0f), glm::vec4(1.0f), false, true);
          Renderer->DrawMaskedSprite(
              LogoSprite, Masks2D[17].MaskSprite,
              RectF(LogoX, LogoY, LogoSprite.ScaledWidth(),
                    LogoSprite.ScaledHeight()),
              287, 16);
          Renderer->DrawSprite(CopyrightSprite,
                               glm::vec2(CopyrightX, CopyrightY));
          Renderer->DrawMaskedSprite(
              MenuBackgroundSprite, MenuBackgroundSprite,
              RectF(0.0f, 0.0f, MenuBackgroundSprite.ScaledWidth(),
                    MenuBackgroundSprite.ScaledHeight()),
              (ScrWork[SW_TITLECT] * 287) / 48, 32, glm::mat4(1.0f),
              glm::vec4(1.0f), false, true);
        } break;
      }
    }

    int maskAlpha = ScrWork[SW_TITLEMASKALPHA];
    glm::vec4 col = ScrWorkGetColor(SW_TITLEMASKCOLOR);
    col.a = glm::min(maskAlpha / 255.0f, 1.0f);
    Renderer->DrawQuad(RectF(0.0f, 0.0f, Profile::Game::DesignWidth,
                             Profile::Game::DesignHeight),
                       col);
  }
}

void TitleMenu::ShowExtraStoryItems() {
  ExtraStoryItems->Show();
  ExtraStoryItems->Tint.a = 0.0f;
  MainItems->HasFocus = false;

  SecondaryFadeAnimation.StartIn();

  Prologue->Move(SecondaryMenuAnimTarget, SecondaryMenuAnimDuration);
  OtomeJudgeShin->Move(
      glm::vec2(SecondaryMenuAnimTarget.x + SecondaryMenuPadding,
                SecondaryMenuAnimTarget.y),
      SecondaryMenuAnimDuration);
  Warning->Move(
      glm::vec2(SecondaryMenuAnimTarget.x + (2 * SecondaryMenuPadding),
                SecondaryMenuAnimTarget.y),
      SecondaryMenuAnimDuration);

  Continue->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                ExtraStoryItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
  Memories->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                ExtraStoryItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
  Encyclopedia->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                ExtraStoryItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
  System->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                ExtraStoryItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
  Exit->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                ExtraStoryItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
}

void TitleMenu::HideExtraStoryItems() {
  SecondaryFadeAnimation.StartOut();

  Prologue->Move(-SecondaryMenuAnimTarget, SecondaryMenuAnimDuration);
  OtomeJudgeShin->Move(
      glm::vec2(-(SecondaryMenuAnimTarget.x + SecondaryMenuPadding),
                SecondaryMenuAnimTarget.y),
      SecondaryMenuAnimDuration);
  Warning->Move(
      glm::vec2(-(SecondaryMenuAnimTarget.x + (2 * SecondaryMenuPadding)),
                SecondaryMenuAnimTarget.y),
      SecondaryMenuAnimDuration);

  Continue->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                -ExtraStoryItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
  Memories->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                -ExtraStoryItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
  Encyclopedia->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                -ExtraStoryItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
  System->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                -ExtraStoryItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
  Exit->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                -ExtraStoryItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
}

void TitleMenu::ShowContinueItems() {
  ContinueItems->Show();
  ContinueItems->Tint.a = 0.0f;
  MainItems->HasFocus = false;

  SecondaryFadeAnimation.StartIn();

  Load->Move(SecondaryMenuAnimTarget, SecondaryMenuAnimDuration);
  QuickLoad->Move(glm::vec2(SecondaryMenuAnimTarget.x + SecondaryMenuPadding,
                            SecondaryMenuAnimTarget.y),
                  SecondaryMenuAnimDuration);

  Memories->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                ContinueItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
  Encyclopedia->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                ContinueItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
  System->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                ContinueItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
  Exit->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                ContinueItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
}

void TitleMenu::HideContinueItems() {
  SecondaryFadeAnimation.StartOut();

  Load->Move(-SecondaryMenuAnimTarget, SecondaryMenuAnimDuration);
  QuickLoad->Move(glm::vec2(-(SecondaryMenuAnimTarget.x + SecondaryMenuPadding),
                            SecondaryMenuAnimTarget.y),
                  SecondaryMenuAnimDuration);

  Memories->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                -ContinueItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
  Encyclopedia->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                -ContinueItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
  System->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                -ContinueItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
  Exit->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                -ContinueItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
}

void TitleMenu::ShowMemoriesItems() {
  MemoriesItems->Show();
  MemoriesItems->Tint.a = 0.0f;
  MainItems->HasFocus = false;

  SecondaryFadeAnimation.StartIn();

  ClearList->Move(SecondaryMenuAnimTarget, SecondaryMenuAnimDuration);
  Album->Move(glm::vec2(SecondaryMenuAnimTarget.x + SecondaryMenuPadding,
                        SecondaryMenuAnimTarget.y),
              SecondaryMenuAnimDuration);
  Music->Move(glm::vec2(SecondaryMenuAnimTarget.x + (2 * SecondaryMenuPadding),
                        SecondaryMenuAnimTarget.y),
              SecondaryMenuAnimDuration);
  Movie->Move(glm::vec2(SecondaryMenuAnimTarget.x + (3 * SecondaryMenuPadding),
                        SecondaryMenuAnimTarget.y),
              SecondaryMenuAnimDuration);
  ActorsVoice->Move(
      glm::vec2(SecondaryMenuAnimTarget.x + (4 * SecondaryMenuPadding),
                SecondaryMenuAnimTarget.y),
      SecondaryMenuAnimDuration);

  Encyclopedia->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                MemoriesItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
  System->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                MemoriesItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
  Exit->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                MemoriesItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
}

void TitleMenu::HideMemoriesItems() {
  SecondaryFadeAnimation.StartOut();

  ClearList->Move(-SecondaryMenuAnimTarget, SecondaryMenuAnimDuration);
  Album->Move(glm::vec2(-(SecondaryMenuAnimTarget.x + SecondaryMenuPadding),
                        SecondaryMenuAnimTarget.y),
              SecondaryMenuAnimDuration);
  Music->Move(
      glm::vec2(-(SecondaryMenuAnimTarget.x + (2 * SecondaryMenuPadding)),
                SecondaryMenuAnimTarget.y),
      SecondaryMenuAnimDuration);
  Movie->Move(
      glm::vec2(-(SecondaryMenuAnimTarget.x + (3 * SecondaryMenuPadding)),
                SecondaryMenuAnimTarget.y),
      SecondaryMenuAnimDuration);
  ActorsVoice->Move(
      glm::vec2(-(SecondaryMenuAnimTarget.x + (4 * SecondaryMenuPadding)),
                SecondaryMenuAnimTarget.y),
      SecondaryMenuAnimDuration);

  Encyclopedia->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                -MemoriesItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
  System->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                -MemoriesItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
  Exit->Move(
      glm::vec2(SecondaryMenuAnimUnderX,
                -MemoriesItemCount * MO6TWTitleProfile::MenuEntriesYPadding),
      SecondaryMenuAnimDuration);
}

void TitleMenu::ShowSystemItems() {
  SystemItems->Show();
  SystemItems->Tint.a = 0.0f;
  MainItems->HasFocus = false;

  SecondaryFadeAnimation.StartIn();

  Option->Move(SecondaryMenuAnimTarget, SecondaryMenuAnimDuration);
  SystemSave->Move(glm::vec2(SecondaryMenuAnimTarget.x + SecondaryMenuPadding,
                             SecondaryMenuAnimTarget.y),
                   SecondaryMenuAnimDuration);

  Exit->Move(glm::vec2(SecondaryMenuAnimUnderX,
                       SystemItemCount *
                           Profile::MO6TW::TitleMenu::MenuEntriesYPadding),
             SecondaryMenuAnimDuration);
}

void TitleMenu::HideSystemItems() {
  SecondaryFadeAnimation.StartOut();

  Option->Move(-SecondaryMenuAnimTarget, SecondaryMenuAnimDuration);
  SystemSave->Move(
      glm::vec2(-(SecondaryMenuAnimTarget.x + SecondaryMenuPadding),
                SecondaryMenuAnimTarget.y),
      SecondaryMenuAnimDuration);

  Exit->Move(glm::vec2(SecondaryMenuAnimUnderX,
                       -SystemItemCount *
                           Profile::MO6TW::TitleMenu::MenuEntriesYPadding),
             SecondaryMenuAnimDuration);
}

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto