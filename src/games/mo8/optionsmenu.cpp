#include "optionsmenu.h"

#include "../../profile/ui/optionsmenu.h"
#include "../../profile/games/mo8/optionsmenu.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../vm/interface/input.h"

namespace Impacto {
namespace UI {
namespace MO8 {

using namespace Impacto::Profile::OptionsMenu;
using namespace Impacto::Profile::MO8::OptionsMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;

static bool FalseValue = false;

void OptionsMenu::NextPageOnClick(Widgets::Button* target) {
  GoToPage((CurrentPage + 1) % Pages.size());
}

void OptionsMenu::PreviousPageOnClick(Widgets::Button* target) {
  GoToPage((CurrentPage - 1) % Pages.size());
}

void OptionsMenu::MessageSpeedToggleOnClick(Widgets::Toggle* target) {
  if (*target->Value) {
    memset(MessageSpeedValues, false, sizeof(MessageSpeedValues));
    MessageSpeedValues[target->Id] = true;
  } else {
    *target->Value = true;
  }
}

void OptionsMenu::AutoModeWaitTimeOnClick(Widgets::Toggle* target) {
  if (*target->Value) {
    memset(AutoModeWaitTimeValues, false, sizeof(AutoModeWaitTimeValues));
    AutoModeWaitTimeValues[target->Id] = true;
  } else {
    *target->Value = true;
  }
}

void OptionsMenu::SkipModeOnClick(Widgets::Toggle* target) {
  if (*target->Value) {
    memset(SkipModeValues, false, sizeof(SkipModeValues));
    SkipModeValues[target->Id] = true;
  } else {
    *target->Value = true;
  }
}

OptionsMenu::OptionsMenu() : UI::OptionsMenu() {
  PageFadeAnimation.Direction = AnimationDirection::In;
  PageFadeAnimation.LoopMode = AnimationLoopMode::Stop;
  PageFadeAnimation.DurationIn = FadeInDuration;
  PageFadeAnimation.DurationOut = FadeOutDuration;

  auto nextPageOnClick = [this](auto* btn) { return NextPageOnClick(btn); };
  auto previousPageOnClick = [this](auto* btn) {
    return PreviousPageOnClick(btn);
  };

  PageControls = new Group(this);
  PageControls->FocusLock = false;
  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);
  auto nextPage = new Button(0, NextButtonSprite, NextButtonHighlightedSprite,
                             nullSprite, NextButtonPosition);
  nextPage->OnClickHandler = nextPageOnClick;
  auto previousPage =
      new Button(0, BackButtonSprite, BackButtonHighlightedSprite, nullSprite,
                 BackButtonPosition);
  previousPage->OnClickHandler = previousPageOnClick;
  PageControls->Add(nextPage);
  PageControls->Add(previousPage);
  PageControls->IsShown = true;

  auto currentPos = ListStartingPosition;

  // --- Text page ---
  auto textPage = std::make_unique<Group>(this);
  textPage->FocusLock = false;
  textPage->Add(new Label(TextPageLabel, PageLabelPosition));

  // Text speed
  auto textSpeedOnClick = [this](auto* btn) {
    return MessageSpeedToggleOnClick(btn);
  };
  auto textSpeedOptions =
      new OptionGroup(this, TextSpeedOptionsLabel, TextSpeedOptionsLabelH,
                      ButtonHighlight, currentPos, OptionGroupItemsOffset);
  for (int i = 0; i < TextSpeedOptionsNum; i++) {
    auto toggle = new Toggle(
        i, &MessageSpeedValues[i], TextSpeedOptionsHSprites[i],
        TextSpeedOptionsSprites[i], nullSprite, glm::vec2(0.0f), false);
    toggle->OnClickHandler = textSpeedOnClick;
    textSpeedOptions->AddOption(toggle);
  }
  textPage->Add(textSpeedOptions, FDIR_DOWN);
  currentPos += ListPadding;

  // Auto mode wait time
  auto autoModeWaitTimeOnClick = [this](auto* btn) {
    return AutoModeWaitTimeOnClick(btn);
  };
  auto autoModeOptions =
      new OptionGroup(this, AutoModeOptionsLabel, AutoModeOptionsLabelH,
                      ButtonHighlight, currentPos, OptionGroupItemsOffset);
  for (int i = 0; i < AutoModeOptionsNum; i++) {
    auto toggle = new Toggle(
        i, &AutoModeWaitTimeValues[i], AutoModeOptionsHSprites[i],
        AutoModeOptionsSprites[i], nullSprite, glm::vec2(0.0f), false);
    toggle->OnClickHandler = autoModeWaitTimeOnClick;
    autoModeOptions->AddOption(toggle);
  }
  textPage->Add(autoModeOptions, FDIR_DOWN);
  currentPos += ListPadding;

  // Skip mode
  auto skipModeOnClick = [this](auto* btn) { return SkipModeOnClick(btn); };
  auto skipModeOptions =
      new OptionGroup(this, SkipModeOptionsLabel, SkipModeOptionsLabelH,
                      ButtonHighlight, currentPos, OptionGroupItemsOffset);
  for (int i = 0; i < SkipModeOptionsNum; i++) {
    auto toggle = new Toggle(i, &SkipModeValues[i], SkipModeOptionsHSprites[i],
                             SkipModeOptionsSprites[i], nullSprite,
                             glm::vec2(0.0f), false);
    toggle->OnClickHandler = skipModeOnClick;
    skipModeOptions->AddOption(toggle);
  }
  textPage->Add(skipModeOptions, FDIR_DOWN);
  Pages.push_back(std::move(textPage));
  currentPos = ListStartingPosition;

  // --- Sound page 1 ---
  auto soundPage1 = std::make_unique<Group>(this);
  soundPage1->FocusLock = false;
  soundPage1->Add(new Label(SoundPageLabel, PageLabelPosition));

  // Voice sync options
  auto voiceSyncOptions =
      new OptionGroup(this, VoiceSyncOptionsLabel, VoiceSyncOptionsLabelH,
                      ButtonHighlight, currentPos, OptionGroupItemsOffset);
  for (int i = 0; i < SoundModeOptionsNum; i++) {
    voiceSyncOptions->AddOption(new Toggle(
        i, &FalseValue, SoundModeOptionsHSprites[i], SoundModeOptionsSprites[i],
        nullSprite, glm::vec2(0.0f), false));
  }
  soundPage1->Add(voiceSyncOptions, FDIR_DOWN);
  currentPos += ListPadding;

  // Voice skip options
  auto voiceSkipOptions =
      new OptionGroup(this, VoiceSkipOptionsLabel, VoiceSkipOptionsLabelH,
                      ButtonHighlight, currentPos, OptionGroupItemsOffset);
  for (int i = 0; i < SoundModeOptionsNum; i++) {
    voiceSkipOptions->AddOption(new Toggle(
        i, &FalseValue, SoundModeOptionsHSprites[i], SoundModeOptionsSprites[i],
        nullSprite, glm::vec2(0.0f), false));
  }
  soundPage1->Add(voiceSkipOptions, FDIR_DOWN);
  currentPos += ListPadding;

  // Voice highlight options
  auto voiceHighlightOptions = new OptionGroup(
      this, VoiceHighlightOptionsLabel, VoiceHighlightOptionsLabelH,
      ButtonHighlight, currentPos, OptionGroupItemsOffset);
  for (int i = 0; i < SoundModeOptionsNum; i++) {
    voiceHighlightOptions->AddOption(new Toggle(
        i, &FalseValue, SoundModeOptionsHSprites[i], SoundModeOptionsSprites[i],
        nullSprite, glm::vec2(0.0f), false));
  }
  soundPage1->Add(voiceHighlightOptions, FDIR_DOWN);
  Pages.push_back(std::move(soundPage1));
  currentPos = ListStartingPosition;

  // --- Sound page 2 ---
  auto soundPage2 = std::make_unique<Group>(this);
  soundPage2->FocusLock = false;
  soundPage2->Add(new Label(SoundPageLabel, PageLabelPosition));

  // BGM volume options
  auto bgmVolumeOptions =
      new OptionGroup(this, BgmVolumeLabel, BgmVolumeLabelH, nullSprite,
                      currentPos, OptionGroupSliderOffset);
  auto bgmVolumeSlider = new Scrollbar(
      0, glm::vec2(0.0f), 0.0f, 1.0f, &Audio::GroupVolumes[Audio::ACG_BGM],
      SBDIR_HORIZONTAL, SliderTrackSprite, nullSprite, SliderFillSprite);
  bgmVolumeOptions->AddOption(bgmVolumeSlider);
  bgmVolumeSlider->FillBeforeTrack = true;
  soundPage2->Add(bgmVolumeOptions, FDIR_DOWN);
  currentPos += ListPadding;

  // Voice volume options
  auto voiceVolumeOptions =
      new OptionGroup(this, VoiceVolumeLabel, VoiceVolumeLabelH, nullSprite,
                      currentPos, OptionGroupSliderOffset);
  auto voiceVolumeSlider = new Scrollbar(
      0, glm::vec2(0.0f), 0.0f, 1.0f, &Audio::GroupVolumes[Audio::ACG_Voice],
      SBDIR_HORIZONTAL, SliderTrackSprite, nullSprite, SliderFillSprite);
  voiceVolumeOptions->AddOption(voiceVolumeSlider);
  voiceVolumeSlider->FillBeforeTrack = true;
  soundPage2->Add(voiceVolumeOptions, FDIR_DOWN);
  currentPos += ListPadding;

  // SE volume options
  auto seVolumeOptions =
      new OptionGroup(this, SeVolumeLabel, SeVolumeLabelH, nullSprite,
                      currentPos, OptionGroupSliderOffset);
  auto seVolumeSlider = new Scrollbar(
      0, glm::vec2(0.0f), 0.0f, 1.0f, &Audio::GroupVolumes[Audio::ACG_SE],
      SBDIR_HORIZONTAL, SliderTrackSprite, nullSprite, SliderFillSprite);
  seVolumeOptions->AddOption(seVolumeSlider);
  seVolumeSlider->FillBeforeTrack = true;
  soundPage2->Add(seVolumeOptions, FDIR_DOWN);
  currentPos += ListPadding;

  // SYSSE volume options
  auto systemSeVolumeOptions =
      new OptionGroup(this, SystemSeVolumeLabel, SystemSeVolumeLabelH,
                      nullSprite, currentPos, OptionGroupSliderOffset);
  auto systemSeVolumeSlider = new Scrollbar(
      0, glm::vec2(0.0f), 0.0f, 1.0f, &Audio::GroupVolumes[Audio::ACG_SE],
      SBDIR_HORIZONTAL, SliderTrackSprite, nullSprite, SliderFillSprite);
  systemSeVolumeOptions->AddOption(systemSeVolumeSlider);
  systemSeVolumeSlider->FillBeforeTrack = true;
  soundPage2->Add(systemSeVolumeOptions, FDIR_DOWN);
  currentPos += ListPadding;
  auto characterVoiceButton =
      new Button(0, CharacterVoiceVolumeLabel, CharacterVoiceVolumeLabelH,
                 nullSprite, currentPos);
  soundPage2->Add(characterVoiceButton, FDIR_DOWN);
  Pages.push_back(std::move(soundPage2));
  currentPos = ListStartingPosition;

  // --- Other page ---
  auto otherPage = std::make_unique<Group>(this);
  otherPage->FocusLock = false;
  otherPage->Add(new Label(OtherPageLabel, PageLabelPosition));

  // Quick save options
  auto quickSaveOptions =
      new OptionGroup(this, QuickSaveOptionsLabel, QuickSaveOptionsLabelH,
                      ButtonHighlight, currentPos, OptionGroupItemsOffset);
  for (int i = 0; i < QuickSaveOptionsNum; i++) {
    quickSaveOptions->AddOption(new Toggle(
        i, &FalseValue, QuickSaveOptionsHSprites[i], QuickSaveOptionsSprites[i],
        nullSprite, glm::vec2(0.0f), false));
  }
  otherPage->Add(quickSaveOptions, FDIR_DOWN);
  Pages.push_back(std::move(otherPage));
}

void OptionsMenu::UpdatePageInput(float dt) {
  // Mouse controls
  PageControls->Update(dt);

  UI::OptionsMenu::UpdatePageInput(dt);
}

void OptionsMenu::Show() {
  if (State != Showing) {
    PreviousPage = -1;
  }

  UI::OptionsMenu::Show();
}

void OptionsMenu::UpdateVisibility() {
  if (ScrWork[SW_SYSSUBMENUCT] < 16 && ScrWork[SW_SYSSUBMENUNO] == 5 &&
      State == Shown) {
    Hide();
  } else if (ScrWork[SW_SYSSUBMENUCT] > 0 && ScrWork[SW_SYSSUBMENUNO] == 5 &&
             State == Hidden) {
    Show();
  }

  if (FadeAnimation.IsIn()) {
    State = Shown;
  } else if (ScrWork[SW_SYSSUBMENUCT] == 0 && ScrWork[SW_SYSSUBMENUNO] == 5 &&
             FadeAnimation.IsOut()) {
    State = Hidden;
  }
}

void OptionsMenu::UpdateInput(float dt) {
  UI::OptionsMenu::UpdateInput(dt);

  if (GetControlState(CT_Back)) SetFlag(SF_SUBMENUEXIT, true);
}

void OptionsMenu::Update(float dt) {
  PageFadeAnimation.Update(dt);

  UI::OptionsMenu::Update(dt);

  if (State != Hidden) {
    if (PageFadeAnimation.State == AnimationState::Playing) {
      Pages[PreviousPage]->Update(dt);
    } else if (PreviousPage != -1 && Pages[PreviousPage]->IsShown) {
      Pages[PreviousPage]->Hide();
      Pages[CurrentPage]->Show();
    }
  }
}

void OptionsMenu::Render() {
  if (State != Hidden) {
    glm::vec4 col(1.0f, 1.0f, 1.0f,
                  glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress));
    Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f, 0.0f), col);

    std::unique_ptr<Group>& currentPage = Pages[CurrentPage];
    if (PageFadeAnimation.State == AnimationState::Playing) {
      std::unique_ptr<Group>& previousPage = Pages[PreviousPage];

      currentPage->Tint = col;
      previousPage->Tint = col;
      currentPage->Tint.a *=
          glm::smoothstep(0.0f, 1.0f, PageFadeAnimation.Progress);
      previousPage->Tint.a *=
          glm::smoothstep(1.0f, 0.0f, PageFadeAnimation.Progress);

      previousPage->Render();
      currentPage->Render();
    } else {
      currentPage->Tint = col;
      currentPage->Render();
    }

    PageControls->Tint = col;
    PageControls->Render();
  }
}

void OptionsMenu::GoToPage(int pageNumber) {
  if (CurrentPage == pageNumber) return;

  PreviousPage = CurrentPage;
  UI::OptionsMenu::GoToPage(pageNumber);

  PageFadeAnimation.StartIn(true);
}

}  // namespace MO8
}  // namespace UI
}  // namespace Impacto