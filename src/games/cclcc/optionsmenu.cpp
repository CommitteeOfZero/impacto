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

  Pages.reserve(PageCount);
  glm::vec2 pos = EntriesStartPosition;
  glm::vec4 highlightTint(HighlightColor, 1.0f);
  std::function<void(OptionsEntry*)> select =
      std::bind(&OptionsMenu::Select, this, std::placeholders::_1);

  PageButtons.reserve(PageCount);
  std::function<void(ClickButton*)> pageButtonOnClick =
      std::bind(&OptionsMenu::PageButtonOnClick, this, std::placeholders::_1);
  for (int i = 0; i < PageCount; i++) {
    PageButtons.push_back(
        ClickButton(i, PagePanelHoverBounds[i], pageButtonOnClick));
  }

  BasicPage = new Group(this);
  for (int i = 0; i < 4; i++) {
    BasicPage->Add(
        new OptionsBinaryButton(BinaryBoxSprite, OnSprite, OffSprite,
                                LabelSprites[i], pos, highlightTint, select),
        FDIR_DOWN);

    pos.y += EntriesVerticalOffset;
  }
  Pages.push_back(BasicPage);

  pos = EntriesStartPosition;
  TextPage = new Group(this);
  for (int i = 4; i < 6; i++) {
    TextPage->Add(new OptionsSlider(SliderTrackSprite, LabelSprites[i], pos,
                                    highlightTint, SliderSpeed, select),
                  FDIR_DOWN);

    pos.y += EntriesVerticalOffset;
  }
  TextPage->Add(
      new OptionsBinaryButton(BinaryBoxSprite, SkipReadSprite, SkipAllSprite,
                              LabelSprites[6], pos, highlightTint, select),
      FDIR_DOWN);
  Pages.push_back(TextPage);

  pos = SoundEntriesStartPosition;
  SoundPage = new Group(this);
  for (int i = 7; i < 15; i++) {
    Widget* widget =
        (i < 11 || i == 14)
            ? new OptionsSlider(SliderTrackSprite, LabelSprites[i], pos,
                                highlightTint, SliderSpeed, select)
            : widget = new OptionsBinaryButton(BinaryBoxSprite, YesSprite,
                                               NoSprite, LabelSprites[i], pos,
                                               highlightTint, select);
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
        PortraitSprites[2 * i + 1], pos, highlightTint, SliderSpeed, select);
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
  const bool wasOut = FadeAnimation.IsOut();

  FadeAnimation.Update(dt);
  PoleAnimation.Update(dt);
  if (ScrWork[SW_SYSSUBMENUCT] < 32 && State == Shown &&
      ScrWork[SW_SYSSUBMENUNO] == 5) {
    Hide();
  } else if (ScrWork[SW_SYSSUBMENUCT] >= 32 && State == Hidden &&
             ScrWork[SW_SYSSUBMENUNO] == 5) {
    Show();
  }

  if (State != Hidden) {
    UpdateInput();
    Pages.at(CurrentPage)->Update(dt);
  }

  if (FadeAnimation.IsIn())
    State = Shown;
  else if (!wasOut && FadeAnimation.IsOut()) {
    State = Hidden;

    Pages.at(CurrentPage)->Hide();
    CurrentPage = 0;
    CurrentlyFocusedElement = nullptr;
  }
}

void OptionsMenu::UpdateInput() {
  for (ClickButton& button : PageButtons) {
    const bool wasHovered = button.Hovered;
    button.UpdateInput();
    if (!wasHovered && button.Hovered)
      Audio::Channels[Audio::AC_REV]->Play("sysse", 1, false, 0.0f);
  }

  // Tab cycling
  if (PADinputButtonWentDown & (PAD1L1 | PAD1R1)) {
    Audio::Channels[Audio::AC_REV]->Play("sysse", 1, false, 0.0f);

    const bool focusedElement = CurrentlyFocusedElement;
    const int direction = (bool)(PADinputButtonWentDown & PAD1R1) -
                          (bool)(PADinputButtonWentDown & PAD1L1);
    GoToPage((CurrentPage + direction) % Pages.size());

    if (focusedElement) {
      CurrentlyFocusedElement = Pages.at(CurrentPage)->GetFirstFocusableChild();
      CurrentlyFocusedElement->HasFocus = true;
    }
  }

  if (CurrentlyFocusedElement == nullptr) {
    if (GetControlState(CT_Back)) {
      if (!GetFlag(SF_SUBMENUEXIT))
        Audio::Channels[Audio::AC_REV]->Play("sysse", 3, false, 0.0f);

      SetFlag(SF_SUBMENUEXIT, true);
      return;
    }

    const int direction = (bool)(PADinputButtonWentDown & PAD1DOWN) -
                          (bool)(PADinputButtonWentDown & PAD1UP);
    if (direction) {
      Audio::Channels[Audio::AC_REV]->Play("sysse", 1, false, 0.0f);
      GoToPage((CurrentPage + direction) % Pages.size());
    }

    if (PADinputButtonWentDown & PAD1A) {
      // Don't have anything else consume the confirmation
      PADinputButtonWentDown &= ~PAD1A;

      Audio::Channels[Audio::AC_REV]->Play("sysse", 2, false, 0.0f);

      CurrentlyFocusedElement = Pages.at(CurrentPage)->GetFirstFocusableChild();
      CurrentlyFocusedElement->HasFocus = true;
    }

    return;
  }

  // If something is selected, the option entry takes full control
  if (static_cast<OptionsEntry*>(CurrentlyFocusedElement)->Selected) return;

  if (GetControlState(CT_Back) || PADinputMouseWentDown & PAD1B) {
    Audio::Channels[Audio::AC_REV]->Play("sysse", 3, false, 0.0f);

    static_cast<OptionsEntry*>(CurrentlyFocusedElement)->Hide();
    CurrentlyFocusedElement = nullptr;
    return;
  }

  Menu::UpdateInput();
  if (PADinputButtonWentDown & (PAD1DOWN | PAD1UP))
    Audio::Channels[Audio::AC_REV]->Play("sysse", 1, false, 0.0f);
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
    if (PoleAnimation.IsIn()) {
      for (ClickButton& panel : PageButtons) {
        if (panel.Id == CurrentPage || panel.Hovered) {
          const bool highlighted =
              panel.Id == CurrentPage && (bool)CurrentlyFocusedElement;
          Renderer->DrawSprite(
              PagePanelSprites[2 * panel.Id + !highlighted],
              PagePanelPosition + PagePanelIconOffsets[panel.Id], col);
        }
      }
    }

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
}

void OptionsMenu::Select(OptionsEntry* toSelect) {
  for (Widget* entry : Pages.at(CurrentPage)->Children) {
    const bool select = entry == toSelect;

    static_cast<OptionsEntry*>(entry)->Selected = select;
    entry->HasFocus = select;
  }

  CurrentlyFocusedElement = toSelect;
}

void OptionsMenu::PageButtonOnClick(ClickButton* target) {
  if (target->Id != CurrentPage || !CurrentlyFocusedElement)
    Audio::Channels[Audio::AC_REV]->Play("sysse", 2, false, 0.0f);

  if (target->Id == CurrentPage && CurrentlyFocusedElement) return;

  GoToPage(target->Id);
  CurrentlyFocusedElement = Pages.at(CurrentPage)->GetFirstFocusableChild();
  CurrentlyFocusedElement->HasFocus = true;
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto