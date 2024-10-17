#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace OptionsMenu {

int constexpr VoiceToggleCount = 14;
int constexpr SectionHeaderSpriteCount = 13 * 2;
int constexpr CheckboxLabelCount = 14;
int constexpr AutoSaveTriggerXPosCount = 4;

inline Sprite VoiceToggleEnabledSprites[VoiceToggleCount];
inline Sprite VoiceToggleDisabledSprites[VoiceToggleCount];
inline Sprite VoiceToggleHighlightSprite;

inline glm::vec2 VoiceToggleStart;
inline glm::vec2 VoiceTogglePadding;
inline int VoiceTogglePerLine;

inline Sprite SectionHeaderSprites[SectionHeaderSpriteCount];

inline Sprite SliderTrackSprite;
inline Sprite SliderFillSprite;
inline Sprite SliderThumbSprite;

inline Sprite CheckboxBoxSprite;
inline Sprite CheckboxTickSprite;
inline Sprite CheckboxLabelSprites[CheckboxLabelCount];

inline glm::vec2 FirstPageSliderPos;
inline float FirstPageSliderMargin;
inline glm::vec2 SliderThumbOffset;
inline glm::vec2 CheckboxLabelOffset;
inline glm::vec2 CheckboxFirstPos;
inline float CheckboxFirstSectionPaddingX;
inline glm::vec2 CheckboxMargin;
inline glm::vec2 CheckboxSecondPos;
inline float CheckboxSecondSectionFirstPaddingX;
inline float AutoSaveTriggerXPos[AutoSaveTriggerXPosCount];
inline glm::vec2 ScreenSizeSliderPos;
inline glm::vec2 TipsPos;
inline glm::vec2 FirstPageSectionHeaderPos;
inline glm::vec2 SecondPageSectionHeaderPos;

void Configure();

}  // namespace OptionsMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto