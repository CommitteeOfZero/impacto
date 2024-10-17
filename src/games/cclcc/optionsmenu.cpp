#include "optionsmenu.h"

#include "../../profile/ui/optionsmenu.h"
#include "../../profile/games/cclcc/optionsmenu.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../inputsystem.h"
#include "../../vm/interface/input.h"
#include "../../ui/widgets/button.h"
#include "../../vm/vm.h"
#include "../../audio/audiochannel.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

using namespace Impacto::Profile::OptionsMenu;
using namespace Impacto::Profile::CCLCC::OptionsMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;

OptionsMenu::OptionsMenu() {
  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  PoleAnimation = Profile::CCLCC::OptionsMenu::PoleAnimation.Instantiate();
}

void OptionsMenu::Show() {
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    PoleAnimation.StartIn();
    // FirstPage->Show();
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
    PoleAnimation.StartOut();
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
  UpdateInput();

  FadeAnimation.Update(dt);
  PoleAnimation.Update(dt);
  if (ScrWork[SW_SYSSUBMENUCT] < 32 && State == Shown &&
      ScrWork[SW_SYSSUBMENUNO] == 5) {
    Hide();
  } else if (ScrWork[SW_SYSSUBMENUCT] >= 32 && State == Hidden &&
             ScrWork[SW_SYSSUBMENUNO] == 5) {
    Show();
  }
}

void OptionsMenu::Render() {
  if (State != Hidden && ScrWork[SW_SYSSUBMENUCT] >= 32 &&
      ScrWork[SW_SYSSUBMENUNO] == 5) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);

    Renderer->DrawSprite(BackgroundSprite, BackgroundPosition, col);
    Renderer->DrawSprite(HeaderSprite, HeaderPosition, col);

    Renderer->DrawSprite(PageHeaderSprites[2], PageHeaderPosition, col);
    for (int i = 7; i < 15; i++) {
      glm::vec2 pos = SoundEntriesStartPosition;
      pos.y += SoundEntriesVerticalOffset * (i - 7);

      Renderer->DrawSprite(LabelSprites[i], pos, col);

      if (11 <= i && i <= 13) {
        glm::vec2 boxPos = pos + BinaryBoxOffset;

        Renderer->DrawSprite(BinaryBoxSprite, boxPos, col);
        Renderer->DrawSprite(YesSprite, boxPos, col);
        Renderer->DrawSprite(
            NoSprite, boxPos + glm::vec2(BinaryBoxSprite.ScaledWidth() / 2, 0),
            col);
      } else {
        Renderer->DrawSprite(SliderTrackSprite, pos + SliderTrackOffset, col);
      }
    }

    Renderer->DrawSprite(PoleAnimation.CurrentSprite(), PagePanelPosition, col);

    Renderer->DrawSprite(GuideSprite, GuidePosition, col);
  }
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto