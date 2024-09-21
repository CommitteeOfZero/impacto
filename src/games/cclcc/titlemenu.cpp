#include "titlemenu.h"

#include "../../texture/texture.h"
#include "../../spritesheet.h"
#include "../../io/vfs.h"

#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../inputsystem.h"
#include "../../ui/widgets/label.h"
#include "../../profile/ui/titlemenu.h"
#include "../../profile/games/cclcc/titlemenu.h"
#include "../../profile/scriptvars.h"
#include "../../profile/game.h"
#include "../../profile/vm.h"
#include "../../vm/interface/input.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

using namespace Impacto::Profile::TitleMenu;
using namespace Impacto::Profile::CCLCC::TitleMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets::CCLCC;

void TitleMenu::MenuButtonOnClick(Widgets::Button* target) {
  ScrWork[SW_TITLECUR1] = target->Id;
  SetFlag(SF_TITLEEND, 1);
  AllowsScriptInput = true;
}

void TitleMenu::ContinueButtonOnClick(Widgets::Button* target) {
  CurrentSubMenu = ContinueItems;
  CurrentSubMenu->HasFocus = true;
  AllowsScriptInput = false;
}

void TitleMenu::ExtraButtonOnClick(Widgets::Button* target) {
  CurrentSubMenu = ExtraItems;
  CurrentSubMenu->HasFocus = true;
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

  Widgets::Label* MenuLabel =
      new Widgets::Label(MenuSprite, glm::vec2(MenuX, MenuY));
  MainItems->Add(MenuLabel);

  // NewGame menu button
  NewGame = new TitleButton(0, MenuEntriesSprites[0], MenuEntriesHSprites[0],
                            ItemHighlightSprite,
                            glm::vec2((-1.0f) + ItemHighlightOffsetX,
                                      (ItemYBase + (0 * ItemPadding))));
  NewGame->OnClickHandler = onClick;
  MainItems->Add(NewGame, FDIR_DOWN);

  // Continue menu button
  Continue = new TitleButton(
      1, MenuEntriesSprites[1], MenuEntriesHSprites[1], ItemHighlightSprite,
      glm::vec2(ItemHighlightOffsetX, (ItemYBase + (1 * ItemPadding))));
  Continue->OnClickHandler = continueOnClick;
  MainItems->Add(Continue, FDIR_DOWN);

  // Extra menu button
  Extra = new TitleButton(
      2, MenuEntriesSprites[2], MenuEntriesHSprites[2], ItemHighlightSprite,
      glm::vec2(ItemHighlightOffsetX, (ItemYBase + (2 * ItemPadding))));
  Extra->OnClickHandler = extraOnClick;
  MainItems->Add(Extra, FDIR_DOWN);

  // Config menu button
  Config = new TitleButton(
      30, MenuEntriesSprites[3], MenuEntriesHSprites[3], ItemHighlightSprite,
      glm::vec2(ItemHighlightOffsetX, (ItemYBase + (3 * ItemPadding))));
  Config->OnClickHandler = onClick;
  MainItems->Add(Config, FDIR_DOWN);

  // Help menu button
  Help = new TitleButton(
      40, MenuEntriesSprites[4], MenuEntriesHSprites[4], ItemHighlightSprite,
      glm::vec2(ItemHighlightOffsetX, (ItemYBase + (4 * ItemPadding))));
  Help->OnClickHandler = onClick;
  MainItems->Add(Help, FDIR_DOWN);

  // Load secondary Continue menu button
  Load = new TitleButton(10, LoadSprite, LoadHighlightSprite, nullSprite,
                         glm::vec2(SecondaryFirstItemHighlightOffsetX,
                                   (ItemYBase + (2 * ItemPadding))));
  Load->OnClickHandler = onClick;
  Load->IsSubButton = true;
  ContinueItems->Add(Load, FDIR_RIGHT);

  // QuickLoad secondary Continue menu button
  QuickLoad =
      new TitleButton(11, QuickLoadSprite, QuickLoadHighlightSprite, nullSprite,
                      glm::vec2(SecondarySecondItemHighlightOffsetX,
                                (ItemYBase + (2 * ItemPadding))));
  QuickLoad->OnClickHandler = onClick;
  QuickLoad->IsSubButton = true;
  ContinueItems->Add(QuickLoad, FDIR_RIGHT);

  // Tips secondary Extra menu button
  Tips = new TitleButton(20, TipsSprite, TipsHighlightSprite, nullSprite,
                         glm::vec2(SecondaryFirstItemHighlightOffsetX,
                                   (ItemYBase + (3 * ItemPadding))));
  Tips->OnClickHandler = onClick;
  Tips->IsSubButton = true;
  ExtraItems->Add(Tips, FDIR_RIGHT);

  // Library secondary Extra menu button
  Library =
      new TitleButton(21, LibrarySprite, LibraryHighlightSprite, nullSprite,
                      glm::vec2(SecondarySecondItemHighlightOffsetX,
                                (ItemYBase + (3 * ItemPadding))));
  Library->OnClickHandler = onClick;
  Library->IsSubButton = true;
  ExtraItems->Add(Library, FDIR_RIGHT);

  // EndingList secondary Extra menu button
  EndingList = new TitleButton(22, EndingListSprite, EndingListHighlightSprite,
                               nullSprite,
                               glm::vec2(SecondaryThirdItemHighlightOffsetX,
                                         (ItemYBase + (3 * ItemPadding))));
  EndingList->OnClickHandler = onClick;
  EndingList->IsSubButton = true;
  ExtraItems->Add(EndingList, FDIR_RIGHT);

  // Start menu items offscreen
  MainItems->Move({-Profile::DesignWidth / 2, 0.0f});

  PressToStartAnimation.DurationIn = PressToStartAnimDurationIn;
  PressToStartAnimation.DurationOut = PressToStartAnimDurationOut;
  PrimaryFadeAnimation.DurationIn = PrimaryFadeInDuration;
  PrimaryFadeAnimation.DurationOut = PrimaryFadeOutDuration;
  SecondaryFadeAnimation.DurationIn = SecondaryFadeInDuration;
  SecondaryFadeAnimation.DurationOut = SecondaryFadeOutDuration;
  SmokeAnimation.DurationIn = SmokeAnimationDurationIn;
  SmokeAnimation.DurationOut = SmokeAnimationDurationOut;
  ChoiceBlinkAnimation.DurationIn = ChoiceBlinkAnimationDuration;
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
        CurrentSubMenu->IsShown && CurrentSubMenu->HasFocus) {
      if (CurrentSubMenu == ContinueItems) {
        HideContinueItems();
      }
      if (CurrentSubMenu == ExtraItems) {
        HideExtraItems();
      }
    }
  }
}

void TitleMenu::Update(float dt) {
  UpdateInput();

  PressToStartAnimation.Update(dt);
  PrimaryFadeAnimation.Update(dt);
  SecondaryFadeAnimation.Update(dt);
  SmokeAnimation.Update(dt);
  TitleAnimation.Update(dt);
  TitleAnimationSprite.Update(dt);
  TitleAnimationSprite.OffsetX = 0.0f;
  TitleAnimationSprite.OffsetY = 0.0f;
  SlideItemsAnimation.Update(dt);

  MainItems->Update(dt);
  ContinueItems->Update(dt);
  ExtraItems->Update(dt);

  if (GetFlag(SF_TITLEMODE)) {
    Show();
  } else {
    Hide();
  }

  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
    switch (ScrWork[SW_TITLEMODE]) {
      case 1: {
      } break;
      case 2: {
        if (SlideItemsAnimation.IsIn()) {
          SlideItemsAnimation.Progress = 0.0f;
          MainItems->Move({-Profile::DesignWidth / 2, 0.0f});
          MainItems->HasFocus = false;
        }
        if (PrimaryFadeAnimation.IsIn()) {
          PrimaryFadeAnimation.Progress = 0.0f;
        }
        if (SecondaryFadeAnimation.IsIn()) {
          SecondaryFadeAnimation.Progress = 0.0f;
        }
        if (TitleAnimation.IsIn())
          TitleAnimation.StartOut();
        else if (TitleAnimation.IsOut())
          TitleAnimation.StartIn();
        TitleAnimationSprite.Show = true;
        TitleAnimationSprite.Face =
            (TitleAnimationStartFrame +
             (int)(TitleAnimationFrameCount * TitleAnimation.Progress))
            << 16;
      } break;
      case 3: {  // Main Menu Fade In
        MainItems->HasFocus = true;
        if (CurrentSubMenu) {
          CurrentSubMenu->HasFocus = true;
        }

        MainItems->Tint.a =
            glm::smoothstep(0.0f, 1.0f, PrimaryFadeAnimation.Progress);
        ContinueItems->Tint.a =
            glm::smoothstep(0.0f, 1.0f, SecondaryFadeAnimation.Progress);
        ExtraItems->Tint.a =
            glm::smoothstep(0.0f, 1.0f, SecondaryFadeAnimation.Progress);

        TitleAnimationSprite.Show = false;

        if (SlideItemsAnimation.IsOut()) {
          MainItems->Move({Profile::DesignWidth / 2, 0.0f},
                          SlideItemsAnimation.DurationIn);
          SlideItemsAnimation.StartIn();
        }

        if (PrimaryFadeAnimation.IsOut()) {
          PrimaryFadeAnimation.StartIn();
        }
        if (CurrentSubMenu && SecondaryFadeAnimation.IsOut()) {
          SecondaryFadeAnimation.StartIn();
        }

        if (CurrentSubMenu && !CurrentSubMenu->IsShown) {
          if (CurrentSubMenu == ContinueItems) {
            ShowContinueItems();
          } else if (CurrentSubMenu == ExtraItems) {
            ShowExtraItems();
          }
        }

        if (!MainItems->IsShown) {
          MainItems->Show();
          MainItems->Tint.a = 0.0f;
          CurrentlyFocusedElement = NewGame;
          NewGame->HasFocus = true;
        }
      } break;
      case 4: {
      } break;
      case 5: {
        CurrentSubMenu->HasFocus = false;
        MainItems->HasFocus = false;
        ChoiceBlinkAnimation.Update(dt);
        MainItems->Tint.a =
            glm::smoothstep(0.0f, 1.0f, PrimaryFadeAnimation.Progress);
        ContinueItems->Tint.a =
            glm::smoothstep(0.0f, 1.0f, SecondaryFadeAnimation.Progress);
        ExtraItems->Tint.a =
            glm::smoothstep(0.0f, 1.0f, SecondaryFadeAnimation.Progress);

        if (ChoiceBlinkAnimation.IsOut() && ScrWork[SW_SYSSUBMENUCT] == 0) {
          ChoiceBlinkAnimation.StartIn();
        } else if (ChoiceBlinkAnimation.State == AS_Playing) {
          // Calculate the blink effect to occur 4 times during the animation
          float blinkProgress = ChoiceBlinkAnimation.Progress * 4.0f;
          CurrentlyFocusedElement->Tint.a =
              0.5f * (1.0f + cos(blinkProgress * glm::two_pi<float>()));
        } else if (ChoiceBlinkAnimation.IsIn() && SlideItemsAnimation.IsIn() &&
                   ScrWork[SW_SYSSUBMENUCT] == 32) {
          ChoiceBlinkAnimation.Progress = 0.0f;
          SlideItemsAnimation.StartOut();
          PrimaryFadeAnimation.StartOut();
          SecondaryFadeAnimation.StartOut();
          MainItems->Move({-Profile::DesignWidth / 2, 0.0f},
                          SlideItemsAnimation.DurationOut);
          CurrentSubMenu->Move({-Profile::DesignWidth / 2, 0.0f},
                               SlideItemsAnimation.DurationOut);
        } else if (SlideItemsAnimation.IsOut() &&
                   ScrWork[SW_SYSSUBMENUCT] < 32 &&
                   ScrWork[SW_SYSSUBMENUCT] > 0) {
          SlideItemsAnimation.StartIn();
          PrimaryFadeAnimation.StartIn();
          SecondaryFadeAnimation.StartIn();
          MainItems->Move({Profile::DesignWidth / 2, 0.0f},
                          SlideItemsAnimation.DurationIn);
          CurrentSubMenu->Move({Profile::DesignWidth / 2, 0.0f},
                               SlideItemsAnimation.DurationIn);
        }
      } break;
    }
  }
}

void TitleMenu::Render() {
  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
    switch (ScrWork[SW_TITLEMODE]) {
      case 1: {  // Press to start
        DrawDISwordBackground();
        DrawStartButton();
        DrawSmoke(SmokeOpacityNormal);
        Renderer->DrawSprite(CopyrightTextSprite,
                             glm::vec2(CopyrightTextX, CopyrightTextY));
        Renderer->DrawRect(
            RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
            glm::vec4(1.0f, 1.0f, 1.0f,
                      1.0f - ScrWork[SW_TITLEDISPCT] / 60.0f));
      } break;
      case 2: {  // Transition between Press to start and menus
        DrawMainMenuBackGraphics();
        DrawSmoke(SmokeOpacityNormal);
        TitleAnimationSprite.Render(-1);
      } break;
      case 3: {  // MenuItems Fade In
        DrawMainMenuBackGraphics();
        DrawSmoke(SmokeOpacityNormal);
        MainItems->Render();
        ContinueItems->Render();
        ExtraItems->Render();
      } break;
      case 4: {
        ImpLogSlow(LL_Warning, LC_VMStub,
                   "TitleMenu::Render: Unimplemented title mode %d\n",
                   ScrWork[SW_TITLEMODE]);
      } break;
      case 5: {
        // Implement
        DrawMainMenuBackGraphics();
        DrawSmoke(SmokeOpacityNormal);
        MainItems->Render();
        ContinueItems->Render();
        ExtraItems->Render();
        ImpLogSlow(LL_Warning, LC_VMStub,
                   "TitleMenu::Render: Unimplemented title mode %d\n",
                   ScrWork[SW_TITLEMODE]);
      } break;
      case 10: {
        ImpLogSlow(LL_Warning, LC_VMStub,
                   "TitleMenu::Render: Unimplemented title mode %d\n",
                   ScrWork[SW_TITLEMODE]);
      } break;
      case 11: {  // Initial Fade In
        DrawDISwordBackground(ScrWork[SW_TITLEDISPCT] / 32.0f);
        DrawSmoke(ScrWork[SW_TITLEDISPCT] / 128.0f);
        Renderer->DrawSprite(CopyrightTextSprite,
                             glm::vec2(CopyrightTextX, CopyrightTextY));
      } break;
      case 14: {
        ImpLogSlow(LL_Warning, LC_VMStub,
                   "TitleMenu::Render: Unimplemented title mode %d\n",
                   ScrWork[SW_TITLEMODE]);
      } break;
    }

    int maskAlpha = ScrWork[SW_TITLEMASKALPHA];
    glm::vec4 col = ScrWorkGetColor(SW_TITLEMASKCOLOR);
    col.a = glm::min(maskAlpha / 255.0f, 1.0f);
    Renderer->DrawRect(
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight), col);
  }
}

void TitleMenu::DrawDISwordBackground(float opacity) {
  Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f));
  Renderer->DrawSprite(
      OverlaySprite,
      RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
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
  Renderer->DrawSprite(
      OverlaySprite,
      RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
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
  SecondaryFadeAnimation.StartIn();

  Extra->Move(glm::vec2(0.0f, ItemPadding));
  Config->Move(glm::vec2(0.0f, ItemPadding));
  Help->Move(glm::vec2(0.0f, ItemPadding));
}

void TitleMenu::HideContinueItems() {
  ContinueItems->Hide();
  MainItems->HasFocus = true;
  CurrentlyFocusedElement = Continue;
  CurrentSubMenu = 0;
  ContinueItems->HasFocus = true;
  SecondaryFadeAnimation.StartOut();

  Extra->Move(glm::vec2(0.0f, -ItemPadding));
  Config->Move(glm::vec2(0.0f, -ItemPadding));
  Help->Move(glm::vec2(0.0f, -ItemPadding));
}

void TitleMenu::ShowExtraItems() {
  ExtraItems->Show();
  ExtraItems->Tint.a = 0.0f;
  MainItems->HasFocus = false;
  CurrentlyFocusedElement = Tips;
  CurrentSubMenu = ExtraItems;
  Tips->HasFocus = true;
  SecondaryFadeAnimation.StartIn();

  Config->Move(glm::vec2(0, ItemPadding));
  Help->Move(glm::vec2(0, ItemPadding));
}

void TitleMenu::HideExtraItems() {
  ExtraItems->Hide();
  MainItems->HasFocus = true;
  CurrentlyFocusedElement = Extra;
  CurrentSubMenu = 0;
  ExtraItems->HasFocus = true;
  SecondaryFadeAnimation.StartOut();

  Config->Move(glm::vec2(0, -ItemPadding));
  Help->Move(glm::vec2(0, -ItemPadding));
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto