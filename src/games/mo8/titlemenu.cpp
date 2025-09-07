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
#include "../../vm/interface/input.h"

namespace Impacto {
namespace UI {
namespace MO8 {

using namespace Impacto::Profile::TitleMenu;
using namespace Impacto::Profile::MO8::TitleMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets;

void TitleMenu::MenuButtonOnClick(Widgets::Button* target) {
  AllowsScriptInput = true;
  ScrWork[SW_TITLECUR] = target->Id;
  SetFlag(SF_TITLEEND, 1);
  Audio::Channels[Audio::AC_SSE]->Play("sysse", 5, false, 0.0f);
}

void TitleMenu::ContinueButtonOnClick(Widgets::Button* target) {
  ContinueSelected = true;
  AllowsScriptInput = false;
  Audio::Channels[Audio::AC_SSE]->Play("sysse", 5, false, 0.0f);
}

void TitleMenu::GalleryButtonOnClick(Widgets::Button* target) {
  GallerySelected = true;
  AllowsScriptInput = false;
  Audio::Channels[Audio::AC_SSE]->Play("sysse", 5, false, 0.0f);
}

TitleMenu::TitleMenu() {
  MainItems = new Widgets::Group(this);
  ContinueItems = new Widgets::Group(this);
  GalleryItems = new Widgets::Group(this);

  auto onClick = [this](auto* btn) { return MenuButtonOnClick(btn); };
  auto continueOnClick = [this](auto* btn) {
    return ContinueButtonOnClick(btn);
  };
  auto galleryOnClick = [this](auto* btn) { return GalleryButtonOnClick(btn); };

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
  //  Gallery menu button
  Gallery = new Button(
      0, MenuEntriesSprites[GallerySpriteIndex],
      MenuEntriesHSprites[GallerySpriteIndex],
      MenuEntriesHSprites[NewGameSpriteIndex + 1],
      glm::vec2(MenuEntriesX, MenuEntriesFirstY + (3 * MenuEntriesYPadding)));
  Gallery->OnClickHandler = galleryOnClick;
  Gallery->HighlightOffset = glm::vec2(0.0f);
  Gallery->Enabled = false;
  MainItems->Add(Gallery, FDIR_DOWN);

  // Continue buttons
  // Load button
  Load = new Button(10, MenuEntriesSprites[LoadSpriteIndex],
                    MenuEntriesHSprites[LoadSpriteIndex],
                    MenuEntriesHSprites[NewGameSpriteIndex + 1],
                    glm::vec2(MenuEntriesX, MenuEntriesFirstY));
  Load->OnClickHandler = onClick;
  Load->HighlightOffset = glm::vec2(0.0f);
  ContinueItems->Add(Load, FDIR_DOWN);
  // Quick load button
  QuickLoad = new Button(
      11, MenuEntriesSprites[QuickLoadSpriteIndex],
      MenuEntriesHSprites[QuickLoadSpriteIndex],
      MenuEntriesHSprites[NewGameSpriteIndex + 1],
      glm::vec2(MenuEntriesX, MenuEntriesFirstY + (1 * MenuEntriesYPadding)));
  QuickLoad->OnClickHandler = onClick;
  QuickLoad->HighlightOffset = glm::vec2(0.0f);
  ContinueItems->Add(QuickLoad, FDIR_DOWN);

  int index = 0;
  // Gallery buttons
  // Album button
  AlbumButton =
      new Button(30, MenuEntriesSprites[AlbumSpriteIndex],
                 MenuEntriesHSprites[AlbumSpriteIndex],
                 MenuEntriesHSprites[NewGameSpriteIndex + 1],
                 glm::vec2(MenuEntriesX, MenuEntriesGalleryFirstY +
                                             (index * MenuEntriesYPadding)));
  AlbumButton->OnClickHandler = onClick;
  AlbumButton->HighlightOffset = glm::vec2(0.0f);
  GalleryItems->Add(AlbumButton, FDIR_DOWN);
  index++;
  // Music button
  MusicButton =
      new Button(31, MenuEntriesSprites[MusicSpriteIndex],
                 MenuEntriesHSprites[MusicSpriteIndex],
                 MenuEntriesHSprites[NewGameSpriteIndex + 1],
                 glm::vec2(MenuEntriesX, MenuEntriesGalleryFirstY +
                                             (index * MenuEntriesYPadding)));
  MusicButton->OnClickHandler = onClick;
  MusicButton->HighlightOffset = glm::vec2(0.0f);
  GalleryItems->Add(MusicButton, FDIR_DOWN);
  index++;
  // Clear List button
  ClearListButton =
      new Button(32, MenuEntriesSprites[ClearListSpriteIndex],
                 MenuEntriesHSprites[ClearListSpriteIndex],
                 MenuEntriesHSprites[NewGameSpriteIndex + 1],
                 glm::vec2(MenuEntriesX, MenuEntriesGalleryFirstY +
                                             (index * MenuEntriesYPadding)));
  ClearListButton->OnClickHandler = onClick;
  ClearListButton->HighlightOffset = glm::vec2(0.0f);
  GalleryItems->Add(ClearListButton, FDIR_DOWN);
  index++;
  // Warning button
  WarningButton =
      new Button(33, MenuEntriesSprites[WarningSpriteIndex],
                 MenuEntriesHSprites[WarningSpriteIndex],
                 MenuEntriesHSprites[NewGameSpriteIndex + 1],
                 glm::vec2(MenuEntriesX, MenuEntriesGalleryFirstY +
                                             (index * MenuEntriesYPadding)));
  WarningButton->OnClickHandler = onClick;
  WarningButton->HighlightOffset = glm::vec2(0.0f);
  GalleryItems->Add(WarningButton, FDIR_DOWN);
  index++;
  if (HasAdditional) {
    // Additional button
    AdditionalButton =
        new Button(34, MenuEntriesSprites[AdditionalSpriteIndex],
                   MenuEntriesHSprites[AdditionalSpriteIndex],
                   MenuEntriesHSprites[NewGameSpriteIndex + 1],
                   glm::vec2(MenuEntriesX, MenuEntriesGalleryFirstY +
                                               (index * MenuEntriesYPadding)));
    AdditionalButton->OnClickHandler = onClick;
    AdditionalButton->HighlightOffset = glm::vec2(0.0f);
    GalleryItems->Add(AdditionalButton, FDIR_DOWN);
    index++;
  }
  // DLC button
  DLCButton =
      new Button(35 - (!HasAdditional), MenuEntriesSprites[DLCSpriteIndex],
                 MenuEntriesHSprites[DLCSpriteIndex],
                 MenuEntriesHSprites[NewGameSpriteIndex + 1],
                 glm::vec2(MenuEntriesX, MenuEntriesGalleryFirstY +
                                             (index * MenuEntriesYPadding)));
  DLCButton->OnClickHandler = onClick;
  DLCButton->HighlightOffset = glm::vec2(0.0f);
  GalleryItems->Add(DLCButton, FDIR_DOWN);
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
    if (PressToStartAnimated &&
        PressToStartAnimation.State == AnimationState::Stopped) {
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
  UpdateInput(dt);

  PrimaryFadeAnimation.Update(dt);
  MainItemsHideAnimation.Update(dt);
  ContinueItemsShowAnimation.Update(dt);
  GalleryItemsShowAnimation.Update(dt);
  if (PressToStartAnimated) {
    PressToStartAnimation.Update(dt);
  }
  if (GetFlag(SF_TITLEMODE)) {
    Show();
  } else {
    Hide();
  }

  if (State != Hidden && IsFocused && GetFlag(SF_TITLEMODE) &&
      ScrWork[SW_TITLEMODE] != 1) {
    Gallery->Tint = glm::vec4(1.0f, 1.0f, 1.0f, GetFlag(803) ? 1.0f : 0.0f);
    Gallery->Enabled = GetFlag(803);
    MainItems->Update(dt);
    ContinueItems->Update(dt);
    GalleryItems->Update(dt);

    switch (ScrWork[SW_TITLEMODE]) {
      case 0:
      case 2: {
        if (PrimaryFadeAnimation.State == AnimationState::Stopped &&
            ScrWork[SW_TITLEDISPCT] == 0)
          PrimaryFadeAnimation.StartOut(true);
      } break;
      case 5: {
        if (ContinueSelected) {
          UpdateSubMenu(&ContinueItemsShowAnimation, ContinueItems,
                        &ContinueSelected);
        } else if (GallerySelected) {
          UpdateSubMenu(&GalleryItemsShowAnimation, GalleryItems,
                        &GallerySelected);
        } else {
          if (!MainItems->IsShown) MainItems->Show();
        }
      } break;
      case 6: {
        if (PrimaryFadeAnimation.State == AnimationState::Stopped &&
            ScrWork[SW_TITLEDISPCT] == 0)
          PrimaryFadeAnimation.StartIn(true);
      } break;
      case 11: {
        if (PrimaryFadeAnimation.IsIn()) {
          PrimaryFadeAnimation.StartOut(true);
        }
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
        Renderer->DrawQuad(
            RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
            black);
      } break;
      case 2: {  // Press to start fade
        glm::vec4 col = glm::vec4(1.0f);
        col.a = glm::smoothstep(
            0.0f, 1.0f,
            ScrWork[SW_TITLEDISPCT] > 0 ? PrimaryFadeAnimation.Progress : 1.0f);
        Renderer->DrawSprite(PressToStartSprite,
                             glm::vec2(PressToStartX, PressToStartY), col);
      } break;
      case 5:
      case 6:
      case 7:
      case 11: {  // Main and main fade out
        MainItems->Tint = glm::vec4(
            1.0f, 1.0f, 1.0f,
            glm::smoothstep(1.0f, 0.0f, MainItemsHideAnimation.Progress));
        MainItems->Render();
        ContinueItems->Tint = glm::vec4(
            1.0f, 1.0f, 1.0f,
            glm::smoothstep(0.0f, 1.0f, ContinueItemsShowAnimation.Progress));
        ContinueItems->Render();
        GalleryItems->Tint = glm::vec4(
            1.0f, 1.0f, 1.0f,
            glm::smoothstep(0.0f, 1.0f, GalleryItemsShowAnimation.Progress));
        GalleryItems->Render();
        glm::vec4 black = glm::vec4(0.0f);
        black.a = glm::smoothstep(0.0f, 1.0f, PrimaryFadeAnimation.Progress);
        Renderer->DrawQuad(
            RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
            black);
      } break;
    }
  }
}

void TitleMenu::UpdateSubMenu(Animation* showAnimation,
                              Widgets::Group* subMenuGroup,
                              bool* subMenuSelected) {
  if (MainItemsHideAnimation.IsOut()) {
    MainItemsHideAnimation.StartIn();
  } else if (MainItemsHideAnimation.IsIn() && showAnimation->IsOut()) {
    if (subMenuGroup->IsShown) {
      MainItems->Show();
      subMenuGroup->Hide();
      MainItemsHideAnimation.StartOut();
      *subMenuSelected = false;
      AllowsScriptInput = true;
    } else {
      MainItems->Hide();
      subMenuGroup->Show();
      showAnimation->StartIn();
    }
  } else if (showAnimation->IsIn() &&
             Vm::Interface::GetControlState(Vm::Interface::CT_Back)) {
    Audio::Channels[Audio::AC_SSE]->Play("sysse", 3, false, 0.0f);
    showAnimation->StartOut();
  }
  Vm::Interface::PADinputButtonWentDown &= ~Vm::Interface::PADcustom[6];
  Vm::Interface::PADinputMouseWentDown &= ~Vm::Interface::PADcustom[6];
}

}  // namespace MO8
}  // namespace UI
}  // namespace Impacto