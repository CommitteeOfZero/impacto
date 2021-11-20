#include "optionsmenu.h"

#include "../../profile/ui/optionsmenu.h"
#include "../../profile/games/mo6tw/optionsmenu.h"
#include "../../renderer2d.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../inputsystem.h"
#include "../../ui/widgets/button.h"
#include "../../vm/vm.h"
#include "../../audio/audiochannel.h"

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
  FirstPage = new Widgets::Group(this);
  FirstPage->WrapFocus = false;
  CharacterVoiceToggles = new Widgets::Group(this);
  CharacterVoiceToggles->FocusLock = false;
  CharacterVoiceToggles->WrapFocus = false;

  VoiceVolumeSlider = new Widgets::Scrollbar(
      0, glm::vec2(94.0f, 88.0f), 0.0f, 1.0f,
      &Audio::GroupVolumes[Audio::ACG_Voice], Widgets::SBDIR_HORIZONTAL,
      SliderTrackSprite, SliderThumbSprite, SliderFillSprite);
  FirstPage->Add(VoiceVolumeSlider, FDIR_DOWN);
  BGMVolumeSlider = new Widgets::Scrollbar(
      0, glm::vec2(94.0f, 183.0f), 0.0f, 1.0f,
      &Audio::GroupVolumes[Audio::ACG_BGM], Widgets::SBDIR_HORIZONTAL,
      SliderTrackSprite, SliderThumbSprite, SliderFillSprite);
  FirstPage->Add(BGMVolumeSlider, FDIR_DOWN);
  SEVolumeSlider = new Widgets::Scrollbar(
      0, glm::vec2(94.0f, 280.0f), 0.0f, 1.0f,
      &Audio::GroupVolumes[Audio::ACG_SE], Widgets::SBDIR_HORIZONTAL,
      SliderTrackSprite, SliderThumbSprite, SliderFillSprite);
  FirstPage->Add(SEVolumeSlider, FDIR_DOWN);
  MovieVolumeSlider = new Widgets::Scrollbar(
      0, glm::vec2(94.0f, 376.0f), 0.0f, 1.0f,
      &Audio::GroupVolumes[Audio::ACG_Movie], Widgets::SBDIR_HORIZONTAL,
      SliderTrackSprite, SliderThumbSprite, SliderFillSprite);
  FirstPage->Add(MovieVolumeSlider, FDIR_DOWN);

  glm::vec2 pos = VoiceToggleStart;
  int row = 1;
  int totalRows = VoiceToggleCount / VoiceTogglePerLine +
                  (VoiceToggleCount % VoiceTogglePerLine != 0);
  for (int i = 0; i < VoiceToggleCount - 1; i++) {
    auto toggle = new Widgets::Toggle(
        1, &VoiceTest[i], VoiceToggleEnabledSprites[i],
        VoiceToggleDisabledSprites[i], VoiceToggleHighlightSprite, pos, false);
    toggle->Enabled = true;
    CharacterVoiceToggles->Add(toggle, FDIR_RIGHT);
    if (row == 1) {
      toggle->SetFocus(MovieVolumeSlider, FDIR_UP);
    } else {
      toggle->SetFocus(
          CharacterVoiceToggles->Children.at(i - VoiceTogglePerLine), FDIR_UP);
      if (row == totalRows) {
        toggle->SetFocus(VoiceVolumeSlider, FDIR_DOWN);
      }
    }

    if ((i + 1) % VoiceTogglePerLine == 0) {
      pos.x = VoiceToggleStart.x;
      pos.y += VoiceTogglePadding.y;
      row += 1;
    } else {
      pos.x += VoiceTogglePadding.x;
    }
  }
  row = 1;
  int idx = 0;
  for (auto el : CharacterVoiceToggles->Children) {
    if (row != totalRows) {
      Widget* focusTarget;
      if ((idx + VoiceTogglePerLine) >
          CharacterVoiceToggles->Children.size() - 1)
        focusTarget = CharacterVoiceToggles->Children.back();
      else
        focusTarget =
            CharacterVoiceToggles->Children.at(idx + VoiceTogglePerLine);
      el->SetFocus(focusTarget, FDIR_DOWN);
    }

    idx += 1;
    if (idx % VoiceTogglePerLine == 0) {
      row += 1;
    }
  }
  VoiceVolumeSlider->SetFocus(CharacterVoiceToggles->Children.back(), FDIR_UP);
  MovieVolumeSlider->SetFocus(CharacterVoiceToggles->Children.front(),
                              FDIR_DOWN);
}

void OptionsMenu::Show() {
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    FirstPage->Show();
    CharacterVoiceToggles->Show();
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
    CurrentlyFocusedElement->HasFocus = false;
    CurrentlyFocusedElement = 0;
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
  FirstPage->Update(dt);
  CharacterVoiceToggles->Update(dt);
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
    if (FirstPage->IsShown) {
      if (VoiceVolumeSlider->HasFocus) {
        Renderer2D::DrawSprite(SectionHeaderHighlightedSprites[0],
                               glm::vec2(79.0f, 47.0f));
      } else {
        Renderer2D::DrawSprite(SectionHeaderSprites[0],
                               glm::vec2(79.0f, 47.0f));
      }
      if (BGMVolumeSlider->HasFocus) {
        Renderer2D::DrawSprite(SectionHeaderHighlightedSprites[1],
                               glm::vec2(79.0f, 143.0f));
      } else {
        Renderer2D::DrawSprite(SectionHeaderSprites[1],
                               glm::vec2(79.0f, 143.0f));
      }
      if (SEVolumeSlider->HasFocus) {
        Renderer2D::DrawSprite(SectionHeaderHighlightedSprites[2],
                               glm::vec2(79.0f, 239.0f));
      } else {
        Renderer2D::DrawSprite(SectionHeaderSprites[2],
                               glm::vec2(79.0f, 239.0f));
      }
      if (MovieVolumeSlider->HasFocus) {
        Renderer2D::DrawSprite(SectionHeaderHighlightedSprites[3],
                               glm::vec2(79.0f, 336.0f));
      } else {
        Renderer2D::DrawSprite(SectionHeaderSprites[3],
                               glm::vec2(79.0f, 336.0f));
      }
      if (CharacterVoiceToggles->HasFocus) {
        Renderer2D::DrawSprite(SectionHeaderHighlightedSprites[4],
                               glm::vec2(79.0f, 431.0f));
      } else {
        Renderer2D::DrawSprite(SectionHeaderSprites[4],
                               glm::vec2(79.0f, 431.0f));
      }
    }
    FirstPage->Render();
    CharacterVoiceToggles->Render();
  }
}

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto