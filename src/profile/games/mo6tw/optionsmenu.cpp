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

Sprite VoiceToggleEnabledSprites[VoiceToggleCount];
Sprite VoiceToggleDisabledSprites[VoiceToggleCount];
Sprite VoiceToggleHighlightSprite;

glm::vec2 VoiceToggleStart;
glm::vec2 VoiceTogglePadding;
int VoiceTogglePerLine;

Sprite SectionHeaderSprites[SectionHeaderSpriteCount];

Sprite CheckboxBoxSprite;
Sprite CheckboxTickSprite;
Sprite CheckboxLabelSprites[CheckboxLabelCount];

glm::vec2 FirstPageSliderPos;
float FirstPageSliderMargin;
glm::vec2 SliderThumbOffset;
glm::vec2 CheckboxLabelOffset;
glm::vec2 CheckboxFirstPos;
float CheckboxFirstSectionPaddingX;
glm::vec2 CheckboxMargin;
glm::vec2 CheckboxSecondPos;
float CheckboxSecondSectionFirstPaddingX;
float AutoSaveTriggerXPos[AutoSaveTriggerXPosCount];
glm::vec2 ScreenSizeSliderPos;
glm::vec2 TipsPos;
glm::vec2 FirstPageSectionHeaderPos;
glm::vec2 SecondPageSectionHeaderPos;

static void GetMemberSpriteArray(Sprite* arr, uint32_t count,
                                 char const* name) {
  EnsurePushMemberOfType(name, kArrayType);

  if (TopVal().Size() != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d sprites for %s\n", count,
           name);
    Window->Shutdown();
  }

  for (uint32_t i = 0; i < count; i++) {
    arr[i] = EnsureGetArrayElementSprite(i);
  }

  Pop();
}

static void GetMemberFloatArray(float* arr, uint32_t count, char const* name) {
  EnsurePushMemberOfType(name, kArrayType);

  if (TopVal().Size() != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d floats for %s\n", count,
           name);
    Window->Shutdown();
  }

  for (uint32_t i = 0; i < count; i++) {
    arr[i] = EnsureGetArrayElementFloat(i);
  }

  Pop();
}

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
