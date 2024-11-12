#include "optionsmenu.h"

#include "../../profile/ui/optionsmenu.h"
#include "../../profile/games/mo6tw/optionsmenu.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../vm/interface/input.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

using namespace Impacto::Profile::OptionsMenu;
using namespace Impacto::Profile::MO6TW::OptionsMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;

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

void OptionsMenu::SyncTextSpeedToVoiceOnClick(Widgets::Toggle* target) {
  if (*target->Value) {
    memset(SyncTextSpeedToVoiceValues, false,
           sizeof(SyncTextSpeedToVoiceValues));
    SyncTextSpeedToVoiceValues[target->Id] = true;
  } else {
    *target->Value = true;
  }
}

void OptionsMenu::SkipVoiceAtNextLineOnClick(Widgets::Toggle* target) {
  if (*target->Value) {
    memset(SkipVoiceAtNextLineValues, false, sizeof(SkipVoiceAtNextLineValues));
    SkipVoiceAtNextLineValues[target->Id] = true;
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

void OptionsMenu::AutoSaveTriggerOnClick(Widgets::Toggle* target) {
  if (*target->Value) {
    memset(AutoSaveTriggerValues, false, sizeof(AutoSaveTriggerValues));
    AutoSaveTriggerValues[target->Id] = true;
  } else {
    *target->Value = true;
  }
}

void OptionsMenu::TipsNotificationsOnClick(Widgets::Toggle* target) {
  if (*target->Value) {
    memset(TipsNotificationsValues, false, sizeof(TipsNotificationsValues));
    TipsNotificationsValues[target->Id] = true;
  } else {
    *target->Value = true;
  }
}

OptionsMenu::OptionsMenu() {
  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  // First page
  FirstPage = new Widgets::Group(this);
  CharacterVoiceToggles = new Widgets::Group(this);
  CharacterVoiceToggles->FocusLock = false;
  CharacterVoiceToggles->WrapFocus = false;

  VoiceVolumeSlider = new Widgets::Scrollbar(
      0, FirstPageSliderPos, 0.0f, 1.0f, &Audio::GroupVolumes[Audio::ACG_Voice],
      Widgets::SBDIR_HORIZONTAL, SliderTrackSprite, SliderThumbSprite,
      SliderFillSprite, SliderThumbOffset);
  FirstPage->Add(VoiceVolumeSlider, FDIR_DOWN);
  FirstPageSliderPos.y += FirstPageSliderMargin;

  BGMVolumeSlider = new Widgets::Scrollbar(
      0, FirstPageSliderPos, 0.0f, 1.0f, &Audio::GroupVolumes[Audio::ACG_BGM],
      Widgets::SBDIR_HORIZONTAL, SliderTrackSprite, SliderThumbSprite,
      SliderFillSprite, SliderThumbOffset);
  FirstPage->Add(BGMVolumeSlider, FDIR_DOWN);
  FirstPageSliderPos.y += FirstPageSliderMargin;

  SEVolumeSlider = new Widgets::Scrollbar(
      0, FirstPageSliderPos, 0.0f, 1.0f, &Audio::GroupVolumes[Audio::ACG_SE],
      Widgets::SBDIR_HORIZONTAL, SliderTrackSprite, SliderThumbSprite,
      SliderFillSprite, SliderThumbOffset);
  FirstPage->Add(SEVolumeSlider, FDIR_DOWN);
  FirstPageSliderPos.y += FirstPageSliderMargin;

  MovieVolumeSlider = new Widgets::Scrollbar(
      0, FirstPageSliderPos, 0.0f, 1.0f, &Audio::GroupVolumes[Audio::ACG_Movie],
      Widgets::SBDIR_HORIZONTAL, SliderTrackSprite, SliderThumbSprite,
      SliderFillSprite, SliderThumbOffset);
  FirstPage->Add(MovieVolumeSlider, FDIR_DOWN);

  auto pos = VoiceToggleStart;
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
  size_t idx = 0;
  for (const auto& el : CharacterVoiceToggles->Children) {
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
  FirstPage->Add(CharacterVoiceToggles, FDIR_DOWN);

  // Second page
  SecondPage = new Widgets::Group(this);
  int checkboxLabelIdx = 0;

  MessageSpeedToggles = new Widgets::Group(this);
  MessageSpeedToggles->FocusLock = false;
  AutoModeWaitTimeToggles = new Widgets::Group(this);
  AutoModeWaitTimeToggles->FocusLock = false;
  SyncTextSpeedToVoiceToggles = new Widgets::Group(this);
  SyncTextSpeedToVoiceToggles->FocusLock = false;
  SkipVoiceAtNextLineToggles = new Widgets::Group(this);
  SkipVoiceAtNextLineToggles->FocusLock = false;
  SkipModeToggles = new Widgets::Group(this);
  SkipModeToggles->FocusLock = false;
  AutoSaveTriggerToggles = new Widgets::Group(this);
  AutoSaveTriggerToggles->FocusLock = false;
  TipsNotificationsToggles = new Widgets::Group(this);
  TipsNotificationsToggles->FocusLock = false;

  auto messageSpeedOnClick = std::bind(&OptionsMenu::MessageSpeedToggleOnClick,
                                       this, std::placeholders::_1);
  for (int i = 0; i < 4; i++) {
    auto toggle = new Widgets::Toggle(
        i, &MessageSpeedValues[i], CheckboxTickSprite, CheckboxBoxSprite,
        CheckboxTickSprite,
        glm::vec2(CheckboxFirstPos.x + (i * CheckboxFirstSectionPaddingX),
                  CheckboxFirstPos.y),
        true, CheckboxLabelSprites[checkboxLabelIdx++], CheckboxLabelOffset);
    toggle->Enabled = true;
    toggle->OnClickHandler = messageSpeedOnClick;
    toggle->SetFocus(AutoModeWaitTimeToggles, FDIR_DOWN);
    toggle->SetFocus(TipsNotificationsToggles, FDIR_UP);
    MessageSpeedToggles->Add(toggle, FDIR_RIGHT);
  }
  SecondPage->Add(MessageSpeedToggles, FDIR_DOWN);

  CheckboxFirstPos += CheckboxMargin;
  auto autoModeWaitTimeOnClick = std::bind(
      &OptionsMenu::AutoModeWaitTimeOnClick, this, std::placeholders::_1);
  for (int i = 0; i < 3; i++) {
    auto toggle = new Widgets::Toggle(
        i, &AutoModeWaitTimeValues[i], CheckboxTickSprite, CheckboxBoxSprite,
        CheckboxTickSprite,
        glm::vec2(CheckboxFirstPos.x + (i * CheckboxFirstSectionPaddingX),
                  CheckboxFirstPos.y),
        true, CheckboxLabelSprites[checkboxLabelIdx++], CheckboxLabelOffset);
    toggle->Enabled = true;
    toggle->OnClickHandler = autoModeWaitTimeOnClick;
    toggle->SetFocus(SyncTextSpeedToVoiceToggles, FDIR_DOWN);
    toggle->SetFocus(MessageSpeedToggles, FDIR_UP);
    AutoModeWaitTimeToggles->Add(toggle, FDIR_RIGHT);
  }
  SecondPage->Add(AutoModeWaitTimeToggles, FDIR_DOWN);

  CheckboxFirstPos += CheckboxMargin;
  auto syncTextSpeedToVoiceOnClick = std::bind(
      &OptionsMenu::SyncTextSpeedToVoiceOnClick, this, std::placeholders::_1);
  for (int i = 0; i < 2; i++) {
    auto toggle = new Widgets::Toggle(
        i, &SyncTextSpeedToVoiceValues[i], CheckboxTickSprite,
        CheckboxBoxSprite, CheckboxTickSprite,
        glm::vec2(CheckboxFirstPos.x + (i * CheckboxFirstSectionPaddingX),
                  CheckboxFirstPos.y),
        true, CheckboxLabelSprites[checkboxLabelIdx++], CheckboxLabelOffset);
    toggle->Enabled = true;
    toggle->OnClickHandler = syncTextSpeedToVoiceOnClick;
    toggle->SetFocus(SkipVoiceAtNextLineToggles, FDIR_DOWN);
    toggle->SetFocus(AutoModeWaitTimeToggles, FDIR_UP);
    SyncTextSpeedToVoiceToggles->Add(toggle, FDIR_RIGHT);
  }
  checkboxLabelIdx -= 2;
  SecondPage->Add(SyncTextSpeedToVoiceToggles, FDIR_DOWN);

  CheckboxFirstPos += CheckboxMargin;
  auto skipVoiceAtNextLineOnClick = std::bind(
      &OptionsMenu::SkipVoiceAtNextLineOnClick, this, std::placeholders::_1);
  for (int i = 0; i < 2; i++) {
    auto toggle = new Widgets::Toggle(
        i, &SkipVoiceAtNextLineValues[i], CheckboxTickSprite, CheckboxBoxSprite,
        CheckboxTickSprite,
        glm::vec2(CheckboxFirstPos.x + (i * CheckboxFirstSectionPaddingX),
                  CheckboxFirstPos.y),
        true, CheckboxLabelSprites[checkboxLabelIdx++], CheckboxLabelOffset);
    toggle->Enabled = true;
    toggle->OnClickHandler = skipVoiceAtNextLineOnClick;
    toggle->SetFocus(SkipModeToggles, FDIR_DOWN);
    toggle->SetFocus(SyncTextSpeedToVoiceToggles, FDIR_UP);
    SkipVoiceAtNextLineToggles->Add(toggle, FDIR_RIGHT);
  }
  SecondPage->Add(SkipVoiceAtNextLineToggles, FDIR_DOWN);

  auto skipModeOnClick =
      std::bind(&OptionsMenu::SkipModeOnClick, this, std::placeholders::_1);
  for (int i = 0; i < 2; i++) {
    auto toggle = new Widgets::Toggle(
        i, &SkipModeValues[i], CheckboxTickSprite, CheckboxBoxSprite,
        CheckboxTickSprite,
        glm::vec2(
            CheckboxSecondPos.x + (i * CheckboxSecondSectionFirstPaddingX),
            CheckboxSecondPos.y),
        true, CheckboxLabelSprites[checkboxLabelIdx++], CheckboxLabelOffset);
    toggle->Enabled = true;
    toggle->OnClickHandler = skipModeOnClick;
    toggle->SetFocus(AutoSaveTriggerToggles, FDIR_DOWN);
    toggle->SetFocus(SkipVoiceAtNextLineToggles, FDIR_UP);
    SkipModeToggles->Add(toggle, FDIR_RIGHT);
  }
  SecondPage->Add(SkipModeToggles, FDIR_DOWN);

  float dummy = 0.0f;
  ScreenSizeSlider = new Widgets::Scrollbar(
      0, ScreenSizeSliderPos, 0.0f, 1.0f, &dummy, Widgets::SBDIR_HORIZONTAL,
      SliderTrackSprite, SliderThumbSprite, SliderFillSprite,
      SliderThumbOffset);

  CheckboxSecondPos += CheckboxMargin;
  auto autoSaveTriggerOnClick = std::bind(&OptionsMenu::AutoSaveTriggerOnClick,
                                          this, std::placeholders::_1);
  for (int i = 0; i < 4; i++) {
    auto toggle = new Widgets::Toggle(
        i, &AutoSaveTriggerValues[i], CheckboxTickSprite, CheckboxBoxSprite,
        CheckboxTickSprite,
        glm::vec2(AutoSaveTriggerXPos[i], CheckboxSecondPos.y), true,
        CheckboxLabelSprites[i == 3 ? 8 : checkboxLabelIdx++],
        CheckboxLabelOffset);
    toggle->Enabled = true;
    toggle->OnClickHandler = autoSaveTriggerOnClick;
    toggle->SetFocus(ScreenSizeSlider, FDIR_DOWN);
    toggle->SetFocus(SkipModeToggles, FDIR_UP);
    AutoSaveTriggerToggles->Add(toggle, FDIR_RIGHT);
  }
  SecondPage->Add(AutoSaveTriggerToggles, FDIR_DOWN);

  ScreenSizeSlider->SetFocus(TipsNotificationsToggles, FDIR_DOWN);
  SecondPage->Add(ScreenSizeSlider, FDIR_DOWN);

  auto tipsNotificationsOnClick = std::bind(
      &OptionsMenu::TipsNotificationsOnClick, this, std::placeholders::_1);
  checkboxLabelIdx = 7;
  for (int i = 0; i < 2; i++) {
    auto toggle = new Widgets::Toggle(
        i, &TipsNotificationsValues[i], CheckboxTickSprite, CheckboxBoxSprite,
        CheckboxTickSprite,
        glm::vec2(TipsPos.x + (i * CheckboxFirstSectionPaddingX), TipsPos.y),
        true, CheckboxLabelSprites[checkboxLabelIdx++], CheckboxLabelOffset);
    toggle->Enabled = true;
    toggle->OnClickHandler = tipsNotificationsOnClick;
    toggle->SetFocus(MessageSpeedToggles, FDIR_DOWN);
    toggle->SetFocus(ScreenSizeSlider, FDIR_UP);
    TipsNotificationsToggles->Add(toggle, FDIR_RIGHT);
  }
  SecondPage->Add(TipsNotificationsToggles, FDIR_DOWN);
}

void OptionsMenu::Show() {
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    FirstPage->Show();
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
  UpdateInput();

  FadeAnimation.Update(dt);
  FirstPage->Update(dt);
  SecondPage->Update(dt);
  if (ScrWork[SW_OPTIONALPHA] < 256 && State == Shown) {
    Hide();
  } else if (ScrWork[SW_OPTIONALPHA] == 256 && State == Hidden) {
    Show();
  }

  if (ScrWork[SW_OPTIONALPHA] == 256 && FadeAnimation.IsIn())
    State = Shown;
  else if (ScrWork[SW_OPTIONALPHA] == 0 && FadeAnimation.IsOut())
    State = Hidden;

  if (State == Shown) {
    if (PADinputButtonWentDown & PAD1X) {
      if (FirstPage->IsShown) {
        FirstPage->Hide();
        SecondPage->Show();
      } else if (SecondPage->IsShown) {
        SecondPage->Hide();
        FirstPage->Show();
      }
    }
  }

  if (GetControlState(CT_Back)) {
    SetFlag(SF_SUBMENUEXIT, true);
  }
}

void OptionsMenu::Render() {
  if (State != Hidden) {
    glm::vec4 col(1.0f, 1.0f, 1.0f,
                  glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress));
    Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f, 0.0f), col);
    if (FirstPage->IsShown) {
      auto pos = FirstPageSectionHeaderPos;
      Renderer->DrawSprite(
          SectionHeaderSprites[0 + VoiceVolumeSlider->HasFocus], pos, col);
      pos.y += FirstPageSliderMargin;
      Renderer->DrawSprite(SectionHeaderSprites[2 + BGMVolumeSlider->HasFocus],
                           pos, col);
      pos.y += FirstPageSliderMargin;
      Renderer->DrawSprite(SectionHeaderSprites[4 + SEVolumeSlider->HasFocus],
                           pos, col);
      pos.y += FirstPageSliderMargin;
      Renderer->DrawSprite(
          SectionHeaderSprites[6 + MovieVolumeSlider->HasFocus], pos, col);
      pos.y += FirstPageSliderMargin;
      Renderer->DrawSprite(
          SectionHeaderSprites[8 + CharacterVoiceToggles->HasFocus], pos, col);

      FirstPage->Tint = col;
      FirstPage->Render();
    } else if (SecondPage->IsShown) {
      auto pos = SecondPageSectionHeaderPos;
      Renderer->DrawSprite(
          SectionHeaderSprites[10 + MessageSpeedToggles->HasFocus], pos, col);
      pos += CheckboxMargin;
      Renderer->DrawSprite(
          SectionHeaderSprites[12 + AutoModeWaitTimeToggles->HasFocus], pos,
          col);
      pos += CheckboxMargin;
      Renderer->DrawSprite(
          SectionHeaderSprites[14 + SyncTextSpeedToVoiceToggles->HasFocus], pos,
          col);
      pos += CheckboxMargin;
      Renderer->DrawSprite(
          SectionHeaderSprites[16 + SkipVoiceAtNextLineToggles->HasFocus], pos,
          col);
      pos += CheckboxMargin;
      pos.x = SecondPageSectionHeaderPos.x;
      Renderer->DrawSprite(SectionHeaderSprites[18 + SkipModeToggles->HasFocus],
                           pos, col);
      pos += CheckboxMargin;
      Renderer->DrawSprite(
          SectionHeaderSprites[20 + AutoSaveTriggerToggles->HasFocus], pos,
          col);
      pos += CheckboxMargin;
      Renderer->DrawSprite(
          SectionHeaderSprites[22 + ScreenSizeSlider->HasFocus], pos, col);
      pos += CheckboxMargin;
      Renderer->DrawSprite(
          SectionHeaderSprites[24 + TipsNotificationsToggles->HasFocus], pos,
          col);

      SecondPage->Tint = col;
      SecondPage->Render();
    }
  }
}

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto