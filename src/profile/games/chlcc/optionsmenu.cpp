#include "optionsmenu.h"
#include "../../profile_internal.h"
#include "../../../log.h"
#include "../../../text/text.h"

#include "../../ui/optionsmenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/chlcc/optionsmenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace OptionsMenu {

void Configure() {
  BackgroundColor = EnsureGetMember<uint32_t>("BackgroundColor");

  CircleSprite = EnsureGetMember<Sprite>("CircleSprite");

  ShowPageAnimationStartTime =
      EnsureGetMember<float>("ShowPageAnimationStartTime");
  ShowPageAnimationDuration =
      EnsureGetMember<float>("ShowPageAnimationDuration");

  MenuTitleTextRightPosition =
      EnsureGetMember<glm::vec2>("MenuTitleTextRightPos");
  MenuTitleTextAngle = EnsureGetMember<float>("MenuTitleTextAngle");
  MenuTitleText = EnsureGetMember<Sprite>("MenuTitleText");

  PageRotationAngle = EnsureGetMember<float>("PageRotationAngle");
  PageTransitionDuration = EnsureGetMember<float>("PageTransitionDuration");

  ButtonPromptSprite = EnsureGetMember<Sprite>("ButtonPromptSprite");
  ButtonPromptPosition = EnsureGetMember<glm::vec2>("ButtonPromptPosition");

  SelectedSprite = EnsureGetMember<Sprite>("SelectedSprite");
  SelectedSlideDuration = EnsureGetMember<float>("SelectedSlideDuration");
  SelectedLabelSprite = EnsureGetMember<Sprite>("SelectedLabelSprite");
  SelectedLabelOffset = EnsureGetMember<glm::vec2>("SelectedLabelOffset");
  SelectedDotSprite = EnsureGetMember<Sprite>("SelectedDotSprite");
  SelectedDotOffset = EnsureGetMember<glm::vec2>("SelectedDotOffset");
  SelectedDotVoicesOffset =
      EnsureGetMember<glm::vec2>("SelectedDotVoicesOffset");
  VoiceMutedSprite = EnsureGetMember<Sprite>("VoiceMutedSprite");
  VoiceMutedOffset = EnsureGetMember<glm::vec2>("VoiceMutedOffset");

  SelectedLabelBaseSpeed = EnsureGetMember<float>("SelectedLabelBaseSpeed");
  SelectedLabelModalDistancePerEntry =
      EnsureGetMember<float>("SelectedLabelModalDistancePerEntry");

  BasicSettingsSprite = EnsureGetMember<Sprite>("BasicSettingsSprite");
  BasicSettingsPos = EnsureGetMember<glm::vec2>("BasicSettingsPos");
  TextSettingsSprite = EnsureGetMember<Sprite>("TextSettingsSprite");
  TextSettingsPos = EnsureGetMember<glm::vec2>("TextSettingsPos");
  SoundSettingsSprite = EnsureGetMember<Sprite>("SoundSettingsSprite");
  SoundSettingsPos = EnsureGetMember<glm::vec2>("SoundSettingsPos");
  VoiceSettingsSprite = EnsureGetMember<Sprite>("VoiceSettingsSprite");
  VoiceSettingsPos = EnsureGetMember<glm::vec2>("VoiceSettingsPos");

  SliderBarBaseSprite = EnsureGetMember<Sprite>("SliderBarBaseSprite");
  SliderBarFillSprite = EnsureGetMember<Sprite>("SliderBarFillSprite");
  SliderBarFadeDuration = EnsureGetMember<float>("SliderBarFadeDuration");
  SliderBarTopRightOffset =
      EnsureGetMember<glm::vec2>("SliderBarTopRightOffset");
  SliderBarFillOffset = EnsureGetMember<glm::vec2>("SliderBarFillOffset");

  SettingInstantSprite = EnsureGetMember<Sprite>("SettingInstantSprite");
  SettingFastSprite = EnsureGetMember<Sprite>("SettingFastSprite");
  SettingNormalSprite = EnsureGetMember<Sprite>("SettingNormalSprite");
  SettingSlowSprite = EnsureGetMember<Sprite>("SettingSlowSprite");
  SettingShortSprite = EnsureGetMember<Sprite>("SettingShortSprite");
  SettingLongSprite = EnsureGetMember<Sprite>("SettingLongSprite");
  SettingDoSprite = EnsureGetMember<Sprite>("SettingDoSprite");
  SettingDontSprite = EnsureGetMember<Sprite>("SettingDontSprite");
  SettingYesSprite = EnsureGetMember<Sprite>("SettingYesSprite");
  SettingNoSprite = EnsureGetMember<Sprite>("SettingNoSprite");
  SettingReadSprite = EnsureGetMember<Sprite>("SettingReadSprite");
  SettingAllSprite = EnsureGetMember<Sprite>("SettingAllSprite");
  SettingOnTriggerSprite = EnsureGetMember<Sprite>("SettingOnTriggerSprite");
  SettingOnSceneSprite = EnsureGetMember<Sprite>("SettingOnSceneSprite");
  SettingOnTriggerAndSceneSprite =
      EnsureGetMember<Sprite>("SettingOnTriggerAndSceneSprite");
  SettingTypeASprite = EnsureGetMember<Sprite>("SettingTypeASprite");
  SettingTypeBSprite = EnsureGetMember<Sprite>("SettingTypeBSprite");
  SettingButtonTopRightOffset =
      EnsureGetMember<glm::vec2>("SettingButtonTopRightOffset");

  GetMemberArray<glm::vec2>(
      std::span(TextPageEntryPositions.data(), TextPageEntryPositions.size()),
      "TextPageEntryPositions");
  GetMemberArray<glm::vec2>(
      std::span(SoundPageEntryPositions.data(), SoundPageEntryPositions.size()),
      "SoundPageEntryPositions");
  GetMemberArray<glm::vec2>(
      std::span(VoicePageEntryPositions.data(), VoicePageEntryPositions.size()),
      "VoicePageEntryPositions");

  GetMemberArray<bool>(
      std::span(TriggerStopSkipValues.data(), TriggerStopSkipValues.size()),
      "TriggerStopSkipValues");
  GetMemberArray<bool>(std::span(ShowTipsNotificationValues.data(),
                                 ShowTipsNotificationValues.size()),
                       "ShowTipsNotificationValues");
  GetMemberArray<uint8_t>(
      std::span(AutoQuickSaveValues.data(), AutoQuickSaveValues.size()),
      "AutoQuickSaveValues");
  GetMemberArray<uint8_t>(
      std::span(ControllerTypeValues.data(), ControllerTypeValues.size()),
      "ControllerTypeValues");
  GetMemberArray<float>(
      std::span(TextSpeedValues.data(), TextSpeedValues.size()),
      "TextSpeedValues");
  GetMemberArray<float>(
      std::span(AutoSpeedValues.data(), AutoSpeedValues.size()),
      "AutoSpeedValues");
  GetMemberArray<bool>(std::span(SkipReadValues.data(), SkipReadValues.size()),
                       "SkipReadValues");
  GetMemberArray<bool>(
      std::span(SyncVoiceValues.data(), SyncVoiceValues.size()),
      "SyncVoiceValues");
  GetMemberArray<bool>(
      std::span(SkipVoiceValues.data(), SkipVoiceValues.size()),
      "SkipVoiceValues");

  auto drawType = EnsureGetMember<Game::DrawComponentType>("DrawType");

  UI::OptionsMenuPtr = new UI::CHLCC::OptionsMenu();
  UI::Menus[drawType].push_back(UI::OptionsMenuPtr);
}

}  // namespace OptionsMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto
