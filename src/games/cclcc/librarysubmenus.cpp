#include "../../ui/ui.h"
#include "../../profile/games/cclcc/librarymenu.h"
#include "../../video/videosystem.h"
#include "librarysubmenus.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

using namespace Impacto::Profile::CCLCC::LibraryMenu;
using namespace Impacto::Profile::ScriptVars;

void LibrarySubmenu::Show() {
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    MainItems.Children[0]->Enabled = true;
    MainItems.Children[1]->Enabled = GetFlag(SF_CLR_TRUE_CCLCC);
    MainItems.Children[2]->Enabled = [&]() {
      for (int i = 1; i < 10; ++i) {
        if (GetFlag(SF_CLR_END1 + i)) return true;
      }
      return false;
    }();
    MainItems.Children[3]->Enabled = GetFlag(SF_CLR_END1);
    MainItems.Show();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    if (!CurrentlyFocusedElement) {
      CurrentlyFocusedElement = MainItems.GetFirstFocusableChild();
    }
    IsFocused = true;
  }
}
void LibrarySubmenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    FadeAnimation.StartOut();
    MainItems.Hide();
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void LibrarySubmenu::Update(float dt) {
  FadeAnimation.Update(dt);
  MainItems.Update(dt);
  UpdateInput();
  if (CurrentlyFocusedElement) {
    CurrentlyFocusedElement->Update(dt);
  }
  if (FadeAnimation.IsOut()) {
    IsFocused = false;
    State = Hidden;
  }
  if (FadeAnimation.IsIn()) {
    IsFocused = true;
    State = Shown;
  }
}

void LibrarySubmenu::Render() {
  if (State != Hidden) {
    MainItems.Tint = glm::vec4(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    MainItems.Render();
  }
}

MovieMenu::MovieMenu() : LibrarySubmenu() {
  for (int i = 0; i < MovieDiskSprites.size(); ++i) {
    const auto& diskSprite = MovieDiskSprites[i];
    const auto& diskHighlightSprite = MovieDiskHighlightSprites[i];
    auto movieOnclick = [this, i](Widgets::Button* target) {
      ScrWork[SW_MOVIEMODE_CUR] = MovieDiskPlayIds[i];
      LibraryMenuPtr->AllowsScriptInput = true;
    };
    auto disk =
        new Widgets::Button(i, diskSprite, Sprite(), diskHighlightSprite,
                            MovieDiskDisplayPositions[i]);
    disk->OnClickHandler = movieOnclick;
    MainItems.Add(disk, FDIR_RIGHT);
  }
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto