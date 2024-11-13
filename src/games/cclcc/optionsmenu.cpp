#include "optionsmenu.h"

#include "../../profile/ui/optionsmenu.h"
#include "../../profile/games/cclcc/optionsmenu.h"
#include "../../profile/scriptinput.h"
#include "../../vm/interface/input.h"
#include "../../ui/widgets/cclcc/optionsbinarybutton.h"
#include "../../ui/widgets/cclcc/optionsslider.h"
#include "../../ui/widgets/cclcc/optionsvoiceslider.h"

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
  for (int i = 0; i < 4; i++) {
    BasicPage->Add(new OptionsBinaryButton(BinaryBoxSprite, OnSprite, OffSprite,
                                           LabelSprites[i], pos, highlightTint),
                   FDIR_DOWN);

    pos.y += EntriesVerticalOffset;
  }
  Pages.push_back(BasicPage);

  pos = EntriesStartPosition;
  TextPage = new Group(this);
  for (int i = 4; i < 6; i++) {
    TextPage->Add(new OptionsSlider(SliderTrackSprite, LabelSprites[i], pos,
                                    highlightTint, SliderSpeed),
                  FDIR_DOWN);

    pos.y += EntriesVerticalOffset;
  }
  TextPage->Add(new OptionsBinaryButton(BinaryBoxSprite, YesSprite, NoSprite,
                                        LabelSprites[6], pos, highlightTint),
                FDIR_DOWN);
  Pages.push_back(TextPage);

  pos = SoundEntriesStartPosition;
  SoundPage = new Group(this);
  for (int i = 7; i < 15; i++) {
    Widget* widget = (i < 11 || i == 14)
                         ? new OptionsSlider(SliderTrackSprite, LabelSprites[i],
                                             pos, highlightTint, SliderSpeed)
                         : widget = new OptionsBinaryButton(
                               BinaryBoxSprite, YesSprite, NoSprite,
                               LabelSprites[i], pos, highlightTint);
    SoundPage->Add(widget, FDIR_DOWN);

    pos.y += SoundEntriesVerticalOffset;
  }
  Pages.push_back(SoundPage);

  VoicePage = new Group(this);
  constexpr int columns = 3;
  constexpr int entries = 12;
  for (int i = 0; i < entries; i++) {
    glm::vec2 pos = VoicePosition;
    pos += VoiceEntriesOffset * glm::vec2(i % columns, i / columns);

    Widget* widget = new OptionsVoiceSlider(
        VoiceSliderTrackSprite, NametagSprites[i], PortraitSprites[2 * i],
        PortraitSprites[2 * i + 1], pos, highlightTint, SliderSpeed);
    VoicePage->Add(widget, FDIR_RIGHT);
  }

  // Loop separately to overwrite the direction set at initial adding
  // First entry won't set anything; skip
  for (int i = 1; i < entries; i++) {
    Widget* const widget = VoicePage->Children.at(i);

    if (i % columns != 0) {  // Not on first column
      Widget* const leftWidget = VoicePage->Children.at(i - 1);
      widget->SetFocus(leftWidget, FDIR_LEFT);
      leftWidget->SetFocus(widget, FDIR_RIGHT);

      if (i % columns == columns - 1) {  // On last column
        Widget* const rowStart = VoicePage->Children.at(i - columns + 1);
        widget->SetFocus(rowStart, FDIR_RIGHT);
        rowStart->SetFocus(widget, FDIR_LEFT);
      }
    }
    if (i >= columns) {  // Not on first row
      Widget* const upWidget = VoicePage->Children.at(i - columns);
      widget->SetFocus(upWidget, FDIR_UP);
      upWidget->SetFocus(widget, FDIR_DOWN);

      if (i >= entries - columns) {  // On last layer
        Widget* const columnStart = VoicePage->Children.at(i % columns);
        widget->SetFocus(columnStart, FDIR_DOWN);
        columnStart->SetFocus(widget, FDIR_UP);
      }
    }
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
  Pages.at(CurrentPage)->Update(dt);

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

void OptionsMenu::UpdateInput() {
  bool nothingSelected =
      CurrentlyFocusedElement == nullptr ||
      !static_cast<OptionsEntry*>(CurrentlyFocusedElement)->Selected;
  if (nothingSelected && GetControlState(CT_Back)) {
    SetFlag(SF_SUBMENUEXIT, true);
    return;
  }

  // Only able to move up and down if nothing is selected
  if (nothingSelected) Menu::UpdateInput();
}

void OptionsMenu::Render() {
  if (State != Hidden && ScrWork[SW_SYSSUBMENUCT] >= 32 &&
      ScrWork[SW_SYSSUBMENUNO] == 5) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);

    Renderer->DrawSprite(BackgroundSprite, BackgroundPosition, col);
    Renderer->DrawSprite(HeaderSprite, HeaderPosition, col);

    Renderer->DrawSprite(PageHeaderSprites[CurrentPage], PageHeaderPosition,
                         col);
    Pages.at(CurrentPage)->Render();

    Renderer->DrawSprite(PoleAnimation.CurrentSprite(), PagePanelPosition, col);

    const Sprite& guideSprite =
        CurrentPage == 3 ? VoiceGuideSprite : GuideSprite;
    Renderer->DrawSprite(guideSprite, GuidePosition, col);
  }
}

void OptionsMenu::GoToPage(int pageNumber) {
  if (CurrentPage == pageNumber) return;

  Pages.at(CurrentPage)->Hide();

  CurrentPage = pageNumber;
  Group& page = *Pages.at(CurrentPage);

  page.HasFocus = true;
  page.Show();

  if (CurrentlyFocusedElement) CurrentlyFocusedElement->Hide();
  CurrentlyFocusedElement = page.GetFirstFocusableChild();
  CurrentlyFocusedElement->HasFocus = true;
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto