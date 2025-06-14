#include "optionsmenu.h"
#include "../../profile_internal.h"
#include "../../../log.h"

#include "../../ui/optionsmenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/mo6tw/optionsmenu.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace OptionsMenu {

void Configure() {
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");

  GetMemberArray<Sprite>(VoiceToggleEnabledSprites, VoiceToggleCount,
                         "VoiceToggleEnabledSprites");
  GetMemberArray<Sprite>(VoiceToggleDisabledSprites, VoiceToggleCount,
                         "VoiceToggleDisabledSprites");
  VoiceToggleHighlightSprite =
      EnsureGetMember<Sprite>("VoiceToggleHighlightSprite");

  VoiceToggleStart = EnsureGetMember<glm::vec2>("VoiceToggleStart");
  VoiceTogglePadding = EnsureGetMember<glm::vec2>("VoiceTogglePadding");
  VoiceTogglePerLine = EnsureGetMember<int>("VoiceTogglePerLine");

  GetMemberArray<Sprite>(SectionHeaderSprites, SectionHeaderSpriteCount,
                         "SectionHeaderSprites");

  SliderTrackSprite = EnsureGetMember<Sprite>("SliderTrackSprite");
  SliderFillSprite = EnsureGetMember<Sprite>("SliderFillSprite");
  SliderThumbSprite = EnsureGetMember<Sprite>("SliderThumbSprite");

  CheckboxBoxSprite = EnsureGetMember<Sprite>("CheckboxBoxSprite");
  CheckboxTickSprite = EnsureGetMember<Sprite>("CheckboxTickSprite");
  GetMemberArray<Sprite>(CheckboxLabelSprites, CheckboxLabelCount,
                         "CheckboxLabelSprites");

  FirstPageSliderPos = EnsureGetMember<glm::vec2>("FirstPageSliderPos");
  FirstPageSliderMargin = EnsureGetMember<float>("FirstPageSliderMargin");
  SliderThumbOffset = EnsureGetMember<glm::vec2>("SliderThumbOffset");
  CheckboxLabelOffset = EnsureGetMember<glm::vec2>("CheckboxLabelOffset");
  CheckboxFirstPos = EnsureGetMember<glm::vec2>("CheckboxFirstPos");
  CheckboxFirstSectionPaddingX =
      EnsureGetMember<float>("CheckboxFirstSectionPaddingX");
  CheckboxMargin = EnsureGetMember<glm::vec2>("CheckboxMargin");
  CheckboxSecondPos = EnsureGetMember<glm::vec2>("CheckboxSecondPos");
  CheckboxSecondSectionFirstPaddingX =
      EnsureGetMember<float>("CheckboxSecondSectionFirstPaddingX");
  GetMemberArray<float>(AutoSaveTriggerXPos, AutoSaveTriggerXPosCount,
                        "AutoSaveTriggerXPos");
  ScreenSizeSliderPos = EnsureGetMember<glm::vec2>("ScreenSizeSliderPos");
  TipsPos = EnsureGetMember<glm::vec2>("TipsPos");
  FirstPageSectionHeaderPos =
      EnsureGetMember<glm::vec2>("FirstPageSectionHeaderPos");
  SecondPageSectionHeaderPos =
      EnsureGetMember<glm::vec2>("SecondPageSectionHeaderPos");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  UI::OptionsMenuPtr = new UI::MO6TW::OptionsMenu();
  UI::Menus[drawType].push_back(UI::OptionsMenuPtr);
}

}  // namespace OptionsMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto
