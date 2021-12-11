#include "albummenu.h"

#include "../../profile/ui/extramenus.h"
#include "../../profile/games/mo6tw/albummenu.h"
#include "../../renderer2d.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../data/savesystem.h"
#include "../../background2d.h"
#include "../../vm/interface/input.h"
#include "../../ui/widgets/mo6tw/albumcharacterbutton.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

using namespace Impacto::Profile::MO6TW::AlbumMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;

AlbumMenu::AlbumMenu() {
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  MainItems = new Group(this);
  auto pos = InitialButtonPosition;
  int idx = 0;
  for (int i = 0; i < CharacterButtonCount; i++) {
    if (idx % 2 == 0)
      pos.x = ButtonEvenX;
    else
      pos.x = ButtonOddX;

    Sprite *sprite, *lockedSprite, *highlightedSprite, *highlightedLockedSprite;
    if (idx == 3 || idx == 4) {
      sprite = &CharacterButtonSprites[i];
      lockedSprite = &CharacterButtonSprites[i + 1];
      highlightedSprite = &HighlightedCharacterButtonSprites[i];
      highlightedLockedSprite = &HighlightedCharacterButtonSprites[i + 1];
      i += 1;
    } else {
      sprite = &CharacterButtonSprites[i];
      lockedSprite = sprite;
      highlightedSprite = &HighlightedCharacterButtonSprites[i];
      highlightedLockedSprite = highlightedSprite;
    }
    MainItems->Add(
        new Widgets::MO6TW::AlbumCharacterButton(
            idx, *sprite, *lockedSprite, *highlightedSprite,
            *highlightedLockedSprite, pos, HighlightAnimationDuration),
        FDIR_DOWN);
    pos += ButtonMargin;
    idx += 1;
  }
}

void AlbumMenu::Show() {
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    MainItems->Show();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}
void AlbumMenu::Hide() {
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

void AlbumMenu::UpdateInput() {
  Menu::UpdateInput();
  if (State == Shown) {
    MainItems->UpdateInput();
    if (PADinputButtonWentDown & PAD1B || PADinputMouseWentDown & PAD1B) {
      SetFlag(SF_ALBUMEND, true);
    }
  }
}

void AlbumMenu::Update(float dt) {
  UpdateInput();

  FadeAnimation.Update(dt);
  if (ScrWork[SW_ALBUM_ALPHA] < 256 && State == Shown) {
    Hide();
  } else if (ScrWork[SW_ALBUM_ALPHA] == 256 && State == Hidden) {
    Show();
  }

  if (ScrWork[SW_ALBUM_ALPHA] == 256 && FadeAnimation.IsIn())
    State = Shown;
  else if (FadeAnimation.IsOut()) {
    State = Hidden;
  }

  if (State != Hidden) {
    MainItems->Update(dt);
  }
}

void AlbumMenu::Render() {
  if (State != Hidden) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    Renderer2D::DrawSprite(BackgroundSprite, glm::vec2(0.0f, 0.0f), col);
    MainItems->Tint = col;
    MainItems->Render();
  }
}

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto