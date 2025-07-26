#include "librarymenu.h"

#include "clearlistmenu.h"

#include "../../profile/games/cclcc/librarymenu.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../profile/dialogue.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../video/videosystem.h"
#include "../../profile/scriptvars.h"
#include "../../vm/interface/input.h"
#include "../../profile/game.h"
#include "../../ui/widgets/cclcc/librarymenubutton.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

using namespace Impacto::Profile::CCLCC::LibraryMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;
using namespace Impacto::UI::Widgets::CCLCC;

LibrarySubmenu& LibraryMenu::GetMenuFromType(
    LibraryMenuPageType menuType) const {
  switch (menuType) {
    case LibraryMenuPageType::Album:
      return static_cast<LibrarySubmenu&>(*UI::AlbumMenuPtr);
    case LibraryMenuPageType::Sound:
      return static_cast<LibrarySubmenu&>(*UI::MusicMenuPtr);
    case LibraryMenuPageType::Movie:
      return static_cast<LibrarySubmenu&>(*UI::MovieMenuPtr);
  }
}

LibraryMenuPageType LibraryMenu::GetMenuTypeFromButton(Widget* btn) const {
  const auto& btnArr = MainItems.Children;
  if (btn == btnArr[LibraryMenuPageType::Album]) {
    return LibraryMenuPageType::Album;

  } else if (btn == btnArr[LibraryMenuPageType::Sound]) {
    return LibraryMenuPageType::Sound;

  } else if (btn == btnArr[LibraryMenuPageType::Movie]) {
    return LibraryMenuPageType::Movie;
  }
}

LibraryMenu::LibraryMenu() : MainItems(this) {
  auto readyExistingMenu = [this](LibraryMenuPageType clickedType,
                                  LibraryMenuButton* btn) -> bool {
    if (CurrentLibraryMenu == clickedType) return !btn->Selected;
    auto& submenu = GetMenuFromType(CurrentLibraryMenu);
    if (submenu.State != UI::MenuState::Shown) return false;
    submenu.Hide();
    return true;
  };
  auto libraryMenuOnClickCommon = [this,
                                   readyExistingMenu](Widgets::Button* target) {
    auto clickedType = LibraryMenuPageType::_from_integral(target->Id);
    auto* button = static_cast<LibraryMenuButton*>(target);
    if (!readyExistingMenu(clickedType, button)) return;
    GetMenuFromType(clickedType).Show();
    auto* prevButton = static_cast<LibraryMenuButton*>(
        MainItems.Children.at(CurrentLibraryMenu));
    prevButton->Selected = false;
    Audio::Channels[Audio::AC_SSE]->Play("sysse", 2, false, 0);
    CurrentLibraryMenu = clickedType;
    AllowsScriptInput = false;
    button->Selected = true;
  };

  auto* album =
      new LibraryMenuButton(0, SnapPhotoSpriteHover, SnapPhotoSpriteSelect,
                            SnapPhotoPos, ButtonBlinkAnimation);
  album->OnClickHandler = libraryMenuOnClickCommon;

  auto* sound =
      new LibraryMenuButton(1, HitSongsSpriteHover, HitSongsSpriteSelect,
                            HitSongsPos, ButtonBlinkAnimation);
  sound->OnClickHandler = libraryMenuOnClickCommon;
  auto* movie =
      new LibraryMenuButton(2, LoveMovieSpriteHover, LoveMovieSpriteSelect,
                            LoveMoviePos, ButtonBlinkAnimation);
  movie->OnClickHandler = libraryMenuOnClickCommon;

  MainItems.Add(album, FDIR_DOWN);
  MainItems.Add(sound, FDIR_DOWN);
  MainItems.Add(movie, FDIR_DOWN);
  MainItems.MoveTo({-LibraryTransitionPositionOffset, 0.0f});
  FocusStart[FDIR_DOWN] = album;
  FocusStart[FDIR_UP] = movie;

  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
  ButtonBlinkAnimation.Direction = AnimationDirection::In;
  ButtonBlinkAnimation.LoopMode = AnimationLoopMode::ReverseDirection;
  ButtonBlinkAnimation.SetDuration(ButtonBlinkDuration);
}

void LibraryMenu::Init() { SetFlag(SF_ALBUMEND, false); }
void LibraryMenu::Show() {
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    UI::FocusedMenu = this;
    MainItems.Show();
    MainItems.Move({LibraryTransitionPositionOffset, 0.0f},
                   FadeAnimation.DurationIn);
  }
}

void LibraryMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    FadeAnimation.StartOut();
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    MainItems.Hide();
    MainItems.IsShown = true;
    MainItems.Move({-LibraryTransitionPositionOffset, 0.0f},
                   FadeAnimation.DurationOut);
    IsFocused = false;
  }
}

void LibraryMenu::Update(float dt) {
  auto* prevBtn = static_cast<LibraryMenuButton*>(CurrentlyFocusedElement);
  const bool wasHovered = prevBtn && prevBtn->Hovered;
  if (ScrWork[SW_SYSSUBMENUCT] < 32 && State == Shown) {
    Hide();
  } else if (ScrWork[SW_SYSSUBMENUCT] > 0 && State == Hidden &&
             (ScrWork[SW_SYSSUBMENUNO] == 8)) {
    Show();
  }
  const auto* albumMenuPtr = static_cast<AlbumMenu*>(UI::AlbumMenuPtr);
  const bool moviePlaying = CurrentLibraryMenu == +LibraryMenuPageType::Movie &&
                            Video::Players[0]->IsPlaying;
  const bool cgViewerActive =
      CurrentLibraryMenu == +LibraryMenuPageType::Album &&
      albumMenuPtr->CGViewer;
  if (State == Shown && ScrWork[SW_SYSSUBMENUNO] == 8) {
    if (!moviePlaying && !cgViewerActive) {
      UpdateInput();
      if ((Vm::Interface::PADinputButtonWentDown & Vm::Interface::PAD1B) ||
          (Vm::Interface::PADinputMouseWentDown & Vm::Interface::PAD1B)) {
        Audio::Channels[Audio::AC_SSE]->Play("sysse", 3, false, 0);
        if (!IsFocused) {  // unfocus submenu
          if (CurrentLibraryMenu != +LibraryMenuPageType::Album ||
              !albumMenuPtr->CGViewer) {
            auto* activeButton = static_cast<LibraryMenuButton*>(
                MainItems.Children.at(CurrentLibraryMenu));
            auto& submenu = GetMenuFromType(CurrentLibraryMenu);
            submenu.Unfocus();
            IsFocused = true;
            activeButton->Selected = false;
            if (!CurrentlyFocusedElement) activeButton->HasFocus = true;
          }
        } else {
          UI::AlbumMenuPtr->Hide();
          UI::MusicMenuPtr->Hide();
          UI::MovieMenuPtr->Hide();
          SetFlag(SF_ALBUMEND, 1);
        }
      }
    }
  }
  if (State == Hidden) return;

  FadeAnimation.Update(dt);
  UI::AlbumMenuPtr->Update(dt);
  UI::MusicMenuPtr->Update(dt);
  UI::MovieMenuPtr->Update(dt);
  ButtonBlinkAnimation.Update(dt);
  if (!moviePlaying && !cgViewerActive) MainItems.Update(dt);

  if (CurrentlyFocusedElement) {
    auto* activeBtn = static_cast<LibraryMenuButton*>(CurrentlyFocusedElement);
    if (activeBtn->Selected)
      ButtonBlinkAnimation.Stop();
    else if (ButtonBlinkAnimation.State == +AnimationState::Stopped)
      ButtonBlinkAnimation.StartIn();
    if (!IsFocused && !activeBtn->Hovered &&
        wasHovered) {  // Stop blink when mouse out on submenu
      ButtonBlinkAnimation.Stop();
      ButtonBlinkAnimation.Progress = 0.0f;
      CurrentlyFocusedElement->HasFocus = false;
    }
    if (prevBtn != CurrentlyFocusedElement) {
      Audio::Channels[Audio::AC_SSE]->Play("sysse", 1, false, 0);
    }
  }

  if (State == Showing && FadeAnimation.Progress == 1.0f &&
      ScrWork[SW_SYSSUBMENUCT] == 32) {
    State = Shown;
    if (!CurrentlyFocusedElement) {
      CurrentlyFocusedElement = MainItems.Children.at(CurrentLibraryMenu);
      GetMenuFromType(CurrentLibraryMenu).Show();
      static_cast<LibraryMenuButton*>(CurrentlyFocusedElement)->Selected = true;
    };
  } else if (State == Hiding && FadeAnimation.Progress == 0.0f &&
             ScrWork[SW_SYSSUBMENUCT] == 0) {
    State = Hidden;
    MainItems.IsShown = false;
    IsFocused = false;
    if (UI::FocusedMenu) UI::FocusedMenu->IsFocused = true;
  }
}

void LibraryMenu::Render() {
  if (State != Hidden && ScrWork[SW_SYSSUBMENUCT] > 0 &&
      ScrWork[SW_SYSSUBMENUNO] == 8) {
    const glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);

    const glm::vec2 leftSpritesOffset{
        (1.0f - FadeAnimation.Progress) * -LibraryTransitionPositionOffset,
        0.0f};

    const glm::vec2 rightSpritesOffset{
        (1.0f - FadeAnimation.Progress) * LibraryTransitionPositionOffset,
        0.0f};

    const float submenuFadeProgress =
        GetMenuFromType(CurrentLibraryMenu).FadeAnimation.Progress;
    const glm::vec4 libBgCol =
        (CurrentLibraryMenu == +LibraryMenuPageType::Sound)
            ? col * glm::vec4{glm::vec3(1.0f), 1 - submenuFadeProgress}
            : col;
    Renderer->DrawSprite(LibraryBackgroundSprite,
                         LibraryBackgroundPosition + rightSpritesOffset,
                         libBgCol);
    GetMenuFromType(LibraryMenuPageType::Album).Render();
    GetMenuFromType(LibraryMenuPageType::Sound).Render();
    GetMenuFromType(LibraryMenuPageType::Movie).Render();
    auto* albumMenuPtr = static_cast<AlbumMenu*>(UI::AlbumMenuPtr);
    const bool cgViewerActive =
        CurrentLibraryMenu == +LibraryMenuPageType::Album &&
        albumMenuPtr->CGViewer;
    Renderer->DrawSprite(LibraryIndexSprite,
                         LibraryIndexPosition + leftSpritesOffset, col);
    MainItems.Render();
    Renderer->DrawSprite(
        LibraryMaskSprite,
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
        glm::vec4(1.0f, 1.0f, 1.0f, FadeAnimation.Progress * LibraryMaskAlpha));

    const auto* const submenuGuideSprite = [&]() -> Sprite* {
      switch (CurrentLibraryMenu) {
        case LibraryMenuPageType::Album:
          if (cgViewerActive && albumMenuPtr->CGViewer->EnableGuide)
            return &AlbumMenuCGViewerGuideSprite;
          else
            return &AlbumMenuGuideSprite;
        case LibraryMenuPageType::Sound:
          return &MusicMenuGuideSprite;
        case LibraryMenuPageType::Movie:
          return &MovieMenuGuideSprite;
        default:
          return nullptr;
      }
    }();
    if (cgViewerActive) {
      albumMenuPtr->RenderCGViewer();
    }

    if (submenuGuideSprite) {
      Renderer->DrawSprite(
          *submenuGuideSprite, LibraryButtonGuidePosition + leftSpritesOffset,
          col * glm::vec4{glm::vec3(1.0f), submenuFadeProgress});
    }

    // This is technically a double render but menus always render after videos
    // so what can you do.
    if (CurrentLibraryMenu == +LibraryMenuPageType::Movie &&
        Video::Players[0]->IsPlaying) {
      Video::VideoRender(1);
    }
  }
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto