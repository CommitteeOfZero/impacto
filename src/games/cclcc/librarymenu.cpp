#include "librarymenu.h"

#include "clearlistmenu.h"

#include "../../profile/games/cclcc/librarymenu.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../profile/dialogue.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
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

LibraryMenu::LibraryMenu() : MainItems(this) {
  auto libraryMenuOnClick = [&](Widgets::Button* target) {
    auto* button = static_cast<LibraryMenuButton*>(target);
    auto* prevButton = static_cast<LibraryMenuButton*>(
        MainItems.Children.at(CurrentLibraryMenu));
    Audio::Channels[Audio::AC_SSE]->Play("sysse", 2, false, 0);
    prevButton->Selected = false;
    CurrentLibraryMenu = LibraryMenuPageType::_from_integral(target->Id);
    button->Selected = true;
  };

  auto* album = new LibraryMenuButton(0, SnapPhotoSpriteHover,
                                      SnapPhotoSpriteSelect, SnapPhotoPos);
  album->OnClickHandler = libraryMenuOnClick;

  auto* sound = new LibraryMenuButton(1, HitSongsSpriteHover,
                                      HitSongsSpriteSelect, HitSongsPos);
  sound->OnClickHandler = libraryMenuOnClick;
  auto* movie = new LibraryMenuButton(2, LoveMovieSpriteHover,
                                      LoveMovieSpriteSelect, LoveMoviePos);
  movie->OnClickHandler = libraryMenuOnClick;

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
  }
  FadeAnimation.Update(dt);
  MainItems.Update(dt);

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
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    glm::vec4 maskTint = glm::vec4(1.0f);
    if (CurrentLibraryMenu != +LibraryMenuPageType::Sound) {
      Renderer->DrawSprite(LibraryBackgroundSprite, LibraryBackgroundPosition,
                           col);
    }
    Renderer->DrawSprite(LibraryIndexSprite, LibraryIndexPosition, col);
    MainItems.Render();
    Renderer->DrawSprite(
        LibraryMaskSprite,
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
        maskTint);
    Renderer->DrawSprite(LibraryButtonGuideSprite, LibraryButtonGuidePosition,
                         col);
  }
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto