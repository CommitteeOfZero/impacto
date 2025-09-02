#include "optionsmenu.h"
#include "../../profile_internal.h"
#include "../../../log.h"
#include "../../../text.h"

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
  CircleStartPosition = EnsureGetMember<glm::vec2>("CircleStartPosition");
  CircleOffset = EnsureGetMember<float>("CircleOffset");

  ErinPosition = EnsureGetMember<glm::vec2>("ErinPosition");
  ErinSprite = EnsureGetMember<Sprite>("ErinSprite");

  BackgroundFilter = EnsureGetMember<Sprite>("BackgroundFilter");

  InitialRedBarPosition = EnsureGetMember<glm::vec2>("InitialRedBarPosition");
  RightRedBarPosition = EnsureGetMember<glm::vec2>("RightRedBarPosition");
  RedBarDivision = EnsureGetMember<float>("RedBarDivision");
  RedBarBaseX = EnsureGetMember<float>("RedBarBaseX");
  InitialRedBarSprite = EnsureGetMember<Sprite>("RedBarSprite");
  RedBarLabelPosition = EnsureGetMember<glm::vec2>("RedBarLabelPosition");
  RedBarLabel = EnsureGetMember<Sprite>("RedBarLabel");

  MenuTitleTextRightPosition =
      EnsureGetMember<glm::vec2>("MenuTitleTextRightPos");
  MenuTitleTextAngle = EnsureGetMember<float>("MenuTitleTextAngle");
  TitleFadeInDuration = EnsureGetMember<float>("TitleFadeInDuration");
  TitleFadeOutDuration = EnsureGetMember<float>("TitleFadeOutDuration");
  MenuTitleText = EnsureGetMember<Sprite>("MenuTitleText");

  ButtonPromptSprite = EnsureGetMember<Sprite>("ButtonPromptSprite");
  ButtonPromptPosition = EnsureGetMember<glm::vec2>("ButtonPromptPosition");

  SelectedSprite = EnsureGetMember<Sprite>("SelectedSprite");
  SelectedLabelSprite = EnsureGetMember<Sprite>("SelectedLabelSprite");
  SelectedDotSprite = EnsureGetMember<Sprite>("SelectedDotSprite");
  VoiceMutedSprite = EnsureGetMember<Sprite>("VoiceMutedSprite");

  BasicSettingsSprite = EnsureGetMember<Sprite>("BasicSettingsSprite");
  TextSettingsSprite = EnsureGetMember<Sprite>("TextSettingsSprite");
  SoundSettingsSprite = EnsureGetMember<Sprite>("SoundSettingsSprite");
  VoiceSettingsSprite = EnsureGetMember<Sprite>("VoiceSettingsSprite");

  SliderBarBaseSprite = EnsureGetMember<Sprite>("SliderBarBaseSprite");
  SliderBarFillSprite = EnsureGetMember<Sprite>("SliderBarFillSprite");

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

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  UI::OptionsMenuPtr = new UI::CHLCC::OptionsMenu();
  UI::Menus[drawType].push_back(UI::OptionsMenuPtr);
}

}  // namespace OptionsMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto
