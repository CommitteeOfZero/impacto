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

OptionsMenu::OptionsMenu() : UI::OptionsMenu() {
  PoleAnimation = Profile::CCLCC::OptionsMenu::PoleAnimation.Instantiate();

  Pages.reserve(PageCount);
  glm::vec2 pos = EntriesStartPosition;
  glm::vec4 highlightTint(HighlightColor, 1.0f);
  std::function<void(OptionsEntry*)> select =
      std::bind(&OptionsMenu::Select, this, std::placeholders::_1);
  std::function<void(Widget*)> highlight =
      std::bind(&OptionsMenu::Highlight, this, std::placeholders::_1);

  PageButtons.reserve(PageCount);
  for (int i = 0; i < PageCount; i++) {
    PageButtons.push_back(ClickArea(i, PagePanelHoverBounds[i]));
  }

  std::unique_ptr<Group> basicPage = std::make_unique<Group>(this);
  for (int i = 0; i < 4; i++) {
    basicPage->Add(new OptionsBinaryButton(BinaryBoxSprite, OnSprite, OffSprite,
                                           LabelSprites[i], pos, highlightTint,
                                           select, highlight),
                   FDIR_DOWN);

    pos.y += EntriesVerticalOffset;
  }
  Pages.push_back(std::move(basicPage));

  pos = EntriesStartPosition;
  std::unique_ptr<Group> textPage = std::make_unique<Group>(this);
  for (int i = 4; i < 6; i++) {
    textPage->Add(
        new OptionsSlider(SliderTrackSprite, LabelSprites[i], pos,
                          highlightTint, SliderSpeed, select, highlight),
        FDIR_DOWN);

    pos.y += EntriesVerticalOffset;
  }
  textPage->Add(new OptionsBinaryButton(BinaryBoxSprite, SkipReadSprite,
                                        SkipAllSprite, LabelSprites[6], pos,
                                        highlightTint, select, highlight),
                FDIR_DOWN);
  Pages.push_back(std::move(textPage));

  pos = SoundEntriesStartPosition;
  std::unique_ptr<Group> soundPage = std::make_unique<Group>(this);
  for (int i = 7; i < 15; i++) {
    Widget* widget =
        (i < 11 || i == 14)
            ? new OptionsSlider(SliderTrackSprite, LabelSprites[i], pos,
                                highlightTint, SliderSpeed, select, highlight)
            : widget = new OptionsBinaryButton(
                  BinaryBoxSprite, YesSprite, NoSprite, LabelSprites[i], pos,
                  highlightTint, select, highlight);
    soundPage->Add(widget, FDIR_DOWN);

    pos.y += SoundEntriesVerticalOffset;
  }
  Pages.push_back(std::move(soundPage));

  std::unique_ptr<Group> voicePage = std::make_unique<Group>(this);
  constexpr int columns = 3;
  constexpr int entries = 12;
  for (int i = 0; i < entries; i++) {
    glm::vec2 pos = VoicePosition;
    pos += VoiceEntriesOffset * glm::vec2(i % columns, i / columns);

    Widget* widget = new OptionsVoiceSlider(
        VoiceSliderTrackSprite, NametagSprites[i], PortraitSprites[2 * i],
        PortraitSprites[2 * i + 1], pos, highlightTint, SliderSpeed, select,
        highlight);
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

  Pages.push_back(std::move(voicePage));

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

  if (PADinputMouseWentDown & PAD1B ||
      !AnyEntrySelected() && GetControlState(CT_Back)) {
    if (!GetFlag(SF_SUBMENUEXIT))
      Audio::Channels[Audio::AC_SSE]->Play("sysse", 3, false, 0.0f);

    Hide();
    return;
  }

  // If something is selected, the option entry takes full control
  if (AnyEntrySelected()) return;

  UpdateEntryMovementInput(dt);
}

void OptionsMenu::Render() {
  if (State != Hidden && ScrWork[SW_SYSSUBMENUCT] >= 32 &&
      ScrWork[SW_SYSSUBMENUNO] == 5) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);

    glm::vec4 maskTint = col;
    maskTint.a *= (float)0xa0 / 0x100;

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