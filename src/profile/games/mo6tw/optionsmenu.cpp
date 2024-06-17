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
  GetMemberSpriteArray(VoiceToggleEnabledSprites, VoiceToggleCount,
                       "VoiceToggleEnabledSprites");
  GetMemberSpriteArray(VoiceToggleDisabledSprites, VoiceToggleCount,
                       "VoiceToggleDisabledSprites");
  VoiceToggleHighlightSprite =
      EnsureGetMemberSprite("VoiceToggleHighlightSprite");

  VoiceToggleStart = EnsureGetMemberVec2("VoiceToggleStart");
  VoiceTogglePadding = EnsureGetMemberVec2("VoiceTogglePadding");
  VoiceTogglePerLine = EnsureGetMemberInt("VoiceTogglePerLine");

  GetMemberSpriteArray(SectionHeaderSprites, SectionHeaderSpriteCount,
                       "SectionHeaderSprites");

  CheckboxBoxSprite = EnsureGetMemberSprite("CheckboxBoxSprite");
  CheckboxTickSprite = EnsureGetMemberSprite("CheckboxTickSprite");
  GetMemberSpriteArray(CheckboxLabelSprites, CheckboxLabelCount,
                       "CheckboxLabelSprites");

  FirstPageSliderPos = EnsureGetMemberVec2("FirstPageSliderPos");
  FirstPageSliderMargin = EnsureGetMemberFloat("FirstPageSliderMargin");
  SliderThumbOffset = EnsureGetMemberVec2("SliderThumbOffset");
  CheckboxLabelOffset = EnsureGetMemberVec2("CheckboxLabelOffset");
  CheckboxFirstPos = EnsureGetMemberVec2("CheckboxFirstPos");
  CheckboxFirstSectionPaddingX =
      EnsureGetMemberFloat("CheckboxFirstSectionPaddingX");
  CheckboxMargin = EnsureGetMemberVec2("CheckboxMargin");
  CheckboxSecondPos = EnsureGetMemberVec2("CheckboxSecondPos");
  CheckboxSecondSectionFirstPaddingX =
      EnsureGetMemberFloat("CheckboxSecondSectionFirstPaddingX");
  GetMemberFloatArray(AutoSaveTriggerXPos, AutoSaveTriggerXPosCount,
                      "AutoSaveTriggerXPos");
  ScreenSizeSliderPos = EnsureGetMemberVec2("ScreenSizeSliderPos");
  TipsPos = EnsureGetMemberVec2("TipsPos");
  FirstPageSectionHeaderPos = EnsureGetMemberVec2("FirstPageSectionHeaderPos");
  SecondPageSectionHeaderPos =
      EnsureGetMemberVec2("SecondPageSectionHeaderPos");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::OptionsMenuPtr = new UI::MO6TW::OptionsMenu();
  UI::Menus[drawType].push_back(UI::OptionsMenuPtr);
}

}  // namespace OptionsMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto
