#include "optionsmenu.h"

#include "../../ui/widgets/chlcc/optionsbutton.h"
#include "../../ui/widgets/chlcc/optionsslider.h"

#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../vm/vm.h"
#include "../../profile/scriptvars.h"
#include "../../profile/dialogue.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../profile/ui/optionsmenu.h"
#include "../../profile/games/chlcc/optionsmenu.h"
#include "../../io/memorystream.h"
#include "../../data/tipssystem.h"
#include "../../vm/interface/input.h"
#include "../../profile/game.h"

#include "../../profile/configsystem.h"
#include "../../audio/audiosystem.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::OptionsMenu;
using namespace Impacto::Profile::CHLCC::OptionsMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::ConfigSystem;

using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;
using namespace Impacto::UI::Widgets::CHLCC;

OptionsMenu::OptionsMenu() : UI::OptionsMenu() {
  TitleFade.Direction = AnimationDirection::In;
  TitleFade.LoopMode = AnimationLoopMode::Stop;
  TitleFade.DurationIn = TitleFadeInDuration;
  TitleFade.DurationOut = TitleFadeOutDuration;

  FromSystemMenuTransition.Direction = AnimationDirection::In;
  FromSystemMenuTransition.LoopMode = AnimationLoopMode::Stop;
  FromSystemMenuTransition.DurationIn = TitleFadeInDuration;
  FromSystemMenuTransition.DurationOut = TitleFadeOutDuration;

  RedBarSprite = InitialRedBarSprite;
  RedBarPosition = InitialRedBarPosition;

  ShowPageAnimation.SetDuration(ShowAnimationDuration);

  PageTransitionAnimation.SetDuration(PageTransitionDuration);

  SelectedAnimation.DurationIn = SelectedSlideDuration;
  SelectedAnimation.LoopMode = AnimationLoopMode::Loop;
  SelectedAnimation.StartIn();

  std::function<void(OptionsEntry*)> highlight = [this](auto* entry) {
    return Highlight(entry);
  };

  Pages.reserve(3);
  Pages.emplace_back(CreateTextPage(highlight));
  Pages.emplace_back(CreateSoundPage(highlight));
  Pages.emplace_back(CreateVoicePage(highlight));

  Highlight(Pages[CurrentPage]->GetFirstFocusableChild());
  SelectedLabelPos =
      Pages[CurrentPage]->GetFirstFocusableChild()->Bounds.GetPos();
}

std::unique_ptr<Widgets::Group> OptionsMenu::CreateTextPage(
    const std::function<void(OptionsEntry*)>& highlight) {
  std::unique_ptr<Group> textPage = std::make_unique<Group>(this);
  RectF highlightBounds(0.0f, 0.0f, SelectedLabelSprite.ScaledWidth(),
                        SelectedLabelSprite.ScaledHeight());

  const auto addButton =
      [&]<typename T>(
          size_t id, T& value, std::span<const T> values,
          std::span<const std::reference_wrapper<const Sprite>> sprites) {
        highlightBounds.X = TextPageEntryPositions[id].x;
        highlightBounds.Y = TextPageEntryPositions[id].y;
        textPage->Add(
            new OptionsButton<T>(value, values, sprites,
                                 highlightBounds.GetPos() +
                                     glm::vec2(highlightBounds.Width, 0.0f),
                                 highlightBounds, highlight),
            FDIR_DOWN);
      };

  // Basic settings
  {
    constexpr static std::array<std::reference_wrapper<const Sprite>, 2>
        sprites{SettingDoSprite, SettingDontSprite};
    addButton(0, TriggerStopSkip, std::span<const bool>(TriggerStopSkipValues),
              sprites);
  }
  {
    constexpr static std::array<std::reference_wrapper<const Sprite>, 2>
        sprites{SettingDoSprite, SettingDontSprite};
    addButton(1, ShowTipsNotification,
              std::span<const bool>(ShowTipsNotificationValues), sprites);
  }
  {
    constexpr static std::array<std::reference_wrapper<const Sprite>, 4>
        sprites{SettingOnTriggerAndSceneSprite, SettingDontSprite,
                SettingOnTriggerSprite, SettingOnSceneSprite};
    addButton(2, AutoQuickSave, std::span<const uint8_t>(AutoQuickSaveValues),
              sprites);
  }
  {
    constexpr static std::array<std::reference_wrapper<const Sprite>, 2>
        sprites{SettingTypeASprite, SettingTypeBSprite};
    addButton(3, ControllerType, std::span<const uint8_t>(ControllerTypeValues),
              sprites);
  }

  highlightBounds.X = TextPageEntryPositions[4].x;
  highlightBounds.Y = TextPageEntryPositions[4].y;
  textPage->Add(
      new OptionsSlider(
          ImageSize, 0.0f, 1.0f, SliderBarBaseSprite, SliderBarFillSprite,
          highlightBounds.GetPos() + glm::vec2(highlightBounds.Width, 0.0f),
          highlightBounds, highlight),
      FDIR_DOWN);

  // Text settings
  {
    constexpr static std::array<std::reference_wrapper<const Sprite>, 4>
        sprites{SettingNormalSprite, SettingFastSprite, SettingInstantSprite,
                SettingSlowSprite};
    addButton(5, TextSpeed, std::span<const float>(TextSpeedValues), sprites);
  }
  {
    constexpr static std::array<std::reference_wrapper<const Sprite>, 3>
        sprites{SettingNormalSprite, SettingFastSprite, SettingSlowSprite};
    addButton(6, AutoSpeed, std::span<const float>(AutoSpeedValues), sprites);
  }
  {
    constexpr static std::array<std::reference_wrapper<const Sprite>, 2>
        sprites{SettingReadSprite, SettingAllSprite};
    addButton(7, SkipRead, std::span<const bool>(SkipReadValues), sprites);
  }

  return textPage;
}

std::unique_ptr<Widgets::Group> OptionsMenu::CreateSoundPage(
    const std::function<void(OptionsEntry*)>& highlight) {
  std::unique_ptr<Group> soundPage = std::make_unique<Group>(this);
  RectF highlightBounds(0.0f, 0.0f, SelectedLabelSprite.ScaledWidth(),
                        SelectedLabelSprite.ScaledHeight());

  const auto addButton =
      [&]<typename T>(
          size_t id, T& value, std::span<const T> values,
          std::span<const std::reference_wrapper<const Sprite>> sprites) {
        highlightBounds.X = SoundPageEntryPositions[id].x;
        highlightBounds.Y = SoundPageEntryPositions[id].y;
        const glm::vec2 topRight =
            highlightBounds.GetPos() + glm::vec2(highlightBounds.Width, 0.0f);
        soundPage->Add(new OptionsButton<T>(value, values, sprites, topRight,
                                            highlightBounds, highlight),
                       FDIR_DOWN);
      };
  const auto addSlider = [&](size_t id, float& value, float min, float max) {
    highlightBounds.X = SoundPageEntryPositions[id].x;
    highlightBounds.Y = SoundPageEntryPositions[id].y;
    const glm::vec2 topRight =
        highlightBounds.GetPos() + glm::vec2(highlightBounds.Width, 0.0f);
    soundPage->Add(new OptionsSlider(value, min, max, SliderBarBaseSprite,
                                     SliderBarFillSprite, topRight,
                                     highlightBounds, highlight),
                   FDIR_DOWN);
  };

  addSlider(0, Audio::GroupVolumes[Audio::ACG_Voice], 0.0f, 1.0f);
  addSlider(1, Audio::GroupVolumes[Audio::ACG_BGM], 0.0f, 0.5f);
  addSlider(2, Audio::GroupVolumes[Audio::ACG_SE], 0.0f, 1.0f);
  addSlider(3, Audio::GroupVolumes[Audio::ACG_Movie], 0.0f, 1.0f);
  {
    constexpr static std::array<std::reference_wrapper<const Sprite>, 2>
        sprites{SettingDoSprite, SettingDontSprite};
    addButton(4, SyncVoice, std::span<const bool>(SyncVoiceValues), sprites);
  }
  {
    constexpr static std::array<std::reference_wrapper<const Sprite>, 2>
        sprites{SettingDontSprite, SettingDoSprite};
    addButton(5, SkipVoice, std::span<const bool>(SkipVoiceValues), sprites);
  }

  return soundPage;
}

std::unique_ptr<Widgets::Group> OptionsMenu::CreateVoicePage(
    const std::function<void(OptionsEntry*)>& highlight) {
  std::unique_ptr<Group> voicePage = std::make_unique<Group>(this);
  RectF highlightBounds(0.0f, 0.0f, SelectedLabelSprite.ScaledWidth(),
                        SelectedLabelSprite.ScaledHeight());

  for (size_t i = 0; i < VoicePageEntryPositions.size(); i++) {
    highlightBounds.X = VoicePageEntryPositions[i].x;
    highlightBounds.Y = VoicePageEntryPositions[i].y;
    const glm::vec2 topRight =
        highlightBounds.GetPos() + glm::vec2(highlightBounds.Width, 0.0f);

    voicePage->Add(
        new OptionsSlider(Profile::ConfigSystem::VoiceVolume[i], 0.0f, 1.0f,
                          SliderBarBaseSprite, SliderBarFillSprite, topRight,
                          highlightBounds, highlight, VoiceMutedSprite),
        FDIR_DOWN);
  }

  return voicePage;
}

void OptionsMenu::Show() {
  if (State == Hidden) {
    ShowPageAnimation.StartIn();
  }

  UI::OptionsMenu::Show();
}

void OptionsMenu::Hide() {
  if (State == Shown) {
    SetFlag(SF_SUBMENUEXIT, true);
    ShowPageAnimation.StartOut();
  }

  UI::OptionsMenu::Hide();

  if (State == Hiding) {
    Pages[CurrentPage]->IsShown = true;
  }
}

void OptionsMenu::RenderPage(const size_t pageId, const glm::vec2 offset) {
  switch (pageId) {
    case static_cast<int>(PageType::Text):
      Renderer->DrawSprite(BasicSettingsSprite, BasicSettingsPos + offset);
      Renderer->DrawSprite(TextSettingsSprite, TextSettingsPos + offset);
      break;
    case static_cast<int>(PageType::Sound):
      Renderer->DrawSprite(SoundSettingsSprite, SoundSettingsPos + offset);
      break;
    case static_cast<int>(PageType::Voice):
      Renderer->DrawSprite(VoiceSettingsSprite, VoiceSettingsPos + offset);
      break;
    default:
      ImpLogSlow(LogLevel::Warning, LogChannel::General,
                 "Unexpected options menu page {:d}", pageId);
      break;
  }

  Pages[pageId]->Move(offset);
  Pages[pageId]->Render();
  Pages[pageId]->Move(-offset);
}

void OptionsMenu::Render() {
  if (State == Hidden) return;

  if (FadeAnimation.IsIn()) {
    Renderer->DrawQuad(
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
        RgbIntToFloat(BackgroundColor));
  } else if (GetFlag(SF_SYSTEMMENU)) {
    Renderer->DrawQuad(
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
        RgbIntToFloat(BackgroundColor, FromSystemMenuTransition.Progress));
  } else {
    DrawCircles();
  }
  DrawErin();
  DrawRedBar();
  if (FadeAnimation.Progress > 0.34f) {
    Renderer->DrawSprite(RedBarLabel, RedTitleLabelPos);

    const CornersQuad titleDest = MenuTitleText.ScaledBounds()
                                      .RotateAroundCenter(MenuTitleTextAngle)
                                      .Translate(RightTitlePos);
    Renderer->DrawSprite(MenuTitleText, titleDest);
  }

  // Alpha goes from 0 to 1 in half the time
  float alpha =
      FadeAnimation.Progress < 0.5f ? FadeAnimation.Progress * 2.0f : 1.0f;
  Renderer->DrawSprite(
      BackgroundFilter,
      RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
      {1.0f, 1.0f, 1.0f, alpha});

  if (FadeAnimation.Progress > 0.22f) {
    DrawButtonPrompt();
  }

  if ((CurrentlyFocusedElement != nullptr || !ShowPageAnimation.IsIn()) &&
      PageTransitionAnimation.State == +AnimationState::Stopped) {
    for (float x = SelectedSprite.ScaledWidth() * -SelectedAnimation.Progress;
         x < Profile::DesignWidth; x += SelectedSprite.ScaledWidth()) {
      Renderer->DrawSprite(SelectedSprite,
                           glm::vec2(x, SelectedLabelPos.y) + ShowPageOffset);
    }

    Renderer->DrawSprite(SelectedLabelSprite,
                         SelectedLabelPos + ShowPageOffset);
    const glm::vec2 dotOffset =
        CurrentPage == static_cast<size_t>(PageType::Voice)
            ? SelectedDotVoicesOffset
            : SelectedDotOffset;
    Renderer->DrawSprite(SelectedDotSprite,
                         SelectedLabelPos + dotOffset + ShowPageOffset);
  }

  if (PageTransitionAnimation.State == +AnimationState::Stopped) {
    RenderPage(CurrentPage, ShowPageOffset);
  } else {
    Pages[PreviousPage]->IsShown = true;
    RenderPage(PreviousPage, PageTransitionGoingOffset + ShowPageOffset);
    RenderPage(CurrentPage, PageTransitionComingOffset + ShowPageOffset);
  }
}

void OptionsMenu::UpdateVisibility() {
  if ((!GetFlag(SF_OPTIONMENU) || ScrWork[SW_SYSMENUCT] < 10000) &&
      State == Shown) {
    Hide();
  } else if (GetFlag(SF_OPTIONMENU) && ScrWork[SW_SYSMENUCT] > 0 &&
             State == Hidden) {
    Show();
  }

  if (FadeAnimation.IsOut() && !GetFlag(SF_OPTIONMENU) &&
      (ScrWork[SW_SYSMENUCT] == 0 || GetFlag(SF_SYSTEMMENU)) &&
      State == Hiding) {
    State = Hidden;
  } else if (FadeAnimation.IsIn() && ScrWork[SW_SYSMENUCT] == 10000 &&
             GetFlag(SF_OPTIONMENU) && State == Showing) {
    State = Shown;
  }
}

void OptionsMenu::UpdatePageShowAnimation(float dt) {
  ShowPageAnimation.Update(dt);
  ShowPageOffset = {0.0f, 0.0f};

  if (ShowPageAnimation.IsIn()) return;

  const float startProgress =
      ShowPageAnimationStartTime / ShowAnimationDuration;
  const float endProgress =
      startProgress + ShowPageAnimationDuration / ShowAnimationDuration;

  if (startProgress < ShowPageAnimation.Progress &&
      ShowPageAnimation.Progress < endProgress) {
    const float progress = (ShowPageAnimation.Progress - startProgress) /
                           (endProgress - startProgress);
    const float angle = progress * std::numbers::pi_v<float> * 0.5f;

    ShowPageOffset = {0.0f, (std::sin(angle) - 1.0f) * Profile::DesignHeight};

  } else if (ShowPageAnimation.Progress <= startProgress) {
    ShowPageOffset = {0.0f, -Profile::DesignHeight};
  }
}

void OptionsMenu::UpdateSelectedLabel(float dt) {
  SelectedAnimation.Update(dt);

  if (CurrentlyFocusedElement == nullptr ||
      SelectedLabelPos.y == CurrentlyFocusedElement->Bounds.Y)
    return;

  const float currentY = SelectedLabelPos.y;
  const float targetY = CurrentlyFocusedElement->Bounds.Y;

  // Not exactly how the binary does it, but this is smoother
  const float entriesLeft =
      std::abs(currentY - targetY) / SelectedLabelModalDistancePerEntry;
  const float speedMultiplier =
      1.0f + 3.0f * glm::smoothstep(0.0f, 4.0f, entriesLeft);
  const float pixelsMoved = SelectedLabelBaseSpeed * speedMultiplier * dt;

  if (SelectedLabelPos.y < CurrentlyFocusedElement->Bounds.Y) {
    SelectedLabelPos.y = std::min(SelectedLabelPos.y + pixelsMoved,
                                  CurrentlyFocusedElement->Bounds.Y);
  } else {
    SelectedLabelPos.y = std::max(SelectedLabelPos.y - pixelsMoved,
                                  CurrentlyFocusedElement->Bounds.Y);
  }
}

void OptionsMenu::UpdatePageTransitionAnimation(float dt) {
  PageTransitionAnimation.Update(dt);

  if (PageTransitionAnimation.State == +AnimationState::Stopped) return;

  constexpr glm::vec2 anchor = {1.0f, 0.0f};

  float angle = (1.0f - PageTransitionAnimation.Progress) * PageRotationAngle;
  PageTransitionComingOffset =
      (glm::vec2(std::cos(angle), std::sin(angle)) - anchor) * 720.0f;

  angle = -PageTransitionAnimation.Progress * PageRotationAngle;
  PageTransitionGoingOffset =
      (glm::vec2(std::cos(angle), std::sin(angle)) - anchor) * 720.0f;

  if (PageTransitionAnimation.Direction == AnimationDirection::Out) {
    std::swap(PageTransitionGoingOffset, PageTransitionComingOffset);
  }
}

void OptionsMenu::Update(float dt) {
  UI::OptionsMenu::Update(dt);

  if (State != Hidden) {
    FromSystemMenuTransition.Update(dt);
    if (FadeAnimation.Direction == AnimationDirection::Out &&
        FadeAnimation.Progress <= 0.72f) {
      TitleFade.StartOut();
      FromSystemMenuTransition.StartOut();
    } else if (FadeAnimation.IsIn() &&
               (TitleFade.Direction == AnimationDirection::In ||
                TitleFade.IsOut())) {
      TitleFade.StartIn();
      FromSystemMenuTransition.StartIn();
    }
    TitleFade.Update(dt);

    UpdateTitles();
    UpdatePageShowAnimation(dt);
    UpdatePageTransitionAnimation(dt);
    UpdateSelectedLabel(dt);
  }
}

void OptionsMenu::UpdateInput(float dt) {
  if (State != Shown ||
      PageTransitionAnimation.State == +AnimationState::Playing)
    return;

  UI::OptionsMenu::UpdateInput(dt);
}

void OptionsMenu::GoToPage(size_t pageNumber) {
  if (pageNumber == CurrentPage ||
      PageTransitionAnimation.State != +AnimationState::Stopped)
    return;

  PreviousPage = CurrentPage;
  UI::OptionsMenu::GoToPage(pageNumber);

  AnimationDirection direction = PreviousPage > CurrentPage
                                     ? AnimationDirection::In
                                     : AnimationDirection::Out;
  if (PreviousPage == Pages.size() - 1 && CurrentPage == 0)
    direction = AnimationDirection::Out;
  else if (PreviousPage == 0 && CurrentPage == Pages.size() - 1)
    direction = AnimationDirection::In;

  PageTransitionAnimation.Start(direction, true);
}

inline void OptionsMenu::DrawCircles() {
  float y = CircleStartPosition.y;
  int resetCounter = 0;
  // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
  // duration is totalframes/60
  float progress = FadeAnimation.Progress * FadeInDuration * 60.0f;
  for (int line = 0; line < 4; line++) {
    int counter = resetCounter;
    float x = CircleStartPosition.x;
    for (int col = 0; col < 7; col++) {
      if (counter + 1 <= (progress)) {
        float scale = ((progress) - (counter + 1.0f)) * 16.0f;
        scale = scale <= 320.0f ? scale : 320.0f;
        scale *= CircleSprite.Bounds.Height / 106.0f;
        Renderer->DrawSprite(
            CircleSprite, RectF(x + (CircleSprite.Bounds.Width - scale) / 2.0f,
                                y + (CircleSprite.Bounds.Height - scale) / 2.0f,
                                scale, scale));
        x += CircleOffset;
      }
      counter += 2;
    }
    y += CircleOffset;
    resetCounter += 2;
  }
}

inline void OptionsMenu::DrawErin() {
  float y = ErinPosition.y;
  if (FadeAnimation.Progress < 0.78f) {
    y = 801.0f;
    if (FadeAnimation.Progress > 0.22f) {
      // Approximation from the original function, which was a bigger mess
      y = glm::mix(
          -19.0f, 721.0f,
          0.998938f -
              0.998267f * sin(3.97835f - 3.27549f * FadeAnimation.Progress));
    }
  }
  Renderer->DrawSprite(ErinSprite, glm::vec2(ErinPosition.x, y));
}

inline void OptionsMenu::DrawRedBar() {
  if (FadeAnimation.IsIn()) {
    Renderer->DrawSprite(InitialRedBarSprite, InitialRedBarPosition);
  } else if (FadeAnimation.Progress > 0.70f) {
    // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
    // duration is totalframes/60
    float progress = FadeAnimation.Progress * FadeInDuration * 60.0f;
    float pixelPerAdvanceLeft = RedBarBaseX * (progress - 47.0f) / 17.0f;
    RedBarSprite.Bounds.X = RedBarDivision - pixelPerAdvanceLeft;
    RedBarSprite.Bounds.Width = pixelPerAdvanceLeft;
    RedBarPosition.x = RedBarBaseX - pixelPerAdvanceLeft;
    Renderer->DrawSprite(RedBarSprite, RedBarPosition);
    float pixelPerAdvanceRight = 13.0f * (progress - 47.0f);
    RedBarSprite.Bounds.X = RedBarDivision;
    RedBarSprite.Bounds.Width = pixelPerAdvanceRight;
    RedBarPosition = RightRedBarPosition;
    Renderer->DrawSprite(RedBarSprite, RedBarPosition);
  }
}

inline void OptionsMenu::DrawButtonPrompt() {
  if (FadeAnimation.IsIn()) {
    Renderer->DrawSprite(ButtonPromptSprite, ButtonPromptPosition);
  } else if (FadeAnimation.Progress > 0.734f) {
    float x = ButtonPromptPosition.x - 2560.0f * (FadeAnimation.Progress - 1);
    Renderer->DrawSprite(ButtonPromptSprite,
                         glm::vec2(x, ButtonPromptPosition.y));
  }
}

void OptionsMenu::UpdateTitles() {
  if (FadeAnimation.Progress <= 0.34f) return;

  RedTitleLabelPos = RedBarLabelPosition;
  RightTitlePos = MenuTitleTextRightPosition;

  if (FadeAnimation.Progress >= 0.73f) return;

  RedTitleLabelPos +=
      glm::vec2(-572.0f * (FadeAnimation.Progress * 4.0f - 3.0f),
                460.0f * (FadeAnimation.Progress * 4.0f - 3.0f) / 3.0f);
  RightTitlePos +=
      glm::vec2(-572.0f * (FadeAnimation.Progress * 4.0f - 3.0f),
                460.0f * (FadeAnimation.Progress * 4.0f - 3.0f) / 3.0f);
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto
