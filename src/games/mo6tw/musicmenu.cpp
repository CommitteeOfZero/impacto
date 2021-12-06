#include "musicmenu.h"

#include "../../profile/ui/extramenus.h"
#include "../../profile/games/mo6tw/musicmenu.h"
#include "../../renderer2d.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../data/savesystem.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

using namespace Impacto::Profile::MO6TW::MusicMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets;

void MusicMenu::MusicButtonOnClick(Button* target) {}

MusicMenu::MusicMenu() {
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  MainItems = new Group(this);
}

void MusicMenu::Show() {
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
void MusicMenu::Hide() {
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

void MusicMenu::UpdateInput() {
  Menu::UpdateInput();
  if (State == Shown) {
    MainItems->UpdateInput();
  }
}

void MusicMenu::Update(float dt) {
  UpdateInput();

  FadeAnimation.Update(dt);
  if (ScrWork[SW_MUSICMODE_ALPHA] < 256 && State == Shown) {
    Hide();
  } else if (ScrWork[SW_MUSICMODE_ALPHA] == 256 && State == Hidden) {
    Show();
  }

  if (ScrWork[SW_MUSICMODE_ALPHA] == 256 && FadeAnimation.IsIn())
    State = Shown;
  else if (FadeAnimation.IsOut()) {
    State = Hidden;
  }

  if (State != Hidden) {
    MainItems->Update(dt);
  }
}

void MusicMenu::Render() {
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