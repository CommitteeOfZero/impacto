#include "optionsmenu.h"

#include "../../profile/ui/optionsmenu.h"
#include "../../profile/games/cclcc/optionsmenu.h"
#include "../../profile/scriptinput.h"
#include "../../vm/interface/input.h"
#include "../../ui/widgets/cclcc/optionsbinarybutton.h"
#include "../../ui/widgets/cclcc/optionsslider.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

using namespace Impacto::Profile::OptionsMenu;
using namespace Impacto::Profile::CCLCC::OptionsMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::UI::Widgets;
using namespace Impacto::UI::Widgets::CCLCC;
using namespace Impacto::Vm::Interface;

OptionsMenu::OptionsMenu() {
  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  PoleAnimation = Profile::CCLCC::OptionsMenu::PoleAnimation.Instantiate();

  Pages.reserve(4);
  glm::vec2 pos = EntriesStartPosition;
  glm::vec4 highlightTint(HighlightColor, 1.0f);

  BasicPage = new Group(this);
  BasicPage->FocusLock = false;
  for (int i = 0; i < 4; i++) {
    BasicPage->Add(new OptionsBinaryButton(BinaryBoxSprite, OnSprite, OffSprite,
                                           LabelSprites[i], pos, highlightTint),
                   FDIR_DOWN);

    pos.y += EntriesVerticalOffset;
  }
  Pages.push_back(BasicPage);

  pos = EntriesStartPosition;
  TextPage = new Group(this);
  TextPage->FocusLock = false;
  for (int i = 4; i < 6; i++) {
    TextPage->Add(new OptionsSlider(SliderTrackSprite, LabelSprites[i], pos,
                                    highlightTint),
                  FDIR_DOWN);

    pos.y += EntriesVerticalOffset;
  }
  TextPage->Add(new OptionsBinaryButton(BinaryBoxSprite, YesSprite, NoSprite,
                                        LabelSprites[6], pos, highlightTint),
                FDIR_DOWN);
  Pages.push_back(TextPage);

  pos = SoundEntriesStartPosition;
  SoundPage = new Group(this);
  SoundPage->FocusLock = false;
  for (int i = 7; i < 15; i++) {
    Widget* widget = (i < 11 || i == 14)
                         ? new OptionsSlider(SliderTrackSprite, LabelSprites[i],
                                             pos, highlightTint)
                         : widget = new OptionsBinaryButton(
                               BinaryBoxSprite, YesSprite, NoSprite,
                               LabelSprites[i], pos, highlightTint);
    SoundPage->Add(widget, FDIR_DOWN);

    pos.y += SoundEntriesVerticalOffset;
  }
  Pages.push_back(SoundPage);

  VoicePage = new Group(this);
  VoicePage->FocusLock = false;
  for (int i = 0; i < 12; i++) {
    glm::vec2 pos = VoicePosition;
    pos += VoiceEntriesOffset * glm::vec2(i % 3, i / 3);

    VoicePage->Add(new Label(NametagSprites[i], pos), FDIR_RIGHT);
  }
  Pages.push_back(VoicePage);

  CurrentPage = 0;
}

void OptionsMenu::Show() {
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    PoleAnimation.StartIn();

    Pages.at(CurrentPage)->Show();

    if (UI::FocusedMenu != nullptr) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;

    CurrentlyFocusedElement = Pages.at(CurrentPage)->GetFirstFocusableChild();
    CurrentlyFocusedElement->HasFocus = true;
  }
}

void OptionsMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    FadeAnimation.StartOut();
    PoleAnimation.StartOut();

    if (LastFocusedMenu != nullptr) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = nullptr;
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

  int newPageOffset = -(int)(bool)(PADinputButtonWentDown & PAD1L1) +
                      (int)(bool)(PADinputButtonWentDown & PAD1R1);
  GoToPage((CurrentPage + newPageOffset) % Pages.size());
}

void OptionsMenu::Render() {
  if (State != Hidden && ScrWork[SW_SYSSUBMENUCT] >= 32 &&
      ScrWork[SW_SYSSUBMENUNO] == 5) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);

    Renderer->DrawSprite(BackgroundSprite, BackgroundPosition, col);
    Renderer->DrawSprite(HeaderSprite, HeaderPosition, col);

    Renderer->DrawSprite(PageHeaderSprites[+CurrentPage], PageHeaderPosition,
                         col);
    Pages.at(CurrentPage)->Render();
    /*
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
    */

    Renderer->DrawSprite(PoleAnimation.CurrentSprite(), PagePanelPosition, col);

    Renderer->DrawSprite(GuideSprite, GuidePosition, col);
  }
}

void OptionsMenu::GoToPage(int pageNumber) {
  if (CurrentPage == pageNumber) return;

  Pages.at(CurrentPage)->HasFocus = false;

  CurrentPage = pageNumber;
  Group& page = *Pages.at(CurrentPage);

  page.HasFocus = true;
  page.Show();

  CurrentlyFocusedElement = page.GetFirstFocusableChild();
  CurrentlyFocusedElement->HasFocus = true;
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto