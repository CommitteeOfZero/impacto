#pragma once

#include "../../ui/optionsmenu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/scrollbar.h"
#include "../../ui/widgets/toggle.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

class OptionsMenu : public UI::OptionsMenu {
 public:
  OptionsMenu();

  void Update(float dt) override;
  void Render() override;

  void MessageSpeedToggleOnClick(Widgets::Toggle* target);
  void AutoModeWaitTimeOnClick(Widgets::Toggle* target);
  void SyncTextSpeedToVoiceOnClick(Widgets::Toggle* target);
  void SkipVoiceAtNextLineOnClick(Widgets::Toggle* target);
  void SkipModeOnClick(Widgets::Toggle* target);
  void AutoSaveTriggerOnClick(Widgets::Toggle* target);
  void TipsNotificationsOnClick(Widgets::Toggle* target);

 private:
  void UpdateVisibility() override;

  Widgets::Scrollbar* VoiceVolumeSlider;
  Widgets::Scrollbar* BGMVolumeSlider;
  Widgets::Scrollbar* SEVolumeSlider;
  Widgets::Scrollbar* MovieVolumeSlider;
  Widgets::Group* CharacterVoiceToggles;
  bool VoiceTest[13] = {0};

  Widgets::Scrollbar* ScreenSizeSlider;
  Widgets::Group* MessageSpeedToggles;
  bool MessageSpeedValues[4] = {false};
  Widgets::Group* AutoModeWaitTimeToggles;
  bool AutoModeWaitTimeValues[3] = {false};
  Widgets::Group* SyncTextSpeedToVoiceToggles;
  bool SyncTextSpeedToVoiceValues[2] = {false};
  Widgets::Group* SkipVoiceAtNextLineToggles;
  bool SkipVoiceAtNextLineValues[2] = {false};
  Widgets::Group* SkipModeToggles;
  bool SkipModeValues[2] = {false};
  Widgets::Group* AutoSaveTriggerToggles;
  bool AutoSaveTriggerValues[4] = {false};
  Widgets::Group* TipsNotificationsToggles;
  bool TipsNotificationsValues[2] = {false};
};

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto