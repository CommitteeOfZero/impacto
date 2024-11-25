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
static float Test = 0.5f;

void OptionsMenu::NextPageOnClick(Widgets::Button* target) { GoToNextPage(); }

void OptionsMenu::PreviousPageOnClick(Widgets::Button* target) {
  GoToPreviousPage();
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

OptionsMenu::OptionsMenu() {
  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
  PageFadeAnimation.Direction = AnimationDirection::In;
  PageFadeAnimation.LoopMode = AnimationLoopMode::Stop;
  PageFadeAnimation.DurationIn = FadeInDuration;
  PageFadeAnimation.DurationOut = FadeOutDuration;

  auto nextPageOnClick =
      std::bind(&OptionsMenu::NextPageOnClick, this, std::placeholders::_1);
  auto previousPageOnClick =
      std::bind(&OptionsMenu::PreviousPageOnClick, this, std::placeholders::_1);

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
  TextPage = new Group(this);
  TextPage->FocusLock = false;
  TextPage->Add(new Label(TextPageLabel, PageLabelPosition));

  // Text speed
  auto textSpeedOnClick = std::bind(&OptionsMenu::MessageSpeedToggleOnClick,
                                    this, std::placeholders::_1);
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
  TextPage->Add(textSpeedOptions, FDIR_DOWN);
  currentPos += ListPadding;

  // Auto mode wait time
  auto autoModeWaitTimeOnClick = std::bind(
      &OptionsMenu::AutoModeWaitTimeOnClick, this, std::placeholders::_1);
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
  TextPage->Add(autoModeOptions, FDIR_DOWN);
  currentPos += ListPadding;

  // Skip mode
  auto skipModeOnClick =
      std::bind(&OptionsMenu::SkipModeOnClick, this, std::placeholders::_1);
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
  TextPage->Add(skipModeOptions, FDIR_DOWN);
  Pages.push_back(TextPage);
  currentPos = ListStartingPosition;

  // --- Sound page 1 ---
  SoundPage1 = new Group(this);
  SoundPage1->FocusLock = false;
  SoundPage1->Add(new Label(SoundPageLabel, PageLabelPosition));

  // Voice sync options
  auto voiceSyncOptions =
      new OptionGroup(this, VoiceSyncOptionsLabel, VoiceSyncOptionsLabelH,
                      ButtonHighlight, currentPos, OptionGroupItemsOffset);
  for (int i = 0; i < SoundModeOptionsNum; i++) {
    voiceSyncOptions->AddOption(new Toggle(
        i, &FalseValue, SoundModeOptionsHSprites[i], SoundModeOptionsSprites[i],
        nullSprite, glm::vec2(0.0f), false));
  }
  SoundPage1->Add(voiceSyncOptions, FDIR_DOWN);
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
  SoundPage1->Add(voiceSkipOptions, FDIR_DOWN);
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
  SoundPage1->Add(voiceHighlightOptions, FDIR_DOWN);
  Pages.push_back(SoundPage1);
  currentPos = ListStartingPosition;

  // --- Sound page 2 ---
  SoundPage2 = new Group(this);
  SoundPage2->FocusLock = false;
  SoundPage2->Add(new Label(SoundPageLabel, PageLabelPosition));

  // BGM volume options
  auto bgmVolumeOptions =
      new OptionGroup(this, BgmVolumeLabel, BgmVolumeLabelH, nullSprite,
                      currentPos, OptionGroupSliderOffset);
  auto bgmVolumeSlider = new Scrollbar(
      0, glm::vec2(0.0f), 0.0f, 1.0f, &Audio::GroupVolumes[Audio::ACG_BGM],
      SBDIR_HORIZONTAL, SliderTrackSprite, nullSprite, SliderFillSprite);
  bgmVolumeOptions->AddOption(bgmVolumeSlider);
  bgmVolumeSlider->FillBeforeTrack = true;
  SoundPage2->Add(bgmVolumeOptions, FDIR_DOWN);
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
  SoundPage2->Add(voiceVolumeOptions, FDIR_DOWN);
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
  SoundPage2->Add(seVolumeOptions, FDIR_DOWN);
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
  SoundPage2->Add(systemSeVolumeOptions, FDIR_DOWN);
  currentPos += ListPadding;
  auto characterVoiceButton =
      new Button(0, CharacterVoiceVolumeLabel, CharacterVoiceVolumeLabelH,
                 nullSprite, currentPos);
  SoundPage2->Add(characterVoiceButton, FDIR_DOWN);
  Pages.push_back(SoundPage2);
  currentPos = ListStartingPosition;

  // --- Other page ---
  OtherPage = new Group(this);
  OtherPage->FocusLock = false;
  OtherPage->Add(new Label(OtherPageLabel, PageLabelPosition));

  // Quick save options
  auto quickSaveOptions =
      new OptionGroup(this, QuickSaveOptionsLabel, QuickSaveOptionsLabelH,
                      ButtonHighlight, currentPos, OptionGroupItemsOffset);
  for (int i = 0; i < QuickSaveOptionsNum; i++) {
    quickSaveOptions->AddOption(new Toggle(
        i, &FalseValue, QuickSaveOptionsHSprites[i], QuickSaveOptionsSprites[i],
        nullSprite, glm::vec2(0.0f), false));
  }
  OtherPage->Add(quickSaveOptions, FDIR_DOWN);
  Pages.push_back(OtherPage);

  CurrentPage = Pages.begin();
  PreviousPage = Pages.end();
}

void OptionsMenu::Show() {
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    (*CurrentPage)->Show();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
    CurrentlyFocusedElement = (*CurrentPage)->GetFirstFocusableChild();
    CurrentlyFocusedElement->HasFocus = true;
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
  UpdateInput();

  FadeAnimation.Update(dt);
  PageFadeAnimation.Update(dt);
  if (ScrWork[SW_SYSSUBMENUCT] < 16 && ScrWork[SW_SYSSUBMENUNO] == 5 &&
      State == Shown) {
    Hide();
  } else if (ScrWork[SW_SYSSUBMENUCT] > 0 && ScrWork[SW_SYSSUBMENUNO] == 5 &&
             State == Hidden) {
    Show();
  }

  if (ScrWork[SW_SYSSUBMENUCT] == 16 && ScrWork[SW_SYSSUBMENUNO] == 5 &&
      FadeAnimation.IsIn())
    State = Shown;
  else if (ScrWork[SW_SYSSUBMENUCT] == 0 && ScrWork[SW_SYSSUBMENUNO] == 5 &&
           FadeAnimation.IsOut())
    State = Hidden;

  if (State == Shown && IsFocused) {
    PageControls->Update(dt);

    if (PADinputButtonWentDown & PAD1L2) {
      GoToPreviousPage();
    } else if (PADinputButtonWentDown & PAD1R2) {
      GoToNextPage();
    }

    if (PageFadeAnimation.State == +AnimationState::Playing) {
      (*PreviousPage)->Update(dt);
      (*CurrentPage)->Update(dt);
    } else {
      if (PreviousPage != Pages.end() && (*PreviousPage)->IsShown) {
        (*PreviousPage)->Hide();
        (*CurrentPage)->Show();
      }

      (*CurrentPage)->Update(dt);
    }

    if (GetControlState(CT_Back)) {
      SetFlag(SF_SUBMENUEXIT, true);
    }
  }
}

void OptionsMenu::Render() {
  if (State != Hidden) {
    glm::vec4 col(1.0f, 1.0f, 1.0f,
                  glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress));
    Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f, 0.0f), col);

    if (PageFadeAnimation.State == +AnimationState::Playing) {
      (*CurrentPage)->Tint = col;
      (*PreviousPage)->Tint = col;
      (*CurrentPage)->Tint.a *=
          glm::smoothstep(0.0f, 1.0f, PageFadeAnimation.Progress);
      (*PreviousPage)->Tint.a *=
          glm::smoothstep(1.0f, 0.0f, PageFadeAnimation.Progress);

      (*PreviousPage)->Render();
      (*CurrentPage)->Render();
    } else {
      (*CurrentPage)->Tint = col;
      (*CurrentPage)->Render();
    }

    PageControls->Tint = col;
    PageControls->Render();
  }
}

void OptionsMenu::GoToNextPage() {
  PreviousPage = CurrentPage;
  (*CurrentPage)->HasFocus = false;
  CurrentPage++;
  if (CurrentPage == Pages.end()) {
    CurrentPage = Pages.begin();
  }
  (*CurrentPage)->HasFocus = true;
  (*CurrentPage)->Show();
  CurrentlyFocusedElement = (*CurrentPage)->GetFirstFocusableChild();
  CurrentlyFocusedElement->HasFocus = true;
  PageFadeAnimation.StartIn(true);
}

void OptionsMenu::GoToPreviousPage() {
  PreviousPage = CurrentPage;
  (*CurrentPage)->HasFocus = false;
  if (CurrentPage == Pages.begin()) {
    CurrentPage = Pages.end();
  }
  CurrentPage--;
  (*CurrentPage)->HasFocus = true;
  (*CurrentPage)->Show();
  CurrentlyFocusedElement = (*CurrentPage)->GetFirstFocusableChild();
  CurrentlyFocusedElement->HasFocus = true;
  PageFadeAnimation.StartIn(true);
}

}  // namespace MO8
}  // namespace UI
}  // namespace Impacto