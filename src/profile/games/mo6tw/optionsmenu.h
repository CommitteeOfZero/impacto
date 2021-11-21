#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace OptionsMenu {

int const VoiceToggleCount = 14;
int const SectionHeaderSpriteCount = 13 * 2;
int const CheckboxLabelCount = 14;
int const AutoSaveTriggerXPosCount = 4;

extern Sprite VoiceToggleEnabledSprites[VoiceToggleCount];
extern Sprite VoiceToggleDisabledSprites[VoiceToggleCount];
extern Sprite VoiceToggleHighlightSprite;

extern glm::vec2 VoiceToggleStart;
extern glm::vec2 VoiceTogglePadding;
extern int VoiceTogglePerLine;

extern Sprite SectionHeaderSprites[SectionHeaderSpriteCount];

extern Sprite CheckboxBoxSprite;
extern Sprite CheckboxTickSprite;
extern Sprite CheckboxLabelSprites[CheckboxLabelCount];

extern glm::vec2 FirstPageSliderPos;
extern float FirstPageSliderMargin;
extern glm::vec2 SliderThumbOffset;
extern glm::vec2 CheckboxLabelOffset;
extern glm::vec2 CheckboxFirstPos;
extern float CheckboxFirstSectionPaddingX;
extern glm::vec2 CheckboxMargin;
extern glm::vec2 CheckboxSecondPos;
extern float CheckboxSecondSectionFirstPaddingX;
extern float AutoSaveTriggerXPos[AutoSaveTriggerXPosCount];
extern glm::vec2 ScreenSizeSliderPos;
extern glm::vec2 TipsPos;
extern glm::vec2 FirstPageSectionHeaderPos;
extern glm::vec2 SecondPageSectionHeaderPos;

void Configure();

}  // namespace OptionsMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto