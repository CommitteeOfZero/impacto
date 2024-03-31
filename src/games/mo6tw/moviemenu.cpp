#include "moviemenu.h"

#include "../../profile/ui/extramenus.h"
#include "../../profile/games/mo6tw/moviemenu.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

using namespace Impacto::Profile::MO6TW::MovieMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets;

void MovieMenu::MovieButtonOnClick(Button* target) {
  auto movieButton = static_cast<Widgets::MO6TW::ImageThumbnailButton*>(target);
  if (!movieButton->IsLocked) {
    ScrWork[SW_MOVIEMODE_CUR] = movieButton->Id;
  }
}

MovieMenu::MovieMenu() {
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  MainItems = new Group(this);
  MainItems->WrapFocus = false;

  auto onClick =
      std::bind(&MovieMenu::MovieButtonOnClick, this, std::placeholders::_1);

  auto pos = InitialItemPosition;

  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);
  auto firstOp = new Widgets::MO6TW::ImageThumbnailButton(
      0, FirstOPTopPartSprite, FirstOPBottomPartSprite, nullSprite,
      SelectionHighlightTopLeft, SelectionHighlightTopRight,
      SelectionHighlightBottomLeft, SelectionHighlightBottomRight, pos);
  firstOp->OnClickHandler = onClick;
  MainItems->Add(firstOp, FDIR_RIGHT);

  pos.x += ItemOffset.x;

  auto secondOp = new Widgets::MO6TW::ImageThumbnailButton(
      1, SecondOPTopPartSprite, SecondOPBottomPartSprite, nullSprite,
      SelectionHighlightTopLeft, SelectionHighlightTopRight,
      SelectionHighlightBottomLeft, SelectionHighlightBottomRight, pos);
  secondOp->OnClickHandler = onClick;
  MainItems->Add(secondOp, FDIR_RIGHT);

  pos.x += ItemOffset.x;

  int row = 1;
  int totalRows = ItemCount / ItemsPerRow + (ItemCount % ItemsPerRow != 0);
  for (int i = 2; i < ItemCount; i++) {
    auto item = new Widgets::MO6TW::ImageThumbnailButton(
        i, UnlockedMovieThumbnailSprites[i - 2],
        LockedMovieThumbnailSprites[i - 2], SelectionHighlightTopLeft,
        SelectionHighlightTopRight, SelectionHighlightBottomLeft,
        SelectionHighlightBottomRight, pos);
    item->OnClickHandler = onClick;
    MainItems->Add(item, FDIR_RIGHT);
    if (row != 1) {
      item->SetFocus(MainItems->Children.at(i - ItemsPerRow), FDIR_UP);
    }

    if ((i + 1) % ItemsPerRow == 0) {
      row += 1;
      pos.x = InitialItemPosition.x;
      pos.y += ItemOffset.y;
    } else {
      pos.x += ItemOffset.x;
    }
  }

  row = 1;
  int idx = 0;
  for (const auto& el : MainItems->Children) {
    if (row != totalRows) {
      Widget* focusTarget;
      if ((idx + ItemsPerRow) > MainItems->Children.size() - 1)
        focusTarget = MainItems->Children.back();
      else
        focusTarget = MainItems->Children.at(idx + ItemsPerRow);
      el->SetFocus(focusTarget, FDIR_DOWN);
    }

    idx += 1;
    if (idx % ItemsPerRow == 0) {
      row += 1;
    }
  }
}

void MovieMenu::Show() {
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    MainItems->Show();
    UpdateMovieEntries();
    ScrWork[SW_MOVIEMODE_CUR] = 255;
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}
void MovieMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    FadeAnimation.StartOut();
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

void MovieMenu::UpdateInput() {
  Menu::UpdateInput();
  if (State == Shown) {
    MainItems->UpdateInput();
  }
}

void MovieMenu::Update(float dt) {
  UpdateInput();

  FadeAnimation.Update(dt);
  if (ScrWork[SW_MOVIEMODE_ALPHA] < 256 && State == Shown) {
    Hide();
  } else if (ScrWork[SW_MOVIEMODE_ALPHA] == 256 && State == Hidden) {
    Show();
  }

  if (ScrWork[SW_MOVIEMODE_ALPHA] == 256 && FadeAnimation.IsIn())
    State = Shown;
  else if (FadeAnimation.IsOut()) {
    State = Hidden;
  }

  if (State != Hidden) {
    MainItems->Update(dt);
  }
}

void MovieMenu::Render() {
  if (State != Hidden) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f, 0.0f), col);
    MainItems->Tint = col;
    MainItems->Render();
  }
}

void MovieMenu::UpdateMovieEntries() {
  for (auto el : MainItems->Children) {
    auto movieButton = static_cast<Widgets::MO6TW::ImageThumbnailButton*>(el);
    if (movieButton->Id == 0 || movieButton->Id == 1)
      movieButton->IsLocked = false;
    else
      movieButton->IsLocked = !GetFlag(SF_MOVIE_UNLOCK1 + movieButton->Id);
  }
}

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto