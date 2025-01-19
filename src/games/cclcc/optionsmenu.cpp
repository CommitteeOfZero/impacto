#include "optionsmenu.h"

#include "../../profile/game.h"
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

std::unique_ptr<Group> OptionsMenu::CreateBasicPage(
    const std::function<void(OptionsEntry*)>& select,
    const std::function<void(Widget*)>& highlight) {
  const glm::vec4 highlightTint(HighlightColor, 1.0f);
  std::unique_ptr<Group> basicPage = std::make_unique<Group>(this);

  basicPage->Add(
      new OptionsBinaryButton(ShowTips, BinaryBoxSprite, OnSprite, OffSprite,
                              LabelSprites[0], EntriesStartPosition,
                              highlightTint, select, highlight),
      FDIR_DOWN);
  basicPage->Add(
      new OptionsBinaryButton(
          TextLR, BinaryBoxSprite, OnSprite, OffSprite, LabelSprites[1],
          EntriesStartPosition + glm::vec2(0.0f, EntriesVerticalOffset),
          highlightTint, select, highlight),
      FDIR_DOWN);
  basicPage->Add(
      new OptionsBinaryButton(
          DelusionLR, BinaryBoxSprite, OnSprite, OffSprite, LabelSprites[2],
          EntriesStartPosition + glm::vec2(0.0f, EntriesVerticalOffset * 2),
          highlightTint, select, highlight),
      FDIR_DOWN);
  basicPage->Add(
      new OptionsBinaryButton(
          DelusionStopSkip, BinaryBoxSprite, OnSprite, OffSprite,
          LabelSprites[3],
          EntriesStartPosition + glm::vec2(0.0f, EntriesVerticalOffset * 3),
          highlightTint, select, highlight),
      FDIR_DOWN);

  return basicPage;
}

std::unique_ptr<Group> OptionsMenu::CreateTextPage(
    const std::function<void(OptionsEntry*)>& select,
    const std::function<void(Widget*)>& highlight) {
  const glm::vec4 highlightTint(HighlightColor, 1.0f);
  std::unique_ptr<Group> textPage = std::make_unique<Group>(this);

  textPage->Add(
      new OptionsSlider(TextSpeed, 0.0f, 1.0f, SliderTrackSprite,
                        LabelSprites[4], EntriesStartPosition, highlightTint,
                        SliderSpeed, select, highlight),
      FDIR_DOWN);
  textPage->Add(
      new OptionsSlider(
          AutoSpeed, 0.0f, 1.0f, SliderTrackSprite, LabelSprites[5],
          EntriesStartPosition + glm::vec2(0.0f, EntriesVerticalOffset),
          highlightTint, SliderSpeed, select, highlight),
      FDIR_DOWN);
  textPage->Add(
      new OptionsBinaryButton(
          SkipRead, BinaryBoxSprite, SkipReadSprite, SkipAllSprite,
          LabelSprites[6],
          EntriesStartPosition + glm::vec2(0.0f, EntriesVerticalOffset * 2),
          highlightTint, select, highlight),
      FDIR_DOWN);

  return textPage;
}

std::unique_ptr<Group> OptionsMenu::CreateSoundPage(
    const std::function<void(OptionsEntry*)>& select,
    const std::function<void(Widget*)>& highlight) {
  const glm::vec4 highlightTint(HighlightColor, 1.0f);
  std::unique_ptr<Group> soundPage = std::make_unique<Group>(this);

  soundPage->Add(
      new OptionsSlider(VoiceVolume, 0.0f, 1.0f, SliderTrackSprite,
                        LabelSprites[7], SoundEntriesStartPosition,
                        highlightTint, SliderSpeed, select, highlight),
      FDIR_DOWN);
  soundPage->Add(
      new OptionsSlider(MusicVolume, 0.0f, 1.0f, SliderTrackSprite,
                        LabelSprites[8],
                        SoundEntriesStartPosition +
                            glm::vec2(0.0f, SoundEntriesVerticalOffset),
                        highlightTint, SliderSpeed, select, highlight),
      FDIR_DOWN);
  soundPage->Add(
      new OptionsSlider(SoundEffectVolume, 0.0f, 1.0f, SliderTrackSprite,
                        LabelSprites[9],
                        SoundEntriesStartPosition +
                            glm::vec2(0.0f, SoundEntriesVerticalOffset * 2),
                        highlightTint, SliderSpeed, select, highlight),
      FDIR_DOWN);
  soundPage->Add(
      new OptionsSlider(MovieVolume, 0.0f, 1.0f, SliderTrackSprite,
                        LabelSprites[10],
                        SoundEntriesStartPosition +
                            glm::vec2(0.0f, SoundEntriesVerticalOffset * 3),
                        highlightTint, SliderSpeed, select, highlight),
      FDIR_DOWN);
  soundPage->Add(
      new OptionsBinaryButton(
          SyncVoice, BinaryBoxSprite, YesSprite, NoSprite, LabelSprites[11],
          SoundEntriesStartPosition +
              glm::vec2(0.0f, SoundEntriesVerticalOffset * 4),
          highlightTint, select, highlight),
      FDIR_DOWN);
  soundPage->Add(
      new OptionsBinaryButton(
          SkipVoice, BinaryBoxSprite, YesSprite, NoSprite, LabelSprites[12],
          SoundEntriesStartPosition +
              glm::vec2(0.0f, SoundEntriesVerticalOffset * 5),
          highlightTint, select, highlight),
      FDIR_DOWN);
  soundPage->Add(
      new OptionsBinaryButton(
          UseSpeaker, BinaryBoxSprite, YesSprite, NoSprite, LabelSprites[13],
          SoundEntriesStartPosition +
              glm::vec2(0.0f, SoundEntriesVerticalOffset * 6),
          highlightTint, select, highlight),
      FDIR_DOWN);
  soundPage->Add(
      new OptionsSlider(SpeakerVolume, 0.0f, 1.0f, SliderTrackSprite,
                        LabelSprites[14],
                        SoundEntriesStartPosition +
                            glm::vec2(0.0f, SoundEntriesVerticalOffset * 7),
                        highlightTint, SliderSpeed, select, highlight),
      FDIR_DOWN);

  return soundPage;
}

std::unique_ptr<Group> OptionsMenu::CreateVoicePage(
    const std::function<void(OptionsEntry*)>& select,
    const std::function<void(Widget*)>& highlight) {
  const glm::vec4 highlightTint(HighlightColor, 1.0f);
  std::unique_ptr<Group> voicePage = std::make_unique<Group>(this);

  constexpr int columns = 3;
  constexpr int entries = 12;
  for (int i = 0; i < entries; i++) {
    const glm::vec2 pos =
        VoicePosition +
        VoiceEntriesOffset * glm::vec2(i % columns, i / columns);

    Widget* widget = new OptionsVoiceSlider(
        VoiceVolumes[i], 0.0f, 1.0f, VoiceMuted[i], VoiceSliderTrackSprite,
        NametagSprites[i], PortraitSprites[2 * i], PortraitSprites[2 * i + 1],
        pos, highlightTint, SliderSpeed, select, highlight);
    voicePage->Add(widget, FDIR_RIGHT);
  }

  // Loop separately to overwrite the direction set at initial adding
  // First entry won't set anything; skip
  for (int i = 1; i < entries; i++) {
    Widget* const widget = voicePage->Children[i];

    if (i % columns != 0) {  // Not on first column
      Widget* const leftWidget = voicePage->Children[i - 1];
      widget->SetFocus(leftWidget, FDIR_LEFT);
      leftWidget->SetFocus(widget, FDIR_RIGHT);

      if (i % columns == columns - 1) {  // On last column
        Widget* const rowStart = voicePage->Children[i - columns + 1];
        widget->SetFocus(rowStart, FDIR_RIGHT);
        rowStart->SetFocus(widget, FDIR_LEFT);
      }
    }
    if (i >= columns) {  // Not on first row
      Widget* const upWidget = voicePage->Children[i - columns];
      widget->SetFocus(upWidget, FDIR_UP);
      upWidget->SetFocus(widget, FDIR_DOWN);

      if (i >= entries - columns) {  // On last layer
        Widget* const columnStart = voicePage->Children[i % columns];
        widget->SetFocus(columnStart, FDIR_DOWN);
        columnStart->SetFocus(widget, FDIR_UP);
      }
    }
  }

  return voicePage;
}

OptionsMenu::OptionsMenu() : UI::OptionsMenu() {
  PoleAnimation = Profile::CCLCC::OptionsMenu::PoleAnimation.Instantiate();

  PageButtons.reserve(PageCount);
  for (int i = 0; i < PageCount; i++) {
    PageButtons.push_back(ClickArea(i, PagePanelHoverBounds[i]));
  }

  std::function<void(OptionsEntry*)> select =
      std::bind(&OptionsMenu::Select, this, std::placeholders::_1);
  std::function<void(Widget*)> highlight =
      std::bind(&OptionsMenu::Highlight, this, std::placeholders::_1);

  Pages.reserve(PageCount);
  Pages.push_back(std::move(CreateBasicPage(select, highlight)));
  Pages.push_back(std::move(CreateTextPage(select, highlight)));
  Pages.push_back(std::move(CreateSoundPage(select, highlight)));
  Pages.push_back(std::move(CreateVoicePage(select, highlight)));

  Highlight(Pages[CurrentPage]->GetFirstFocusableChild());
}

void OptionsMenu::Show() {
  UI::OptionsMenu::Show();

  if (State != Shown) PoleAnimation.StartIn();
}

void OptionsMenu::Hide() {
  if (State != Hidden) {
    PoleAnimation.StartOut();

    if (CurrentlyFocusedElement)
      static_cast<OptionsEntry*>(CurrentlyFocusedElement)->Selected = false;
  }

  UI::OptionsMenu::Hide();
}

void OptionsMenu::UpdateVisibility() {
  if (ScrWork[SW_SYSSUBMENUCT] < 32 && State == Shown) {
    Hide();
  } else if (ScrWork[SW_SYSSUBMENUCT] > 0 && State == Hidden &&
             ScrWork[SW_SYSSUBMENUNO] == 5) {
    Show();
  }

  if (FadeAnimation.IsIn() && ScrWork[SW_SYSSUBMENUCT] == 32) {
    State = Shown;
  } else if (State == Hiding && FadeAnimation.IsOut() &&
             ScrWork[SW_SYSSUBMENUCT] == 0) {
    State = Hidden;
    Pages[CurrentPage]->Hide();
  }
}

void OptionsMenu::Update(float dt) {
  UI::OptionsMenu::Update(dt);
  PoleAnimation.Update(dt);

  if (!FadeAnimation.IsIn() && !FadeAnimation.IsOut()) {
    const glm::vec2 backgroundPosition =
        glm::vec2(0.0f, glm::mix(BackgroundFadeStartPosition.y,
                                 BackgroundPosition.y, FadeAnimation.Progress));
    for (std::unique_ptr<Group>& page : Pages) {
      page->MoveTo(backgroundPosition);
    }
  }
}

void OptionsMenu::PageButtonOnHover(int pageNumber) {
  if (pageNumber != CurrentPage || !CurrentlyFocusedElement)
    Audio::Channels[Audio::AC_SSE]->Play("sysse", 1, false, 0.0f);

  if (pageNumber == CurrentPage && CurrentlyFocusedElement) return;

  GoToPage(pageNumber);
  Highlight(Pages[CurrentPage]->GetFirstFocusableChild());
}

void OptionsMenu::UpdatePageInput(float dt) {
  // Mouse input
  for (ClickArea& button : PageButtons) {
    const bool wasHovered = button.Hovered;
    button.UpdateInput();
    if (!wasHovered && button.Hovered) PageButtonOnHover(button.Id);
  }

  const int lastPage = CurrentPage;
  UI::OptionsMenu::UpdatePageInput(dt);

  if (CurrentPage != lastPage)
    Audio::Channels[Audio::AC_SSE]->Play("sysse", 1, false, 0.0f);
}

void OptionsMenu::UpdateEntryMovementInput(float dt) {
  const Widget* const lastHighlight = CurrentlyFocusedElement;
  UI::OptionsMenu::UpdateEntryMovementInput(dt);

  if (CurrentlyFocusedElement != lastHighlight)
    Audio::Channels[Audio::AC_SSE]->Play("sysse", 1, false, 0.0f);
}

void OptionsMenu::UpdateInput(float dt) {
  UpdatePageInput(dt);
  bool backBtnPressed = (PADinputMouseWentDown & PAD1B) ||
                        (!AnyEntrySelected() && GetControlState(CT_Back));
  if (State == Shown && backBtnPressed) {
    if (!GetFlag(SF_SUBMENUEXIT))
      Audio::Channels[Audio::AC_SSE]->Play("sysse", 3, false, 0.0f);
    SetFlag(SF_SUBMENUEXIT, 1);
  }

  // If something is selected, the option entry takes full control
  if (AnyEntrySelected()) return;

  UpdateEntryMovementInput(dt);
}

void OptionsMenu::Render() {
  if (State != Hidden && ScrWork[SW_SYSSUBMENUCT] > 0 &&
      ScrWork[SW_SYSSUBMENUNO] == 5) {
    const glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    const glm::vec4 maskTint =
        col * glm::vec4{glm::vec3{1.0f}, (float)0xa0 / 0x100};

    const glm::vec2 backgroundAnimationOffset =
        glm::vec2(0.0f, FadeAnimation.Progress * BackgroundPosition.y +
                            (1.0f - FadeAnimation.Progress) *
                                BackgroundFadeStartPosition.y);
    const glm::vec2 pagePanelPosition =
        PagePanelPosition * FadeAnimation.Progress +
        (1.0f - FadeAnimation.Progress) * PagePanelFadeStartPosition;
    const glm::vec2 guidePosition =
        GuidePosition * FadeAnimation.Progress +
        (1.0f - FadeAnimation.Progress) * GuideFadeStartPosition;

    Renderer->DrawSprite(BackgroundSprite,
                         BackgroundPosition + backgroundAnimationOffset, col);
    Renderer->DrawSprite(HeaderSprite,
                         HeaderPosition + backgroundAnimationOffset, col);

    Renderer->DrawSprite(PageHeaderSprites[CurrentPage],
                         PageHeaderPosition + backgroundAnimationOffset, col);
    Pages[CurrentPage]->Tint = col;
    Pages[CurrentPage]->Render();

    Renderer->DrawSprite(PoleAnimation.CurrentSprite(), pagePanelPosition, col);
    if (PoleAnimation.IsIn()) {
      Renderer->DrawSprite(
          PagePanelSprites[2 * CurrentPage],
          PagePanelPosition + PagePanelIconOffsets[CurrentPage], col);
    }

    Renderer->DrawSprite(
        MenuMaskSprite,
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
        maskTint);

    const Sprite& guideSprite =
        CurrentPage == 3 ? VoiceGuideSprite : GuideSprite;
    Renderer->DrawSprite(guideSprite, guidePosition, col);
  }
}

void OptionsMenu::Select(OptionsEntry* toSelect) {
  for (Widget* entry : Pages[CurrentPage]->Children) {
    static_cast<OptionsEntry*>(entry)->Selected = false;
    entry->HasFocus = false;
  }

  toSelect->Selected = true;
  toSelect->HasFocus = true;
  CurrentlyFocusedElement = toSelect;
}

void OptionsMenu::Highlight(Widget* toHighlight) {
  UI::OptionsMenu::Highlight(toHighlight);

  for (Widget* entry : Pages[CurrentPage]->Children) {
    static_cast<OptionsEntry*>(entry)->Selected = false;
  }
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto