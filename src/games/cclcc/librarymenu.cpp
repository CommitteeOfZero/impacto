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

LibrarySubmenu& LibraryMenu::GetMenuFromType(LibraryMenuPageType menuType) {
  switch (menuType) {
    case LibraryMenuPageType::Album:
      return AlbumMenu;
    case LibraryMenuPageType::Sound:
      return MusicMenu;
    case LibraryMenuPageType::Movie:
      return MovieMenu;
  }
}

LibraryMenu::LibraryMenu() : MainItems(this) {
  auto readyExistingMenu = [this]() -> bool {
    if (!CurrentLibraryMenu) return true;
    auto& submenu = GetMenuFromType(*CurrentLibraryMenu);
    if (submenu.State != UI::MenuState::Shown) return false;
    submenu.Hide();
    return true;
  };
  auto libraryMenuOnClickCommon = [this,
                                   readyExistingMenu](Widgets::Button* target) {
    auto clickedType = LibraryMenuPageType::_from_integral(target->Id);
    if (CurrentLibraryMenu == clickedType) return;
    if (!readyExistingMenu()) return;
    GetMenuFromType(clickedType).Show();
    auto* button = static_cast<LibraryMenuButton*>(target);
    if (CurrentLibraryMenu) {
      auto* prevButton = static_cast<LibraryMenuButton*>(
          MainItems.Children.at(+*CurrentLibraryMenu));
      prevButton->Selected = false;
    }
    Audio::Channels[Audio::AC_SSE]->Play("sysse", 2, false, 0);
    CurrentLibraryMenu = clickedType;
    AllowsScriptInput = false;
    button->Selected = true;
  };

  auto* album = new LibraryMenuButton(0, SnapPhotoSpriteHover,
                                      SnapPhotoSpriteSelect, SnapPhotoPos);
  album->OnClickHandler = libraryMenuOnClickCommon;

  auto* sound = new LibraryMenuButton(1, HitSongsSpriteHover,
                                      HitSongsSpriteSelect, HitSongsPos);
  sound->OnClickHandler = libraryMenuOnClickCommon;
  auto* movie = new LibraryMenuButton(2, LoveMovieSpriteHover,
                                      LoveMovieSpriteSelect, LoveMoviePos);
  movie->OnClickHandler = libraryMenuOnClickCommon;

  MainItems.Add(album, FDIR_DOWN);
  MainItems.Add(sound, FDIR_DOWN);
  MainItems.Add(movie, FDIR_DOWN);

  FocusStart[FDIR_DOWN] = album;
  FocusStart[FDIR_UP] = movie;

  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
}

void LibraryMenu::Show() {
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    MainItems.Show();
    UI::FocusedMenu = this;
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
    IsFocused = false;
  }
}

void LibraryMenu::Update(float dt) {
  if (ScrWork[SW_SYSSUBMENUCT] < 32 && State == Shown) {
    Hide();
  } else if (ScrWork[SW_SYSSUBMENUCT] > 0 && State == Hidden &&
             (ScrWork[SW_SYSSUBMENUNO] == 8)) {
    Show();
  }
  if (State == Shown && ScrWork[SW_SYSSUBMENUNO] == 8) {
    UpdateInput();
    if ((Vm::Interface::PADinputButtonWentDown & Vm::Interface::PAD1B) ||
        (Vm::Interface::PADinputMouseWentDown & Vm::Interface::PAD1B)) {
      if (CurrentLibraryMenu) {
        auto* activeButton = static_cast<LibraryMenuButton*>(
            MainItems.Children.at(*CurrentLibraryMenu));
        auto& submenu = GetMenuFromType(*CurrentLibraryMenu);
        if (submenu.State == UI::MenuState::Shown) {
          submenu.Hide();
          activeButton->Selected = false;
        }
      } else {
        SetFlag(SF_ALBUMEND, 1);
      }
    }
  }
  FadeAnimation.Update(dt);
  MainItems.Update(dt);
  AlbumMenu.Update(dt);
  MusicMenu.Update(dt);
  MovieMenu.Update(dt);

  if (State == Shown && IsFocused) {
    if (CurrentLibraryMenu) {
      auto& submenu = GetMenuFromType(*CurrentLibraryMenu);
      if (submenu.State == UI::MenuState::Hidden)
        CurrentLibraryMenu = std::nullopt;
    }
  }

  if (State == Showing && FadeAnimation.Progress == 1.0f &&
      ScrWork[SW_SYSSUBMENUCT] == 32) {
    State = Shown;
    IsFocused = true;
  } else if (State == Hiding && FadeAnimation.Progress == 0.0f &&
             ScrWork[SW_SYSSUBMENUCT] == 0) {
    State = Hidden;
    IsFocused = false;
    if (UI::FocusedMenu) UI::FocusedMenu->IsFocused = true;
  }
}

void LibraryMenu::Render() {
  if (State != Hidden && ScrWork[SW_SYSSUBMENUCT] >= 32 &&
      ScrWork[SW_SYSSUBMENUNO] == 8) {
    const glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    const glm::vec4 maskTint = glm::vec4(1.0f);
    const float submenuFadeProgress =
        CurrentLibraryMenu
            ? GetMenuFromType(*CurrentLibraryMenu).FadeAnimation.Progress
            : 0;
    const glm::vec4 libBgCol =
        (CurrentLibraryMenu == +LibraryMenuPageType::Sound)
            ? col * glm::vec4{glm::vec3(1.0f), 1 - submenuFadeProgress}
            : col;
    Renderer->DrawSprite(LibraryBackgroundSprite, LibraryBackgroundPosition,
                         libBgCol);
    if (CurrentLibraryMenu) {
      auto& submenu = GetMenuFromType(*CurrentLibraryMenu);
      submenu.Render();
    }
    Renderer->DrawSprite(LibraryIndexSprite, LibraryIndexPosition, col);
    MainItems.Render();
    Renderer->DrawSprite(
        LibraryMaskSprite,
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
        maskTint);

    const Sprite* submenuGuideSprite =
        (CurrentLibraryMenu == +LibraryMenuPageType::Album)
            ? &AlbumMenuGuideSprite
        : (CurrentLibraryMenu == +LibraryMenuPageType::Sound)
            ? &MusicMenuGuideSprite
        : (CurrentLibraryMenu == +LibraryMenuPageType::Movie)
            ? &MovieMenuGuideSprite
            : nullptr;

    if (submenuGuideSprite) {
      Renderer->DrawSprite(
          *submenuGuideSprite, LibraryButtonGuidePosition,
          col * glm::vec4{glm::vec3(1.0f), submenuFadeProgress});
    }
    Renderer->DrawSprite(
        LibraryButtonGuideSprite, LibraryButtonGuidePosition,
        col * glm::vec4{glm::vec3(1.0f), 1 - submenuFadeProgress});

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