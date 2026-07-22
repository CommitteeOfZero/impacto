#include "titlemenu.h"

#include "../../spritesheet.h"

#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../inputsystem.h"
#include "../../ui/widgets/label.h"
#include "../../profile/ui/titlemenu.h"
#include "../../profile/games/cclcc/titlemenu.h"
#include "../../profile/scriptvars.h"
#include "../../profile/game.h"
#include "../../profile/patch.h"
#include "../../profile/userconfig.h"
#include "../../vm/interface/input.h"
#include "../../audio/audiosystem.h"
#include "../../profile/scriptinput.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

using namespace Impacto::Profile::TitleMenu;
using namespace Impacto::Profile::CCLCC::TitleMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile;

using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets::CCLCC;

enum class TitleMenuState {
  InitialFadeIn,
  PressToStart,
  Exploding,
  MainMenu,
  ReturnToMenu,
  SubMenu,
};

static TitleMenuState LookupTitleMenuState(int scriptState) {
  ImpLogSlow(LogLevel::Trace, LogChannel::General, "title menu state: {}\n",
             scriptState);
  if (Profile::UserConfig::ActiveGame == "cclcc-switch") {
    switch (scriptState) {
      case 2:
        return TitleMenuState::PressToStart;
      case 3:
        return TitleMenuState::Exploding;
      case 9:
      case 10:
        return TitleMenuState::MainMenu;
      case 12:
        return TitleMenuState::ReturnToMenu;
      case 5:
      case 13:
        return TitleMenuState::SubMenu;
      case 18:
        return TitleMenuState::InitialFadeIn;
    }
  } else {
    switch (scriptState) {
      case 1:
        return TitleMenuState::PressToStart;
      case 2:
        return TitleMenuState::Exploding;
      case 3:
        return TitleMenuState::MainMenu;
      case 4:
        return TitleMenuState::ReturnToMenu;
      case 5:
      case 13:
        return TitleMenuState::SubMenu;
      case 11:
        return TitleMenuState::InitialFadeIn;
    }
  }
  ImpLog(LogLevel::Error, LogChannel::General, "Unknown title menu state: {}\n",
         scriptState);
  return TitleMenuState::InitialFadeIn;
}

void TitleMenu::MenuButtonOnClick(Widgets::Button* target) {
  TitleButton* button = static_cast<TitleButton*>(target);
  Audio::PlayInGroup(Audio::ACG_SE, "sysse", 2, false, 0);
  button->ChoiceBlinkAnimation.StartIn();
  AllowsScriptInput = false;
  InputLocked = true;
}

void TitleMenu::ContinueButtonOnClick(Widgets::Button* target) {
  target->Hovered = false;
  if (CurrentSubMenu == ContinueItems) return;
  Audio::PlayInGroup(Audio::ACG_SE, "sysse", 2, false, 0);
  CurrentSubMenu = ContinueItems;
  CurrentSubMenu->HasFocus = true;
  AllowsScriptInput = false;
  InputLocked = true;
  ShowContinueItems();
}

void TitleMenu::ExtraButtonOnClick(Widgets::Button* target) {
  target->Hovered = false;
  if (CurrentSubMenu == ExtraItems) return;
  // SetFlag(SF_CLR_FLAG, true); // Uncomment for testing
  if (!GetFlag(SF_CLR_FLAG)) {
    Audio::PlayInGroup(Audio::ACG_SE, "sysse", 4, false, 0);
    return;
  } else {
    Audio::PlayInGroup(Audio::ACG_SE, "sysse", 2, false, 0);
  }
  CurrentSubMenu = ExtraItems;
  CurrentSubMenu->HasFocus = true;
  AllowsScriptInput = false;
  InputLocked = true;
  ShowExtraItems();
}

void TitleMenu::ExitButtonOnClick(Widgets::Button* target) {
  target->Hovered = false;
  Audio::PlayInGroup(Audio::ACG_SE, "sysse", 2, false, 0);
  AllowsScriptInput = false;
  InputLocked = true;
  Input::KeyboardButtonWentDown[SDL_SCANCODE_ESCAPE] = true;
}

TitleMenu::TitleMenu() {
  MainItems = new Widgets::Group(this);
  ContinueItems = new Widgets::Group(this);
  ContinueItems->WrapFocus = false;
  ExtraItems = new Widgets::Group(this);
  ExtraItems->WrapFocus = false;

  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);

  MenuLabel = new Widgets::Label(MenuSprite, glm::vec2(MenuX, MenuY));

  auto onClick = [this](Widgets::Button* target) {
    return MenuButtonOnClick(target);
  };

  auto btnOnAnimComplete = [this](Widgets::Button* target) {
    TitleButton* button = static_cast<TitleButton*>(target);
    button->ChoiceBlinkAnimation.StartOut();
    ScrWork[SW_TITLECUR1] = target->Id;
    SetFlag(SF_TITLEEND, true);
    AllowsScriptInput = true;
    PADinputButtonWentDown &= ~PADcustom[6];
    IsFocused = false;
  };

  auto setupBtn = [&](TitleButton* btn, auto onClickHandler,
                      Widgets::Group* parent, Impacto::UI::FocusDirection dir) {
    btn->OnClickHandler = onClickHandler;
    btn->OnClickAnimCompleteHandler = btnOnAnimComplete;
    btn->IsSubButton = parent != MainItems;
    parent->Add(btn, dir);
  };

  // NewGame menu button
  NewGame = new TitleButton(NewButtonId, MenuEntriesSprites[0],
                            MenuEntriesHSprites[0], ItemHighlightSprite,
                            glm::vec2((-1.0f) + ItemHighlightOffsetX,
                                      (ItemYBase + (0 * ItemPadding))));
  setupBtn(NewGame, onClick, MainItems, FDIR_DOWN);

  // Continue menu button
  Continue = new TitleButton(
      1, MenuEntriesSprites[1], MenuEntriesHSprites[1], ItemHighlightSprite,
      glm::vec2(ItemHighlightOffsetX, (ItemYBase + (1 * ItemPadding))));
  setupBtn(
      Continue,
      [this](Widgets::Button* target) { return ContinueButtonOnClick(target); },
      MainItems, FDIR_DOWN);

  // Extra menu button
  Extra = new TitleButton(
      2, MenuEntriesSprites[2], MenuEntriesHSprites[2], ItemHighlightSprite,
      glm::vec2(ItemHighlightOffsetX, (ItemYBase + (2 * ItemPadding))));
  setupBtn(
      Extra,
      [this](Widgets::Button* target) { return ExtraButtonOnClick(target); },
      MainItems, FDIR_DOWN);

  // Config menu button
  Config = new TitleButton(
      ConfigButtonId, MenuEntriesSprites[3], MenuEntriesHSprites[3],
      ItemHighlightSprite,
      glm::vec2(ItemHighlightOffsetX, (ItemYBase + (3 * ItemPadding))));
  setupBtn(Config, onClick, MainItems, FDIR_DOWN);

  // Help menu button
  Help = new TitleButton(
      HelpButtonId, MenuEntriesSprites[4], MenuEntriesHSprites[4],
      ItemHighlightSprite,
      glm::vec2(ItemHighlightOffsetX, (ItemYBase + (4 * ItemPadding))));
  setupBtn(Help, onClick, MainItems, FDIR_DOWN);

  if (Patch::HasScriptedExitLogic) {
    // Exit menu button (Configuration/Patch driven)
    auto* const exitPtr = new TitleButton(
        5, ExitSprite, ExitSprite, ItemHighlightSprite,
        glm::vec2(ItemHighlightOffsetX, (ItemYBase + (5 * ItemPadding))));
    Exit.emplace(*exitPtr);
    setupBtn(
        exitPtr,
        [this](Widgets::Button* target) { return ExitButtonOnClick(target); },
        MainItems, FDIR_DOWN);
  }

  // Load secondary Continue menu button
  Load =
      new TitleButton(LoadButtonId, LoadSprite, LoadHighlightSprite, nullSprite,
                      glm::vec2(SecondaryFirstItemHighlightOffsetX,
                                (ItemYBase + (2 * ItemPadding))));
  setupBtn(Load, onClick, ContinueItems, FDIR_RIGHT);

  // QuickLoad secondary Continue menu button
  QuickLoad = new TitleButton(QLoadButtonId, QuickLoadSprite,
                              QuickLoadHighlightSprite, nullSprite,
                              glm::vec2(SecondarySecondItemHighlightOffsetX,
                                        (ItemYBase + (2 * ItemPadding))));
  setupBtn(QuickLoad, onClick, ContinueItems, FDIR_RIGHT);

  // Tips secondary Extra menu button
  Tips =
      new TitleButton(TipsButtonId, TipsSprite, TipsHighlightSprite, nullSprite,
                      glm::vec2(SecondaryFirstItemHighlightOffsetX,
                                (ItemYBase + (3 * ItemPadding))));
  setupBtn(Tips, onClick, ExtraItems, FDIR_RIGHT);

  // Library secondary Extra menu button
  Library = new TitleButton(LibraryButtonId, LibrarySprite,
                            LibraryHighlightSprite, nullSprite,
                            glm::vec2(SecondarySecondItemHighlightOffsetX,
                                      (ItemYBase + (3 * ItemPadding))));
  setupBtn(Library, onClick, ExtraItems, FDIR_RIGHT);

  // EndingList secondary Extra menu button
  EndingList = new TitleButton(EndingListButtonId, EndingListSprite,
                               EndingListHighlightSprite, nullSprite,
                               glm::vec2(SecondaryThirdItemHighlightOffsetX,
                                         (ItemYBase + (3 * ItemPadding))));
  setupBtn(EndingList, onClick, ExtraItems, FDIR_RIGHT);

  // Start menu items offscreen
  MainItems->Move({-Profile::Game::DesignWidth / 2, 0.0f});
  MenuLabel->Move({-Profile::Game::DesignWidth / 2, 0.0f});
  ContinueItems->Move({-Profile::Game::DesignWidth / 2, 0.0f});
  ExtraItems->Move({-Profile::Game::DesignWidth / 2, 0.0f});

  PressToStartAnimation.DurationIn = PressToStartAnimDurationIn;
  PressToStartAnimation.DurationOut = PressToStartAnimDurationOut;
  PrimaryFadeAnimation.DurationIn = PrimaryFadeInDuration;
  PrimaryFadeAnimation.DurationOut = PrimaryFadeOutDuration;
  SecondaryFadeAnimation.DurationIn = SecondaryFadeInDuration;
  SecondaryFadeAnimation.DurationOut = SecondaryFadeOutDuration;
  SmokeAnimation.DurationIn = SmokeAnimationDurationIn;
  SmokeAnimation.DurationOut = SmokeAnimationDurationOut;
  SlideItemsAnimation.DurationIn = SlideItemsAnimationDurationIn;
  SlideItemsAnimation.DurationOut = SlideItemsAnimationDurationOut;
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
    if (PressToStartAnimation.State == AnimationState::Stopped) {
      PressToStartAnimation.StartIn(true);
      SmokeAnimation.StartIn();
    }
  }
}
void TitleMenu::Hide() {
  if (State != Hidden) {
    State = Hidden;
    MainItems->Hide();
    if (SlideItemsAnimation.IsIn()) {
      SlideItemsAnimation.Progress = 0.0f;
      MainItems->Move({-Profile::Game::DesignWidth / 2, 0.0f});
      MenuLabel->Move({-Profile::Game::DesignWidth / 2, 0.0f});
      if (CurrentSubMenu) {
        CurrentSubMenu->Move({-Profile::Game::DesignWidth / 2, 0.0f});
      }
    }
    if (CurrentSubMenu) {
      CurrentSubMenu->HasFocus = false;
      if (CurrentSubMenu == ContinueItems) {
        HideContinueItems();
      } else if (CurrentSubMenu == ExtraItems) {
        HideExtraItems();
      }
    }
    if (PrimaryFadeAnimation.IsIn()) {
      PrimaryFadeAnimation.Progress = 0.0f;
    }
    if (SecondaryFadeAnimation.IsIn()) {
      SecondaryFadeAnimation.Progress = 0.0f;
    }
    MenuLabel->Hide();
    if (LastFocusedMenu != nullptr) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = nullptr;
    }
    IsFocused = false;
    AllowsScriptInput = true;
    SubMenuState = Hidden;
  }
}

void TitleMenu::UpdateInput(float dt) {
  using enum TitleMenuState;
  TitleMenuState currentState = LookupTitleMenuState(ScrWork[SW_TITLEMODE]);
  if (currentState == SubMenu || currentState == MainMenu) {
    if (!InputLocked && !PrevInputLocked) {
      if (SlideItemsAnimation.State == AnimationState::Playing ||
          SecondaryFadeAnimation.State == AnimationState::Playing ||
          PrimaryFadeAnimation.State == AnimationState::Playing ||
          TitleAnimation.State == AnimationState::Playing ||
          (CurrentlyFocusedElement != nullptr &&
           (static_cast<TitleButton*>(CurrentlyFocusedElement)
                    ->ChoiceBlinkAnimation.State == AnimationState::Playing ||
            static_cast<TitleButton*>(CurrentlyFocusedElement)
                    ->HighlightAnimation.State == AnimationState::Playing)) ||
          SubMenuState != Hidden) {
        InputLocked = true;
      }
    } else if (InputLocked && PrevInputLocked) {
      if (SlideItemsAnimation.State == AnimationState::Stopped &&
          SecondaryFadeAnimation.State == AnimationState::Stopped &&
          PrimaryFadeAnimation.State == AnimationState::Stopped &&
          TitleAnimation.State == AnimationState::Stopped &&
          (CurrentlyFocusedElement != nullptr &&
           (static_cast<TitleButton*>(CurrentlyFocusedElement)
                    ->ChoiceBlinkAnimation.State == AnimationState::Stopped &&
            static_cast<TitleButton*>(CurrentlyFocusedElement)
                    ->HighlightAnimation.State == AnimationState::Stopped)) &&
          SubMenuState == Hidden) {
        InputLocked = false;
      }
    }

    if (InputLocked != PrevInputLocked) {
      for (auto* menu : {MainItems, ContinueItems, ExtraItems}) {
        for (auto& item : menu->Children) {
          item->Enabled = !InputLocked;
        }
      }
      PrevInputLocked = InputLocked;
      if (CurrentSubMenu != nullptr) {
        CurrentSubMenu->HasFocus = !InputLocked;
      } else {
        MainItems->HasFocus = !InputLocked;
        AllowsScriptInput = !InputLocked;
      }
    }
  }

  if (InputLocked) return;

  if (CurrentSubMenu && !CurrentSubMenu->HasFocus) return;

  const bool buttonHighlightAnimationPlaying =
      CurrentlyFocusedElement &&
      static_cast<TitleButton*>(CurrentlyFocusedElement)
          ->HighlightAnimation.IsPlaying();
  if (IsFocused && !buttonHighlightAnimationPlaying) {
    Menu::UpdateInput(dt);
    if (MainItems->HasFocus && !CurrentSubMenu) {
      MainItems->UpdateInput(dt);
    }
    if (CurrentSubMenu) {
      CurrentSubMenu->UpdateInput(dt);
    }
  }

  if (CurrentSubMenu && SecondaryFadeAnimation.IsIn()) {
    if ((PADinputButtonWentDown & PAD1B || PADinputMouseWentDown & PAD1B) &&
        CurrentSubMenu->VisibilityState != Hidden && CurrentSubMenu->HasFocus) {
      Audio::PlayInGroup(Audio::ACG_SE, "sysse", 3, false, 0);
      CurrentlyFocusedElement->Enabled = false;
      SecondaryFadeAnimation.StartOut();
      if (CurrentSubMenu == ContinueItems) {
        ContinueItems->Move(glm::vec2(-Profile::Game::DesignWidth / 2, 0.0f),
                            SecondaryFadeAnimation.DurationOut);
      }
      if (CurrentSubMenu == ExtraItems) {
        ExtraItems->Move(glm::vec2(-Profile::Game::DesignWidth / 2, 0.0f),
                         SecondaryFadeAnimation.DurationOut);
      }
    }
  }
}

void TitleMenu::Update(float dt) {
  UpdateInput(dt);
  PressToStartAnimation.Update(dt);
  PrimaryFadeAnimation.Update(dt);
  SecondaryFadeAnimation.Update(dt);
  SmokeAnimation.Update(dt);
  TitleAnimation.Update(dt);
  TitleAnimationSprite.Position = {0.0f, 0.0f};
  SlideItemsAnimation.Update(dt);

  if (!IsFocused) {
    MainItems->HasFocus = false;
    ContinueItems->HasFocus = false;
    ExtraItems->HasFocus = false;
  }

  MainItems->Update(dt);
  MenuLabel->Update(dt);
  ContinueItems->Update(dt);
  ExtraItems->Update(dt);

  if (GetFlag(SF_TITLEMODE)) {
    Show();
  } else {
    Hide();
  }

  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
    using enum TitleMenuState;
    switch (LookupTitleMenuState(ScrWork[SW_TITLEMODE])) {
      case PressToStart: {
        if (PressToStartAnimation.LoopMode !=
            AnimationLoopMode::ReverseDirection) {
          PressToStartAnimation.LoopMode = AnimationLoopMode::ReverseDirection;
          PressToStartAnimation.StartOut();
        }
      } break;
      case Exploding: {
        ExplodeScreenUpdate();
      } break;
      case MainMenu: {  // Main Menu Fade In
        MainMenuUpdate();
      } break;
      case ReturnToMenu: {
        ReturnToMenuUpdate();
      } break;
      // TODO check if that's true
      case SubMenu: {
        SubMenuUpdate();
      } break;
      case InitialFadeIn:
        break;
    }
    if (SubMenuState == Hiding && ScrWork[SW_SYSSUBMENUCT] == 0) {
      SubMenuState = Hidden;
    } else if (SubMenuState == Showing && ScrWork[SW_SYSSUBMENUCT] == 32) {
      SubMenuState = Shown;
      IsFocused = true;
    }
    if (LookupTitleMenuState(ScrWork[SW_TITLEMODE]) !=
        TitleMenuState::Exploding)
      IsExploding = false;
  }
}

void TitleMenu::ExplodeScreenUpdate() {
  if (SlideItemsAnimation.IsIn()) {
    SlideItemsAnimation.Progress = 0.0f;
    MenuLabel->Move({-Profile::Game::DesignWidth / 2, 0.0f});
    MainItems->Move({-Profile::Game::DesignWidth / 2, 0.0f});
    MainItems->HasFocus = false;
  }
  if (PrimaryFadeAnimation.IsIn()) {
    PrimaryFadeAnimation.Progress = 0.0f;
  }
  if (SecondaryFadeAnimation.IsIn()) {
    SecondaryFadeAnimation.Progress = 0.0f;
  }

  if (PressToStartAnimation.Direction != AnimationDirection::Out ||
      PressToStartAnimation.LoopMode != AnimationLoopMode::Stop) {
    PressToStartAnimation.Direction = AnimationDirection::Out;
    PressToStartAnimation.LoopMode = AnimationLoopMode::Stop;
    PressToStartAnimation.StartOut();
  }

  if (TitleAnimation.IsOut() && !IsExploding) {
    TitleAnimation.StartIn(true);
    IsExploding = true;
    EverExploded = true;
  }
  if (TitleAnimation.IsIn() && !IsExploding) {
    TitleAnimation.StartOut(false);
    IsExploding = true;
  }
  TitleAnimationSprite.Show = true;
  TitleAnimationSprite.Face =
      (int)((TitleAnimationStartFrame +
             (TitleAnimationFrameCount * TitleAnimation.Progress)) *
            65536);
  ImpLog(LogLevel::Trace, LogChannel::General,
         "TitleAnimation: Progress {} Face: {}\n", TitleAnimation.Progress,
         TitleAnimationSprite.Face);
  TitleAnimationSprite.UpdateStatesToDraw();
}

void TitleMenu::ReturnToMenuUpdate() {
  if (MainItems->VisibilityState == Hidden) {
    InputLocked = false;
    CurrentlyFocusedElement = NewGame;
    for (auto& item : MainItems->Children) {
      if (item == NewGame) continue;  // Skip NewGame button
      item->HasFocus = false;
    }
    MainItems->Show();
    IsFocused = true;
  }
  PrimaryFadeAnimation.Progress = 1.0f;
  if (SlideItemsAnimation.IsOut()) {
    MainItems->Move({Profile::Game::DesignWidth / 2, 0.0f});
    static_cast<Widget*>(MenuLabel)->Move(
        {Profile::Game::DesignWidth / 2, 0.0f});
    SlideItemsAnimation.Progress = 1.0f;
  }
}

void TitleMenu::MainMenuUpdate() {
  if (IsFocused) {
    MainItems->HasFocus = true;
  }
  MainItems->Tint.a =
      glm::smoothstep(0.0f, 1.0f, PrimaryFadeAnimation.Progress);
  ContinueItems->Tint.a =
      glm::smoothstep(0.0f, 1.0f, SecondaryFadeAnimation.Progress);
  ExtraItems->Tint.a =
      glm::smoothstep(0.0f, 1.0f, SecondaryFadeAnimation.Progress);

  TitleAnimationSprite.Show = false;

  if (SlideItemsAnimation.IsOut()) {
    MainItems->Move({Profile::Game::DesignWidth / 2, 0.0f},
                    SlideItemsAnimation.DurationIn);
    static_cast<Widget*>(MenuLabel)->Move(
        {Profile::Game::DesignWidth / 2, 0.0f}, SlideItemsAnimation.DurationIn);
    SlideItemsAnimation.StartIn();
  }

  if (PrimaryFadeAnimation.IsOut()) {
    PrimaryFadeAnimation.StartIn();
  }

  if (MainItems->VisibilityState == Hidden) {
    MainItems->FocusLock = false;
    MainItems->Show();
  }

  if (SlideItemsAnimation.IsIn() && PrimaryFadeAnimation.IsIn() &&
      CurrentlyFocusedElement == nullptr) {
    MainItems->FocusLock = true;
    NewGame->Enabled = true;
    NewGame->HasFocus = true;
    NewGame->HighlightAnimation.Progress = 1.0f;
    NewGame->PrevFocusState = true;
    CurrentlyFocusedElement = NewGame;
    InputLocked = false;
    IsFocused = true;
  }

  if (SecondaryFadeAnimation.IsOut() && CurrentSubMenu) {
    if (CurrentSubMenu == ContinueItems) {
      HideContinueItems();
    } else if (CurrentSubMenu == ExtraItems) {
      HideExtraItems();
    }
  }
}

void TitleMenu::SubMenuUpdate() {
  MainItems->HasFocus = false;
  MainItems->Tint.a =
      glm::smoothstep(0.0f, 1.0f, PrimaryFadeAnimation.Progress);
  ContinueItems->Tint.a =
      glm::smoothstep(0.0f, 1.0f, SecondaryFadeAnimation.Progress);
  ExtraItems->Tint.a =
      glm::smoothstep(0.0f, 1.0f, SecondaryFadeAnimation.Progress);
  if (ScrWork[SW_SYSSUBMENUCT] == 0 && SubMenuState == Hidden) {
    PrimaryFadeAnimation.StartOut();
    SecondaryFadeAnimation.StartOut();
    AllowsScriptInput = false;
    if (SlideItemsAnimation.IsIn()) {
      SlideItemsAnimation.StartOut();
      MainItems->Move({-Profile::Game::DesignWidth / 2, 0.0f},
                      SlideItemsAnimation.DurationOut);
      static_cast<Widget*>(MenuLabel)->Move(
          {-Profile::Game::DesignWidth / 2, 0.0f},
          SlideItemsAnimation.DurationOut);
      if (CurrentSubMenu) {
        CurrentSubMenu->Move({-Profile::Game::DesignWidth / 2, 0.0f},
                             SlideItemsAnimation.DurationOut);
        CurrentSubMenu->HasFocus = false;
      }
    }

    SubMenuState = Showing;
  } else if (ScrWork[SW_SYSSUBMENUCT] < 32 && SubMenuState == Shown) {
    SlideItemsAnimation.StartIn();
    PrimaryFadeAnimation.StartIn();
    SecondaryFadeAnimation.StartIn();
    AllowsScriptInput = false;
    MainItems->Move({Profile::Game::DesignWidth / 2, 0.0f},
                    SlideItemsAnimation.DurationIn);
    static_cast<Widget*>(MenuLabel)->Move(
        {Profile::Game::DesignWidth / 2, 0.0f}, SlideItemsAnimation.DurationIn);
    if (CurrentSubMenu) {
      CurrentSubMenu->HasFocus = false;
      CurrentSubMenu->Move({Profile::Game::DesignWidth / 2, 0.0f},
                           SlideItemsAnimation.DurationIn);
    }
    SubMenuState = Hiding;
  }
}

void TitleMenu::Render() {
  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
    using enum TitleMenuState;
    switch (LookupTitleMenuState(ScrWork[SW_TITLEMODE])) {
      case PressToStart: {  // Press to start
        DrawDISwordBackground();
        DrawStartButton();
        DrawSmoke(SmokeOpacityNormal);
        Renderer->DrawSprite(CopyrightTextSprite,
                             glm::vec2(CopyrightTextX, CopyrightTextY));
        Renderer->DrawQuad(RectF(0.0f, 0.0f, Profile::Game::DesignWidth,
                                 Profile::Game::DesignHeight),
                           glm::vec4(1.0f, 1.0f, 1.0f,
                                     1.0f - ScrWork[SW_TITLEDISPCT] / 60.0f));
      } break;
      case Exploding: {  // Transition between Press to start
                         // and menus
        if (IsExploding || EverExploded) {
          DrawMainMenuBackGraphics();
        } else {
          DrawDISwordBackground();
        }
        DrawStartButton();
        TitleAnimationSprite.Render(-1);
        DrawSmoke(SmokeOpacityNormal);
      } break;
      case MainMenu: {  // MenuItems Fade In
        DrawMainMenuBackGraphics();
        DrawSmoke(SmokeOpacityNormal);
        Extra->Tint = (GetFlag(SF_CLR_FLAG)) ? MainItems->Tint
                                             : RgbIntToFloat(ExtraDisabledTint);

        MenuLabel->Render();
        MainItems->Render();
        ContinueItems->Render();
        ExtraItems->Render();
      } break;
      case ReturnToMenu: {
        DrawMainMenuBackGraphics();
        DrawSmoke(SmokeOpacityNormal);
        Extra->Tint = (GetFlag(SF_CLR_FLAG)) ? MainItems->Tint
                                             : RgbIntToFloat(ExtraDisabledTint);
        MenuLabel->Render();
        MainItems->Render();
        ContinueItems->Render();
        ExtraItems->Render();
        Renderer->DrawQuad(RectF(0.0f, 0.0f, Profile::Game::DesignWidth,
                                 Profile::Game::DesignHeight),
                           {0.0f, 0.0f, 0.0f, ScrWork[SW_TITLEDISPCT] / 32.0f});
      } break;
      // TODO check if that's true
      case SubMenu: {
        DrawMainMenuBackGraphics();
        DrawSmoke(SmokeOpacityNormal);
        MenuLabel->Render();
        MainItems->Render();
        ContinueItems->Render();
        ExtraItems->Render();
      } break;
      case InitialFadeIn: {  // Initial Fade In
        DrawDISwordBackground(ScrWork[SW_TITLEDISPCT] / 32.0f);
        DrawSmoke(ScrWork[SW_TITLEDISPCT] / 128.0f);
        Renderer->DrawSprite(CopyrightTextSprite,
                             glm::vec2(CopyrightTextX, CopyrightTextY));
      } break;
    }

    int maskAlpha = ScrWork[SW_TITLEMASKALPHA];
    glm::vec4 col = ScrWorkGetColor(SW_TITLEMASKCOLOR);
    col.a = glm::min(maskAlpha / 255.0f, 1.0f);
    Renderer->DrawQuad(RectF(0.0f, 0.0f, Profile::Game::DesignWidth,
                             Profile::Game::DesignHeight),
                       col);
  }
}

void TitleMenu::DrawDISwordBackground(float opacity) {
  Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f));
  Renderer->DrawSprite(OverlaySprite,
                       RectF(0.0f, 0.0f, Profile::Game::DesignWidth,
                             Profile::Game::DesignHeight),
                       glm::vec4(1.0f));
}

void TitleMenu::DrawStartButton() {
  glm::vec4 col = glm::vec4(1.0f);
  col.a = glm::smoothstep(0.0f, 1.0f, PressToStartAnimation.Progress);
  Renderer->DrawSprite(PressToStartSprite,
                       glm::vec2(PressToStartX, PressToStartY), col);
}

void TitleMenu::DrawMainMenuBackGraphics() {
  Renderer->DrawSprite(MainBackgroundSprite, glm::vec2(0.0f));
  Renderer->DrawSprite(OverlaySprite,
                       RectF(0.0f, 0.0f, Profile::Game::DesignWidth,
                             Profile::Game::DesignHeight),
                       glm::vec4(1.0f));
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

void TitleMenu::ShowContinueItems() {
  ContinueItems->Show();
  ContinueItems->Tint.a = 0.0f;
  MainItems->HasFocus = false;
  CurrentlyFocusedElement = Load;
  CurrentSubMenu = ContinueItems;
  Load->HasFocus = true;
  Load->Enabled = false;
  QuickLoad->Enabled = false;
  Continue->Enabled = false;
  SecondaryFadeAnimation.StartIn();

  Extra->Move(glm::vec2(0.0f, ItemPadding));
  Config->Move(glm::vec2(0.0f, ItemPadding));
  Help->Move(glm::vec2(0.0f, ItemPadding));
  if (Exit.has_value()) Exit->get().Move(glm::vec2(0.0f, ItemPadding));
  ContinueItems->Move(glm::vec2(Profile::Game::DesignWidth / 2, 0.0f),
                      SecondaryFadeAnimation.DurationOut);
}

void TitleMenu::HideContinueItems() {
  ContinueItems->Hide();
  MainItems->HasFocus = true;
  CurrentlyFocusedElement = Continue;
  CurrentSubMenu = nullptr;
  AllowsScriptInput = true;
  Extra->Move(glm::vec2(0.0f, -ItemPadding));
  Config->Move(glm::vec2(0.0f, -ItemPadding));
  Help->Move(glm::vec2(0.0f, -ItemPadding));
  if (Exit.has_value()) Exit->get().Move(glm::vec2(0.0f, -ItemPadding));
}

void TitleMenu::ShowExtraItems() {
  ExtraItems->Show();
  ExtraItems->Tint.a = 0.0f;
  MainItems->HasFocus = false;
  CurrentlyFocusedElement = Tips;
  CurrentSubMenu = ExtraItems;
  Tips->HasFocus = true;
  Tips->Enabled = false;
  Extra->Enabled = false;
  SecondaryFadeAnimation.StartIn();

  Config->Move(glm::vec2(0, ItemPadding));
  Help->Move(glm::vec2(0, ItemPadding));
  if (Exit.has_value()) Exit->get().Move(glm::vec2(0, ItemPadding));
  ExtraItems->Move({Profile::Game::DesignWidth / 2, 0.0f},
                   SecondaryFadeAnimation.DurationIn);
}

void TitleMenu::HideExtraItems() {
  ExtraItems->Hide();
  MainItems->HasFocus = true;
  CurrentlyFocusedElement = Extra;
  Extra->HasFocus = true;
  CurrentSubMenu = nullptr;
  AllowsScriptInput = true;
  Config->Move(glm::vec2(0, -ItemPadding));
  Help->Move(glm::vec2(0, -ItemPadding));
  if (Exit.has_value()) Exit->get().Move(glm::vec2(0, -ItemPadding));
  Config->Enabled = true;
  Help->Enabled = true;
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto