#include "optionsmenu.h"

#include "../../profile/ui/optionsmenu.h"
#include "../../profile/games/mo6tw/optionsmenu.h"
#include "../../renderer2d.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../inputsystem.h"
#include "../../ui/widgets/button.h"
#include "../../vm/vm.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

using namespace Impacto::Profile::OptionsMenu;
using namespace Impacto::Profile::MO6TW::OptionsMenu;
using namespace Impacto::Profile::ScriptVars;

void OptionsMenu::MenuButtonOnClick(Widgets::Button* target) {}

OptionsMenu::OptionsMenu() {
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
  MainScrollbar =
      new Widgets::Scrollbar(0, glm::vec2(100.0f, 100.0f), 0.0f, 1.0f, &Test,
                             Widgets::SBDIR_HORIZONTAL, SliderTrackSprite,
                             SliderThumbSprite, SliderFillSprite);
  MainItems = new WidgetGroup();
  glm::vec2 pos = VoiceToggleStart;
  for (int i = 0; i < VoiceToggleCount; i++) {
    auto toggle = new Widgets::Toggle(
        1, &VoiceTest[i], VoiceToggleEnabledSprites[i],
        VoiceToggleDisabledSprites[i], VoiceToggleHighlightSprite, pos, false);
    toggle->Enabled = true;
    if ((i + 1) % VoiceTogglePerLine == 0) {
      pos.x = VoiceToggleStart.x;
      pos.y += VoiceTogglePadding.y;
    } else {
      pos.x += VoiceTogglePadding.x;
    }
    MainItems->Add(toggle, FocusDirection::Horizontal);
  }
  MainItems->Show();
}

void OptionsMenu::Show() {
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}
void OptionsMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    FadeAnimation.StartOut();
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void OptionsMenu::Update(float dt) {
  FadeAnimation.Update(dt);
  MainScrollbar->Update(dt);
  MainScrollbar->UpdateInput();
  MainItems->Update(dt);
  if (ScrWork[SW_OPTIONALPHA] < 256 && State == Shown) {
    Hide();
  } else if (ScrWork[SW_OPTIONALPHA] == 256 && State == Hidden) {
    Show();
  }

  if (ScrWork[SW_OPTIONALPHA] == 256 && FadeAnimation.IsIn())
    State = Shown;
  else if (ScrWork[SW_OPTIONALPHA] == 0 && FadeAnimation.IsOut())
    State = Hidden;
}

void OptionsMenu::Render() {
  if (State != Hidden) {
    Renderer2D::DrawSprite(BackgroundSprite, glm::vec2(0.0f, 0.0f));
    MainScrollbar->Render();
    MainItems->Render();
  }
}

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto