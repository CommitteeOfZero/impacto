#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace OptionsMenu {

inline uint32_t BackgroundColor;

inline Sprite CircleSprite;
inline glm::vec2 CircleStartPosition;
inline float CircleOffset;

inline Sprite ErinSprite;
inline glm::vec2 ErinPosition;

inline Sprite BackgroundFilter;

inline glm::vec2 InitialRedBarPosition;
inline glm::vec2 RightRedBarPosition;
inline float RedBarDivision;
inline float RedBarBaseX;
inline Sprite RedBarSprite;
inline Sprite InitialRedBarSprite;
inline glm::vec2 RedBarPosition;
inline glm::vec2 RedBarLabelPosition;
inline Sprite RedBarLabel;

inline glm::vec2 MenuTitleTextRightPosition;
inline float MenuTitleTextAngle;
inline float TitleFadeInDuration;
inline float TitleFadeOutDuration;
inline Sprite MenuTitleText;

inline Sprite ButtonPromptSprite;
inline glm::vec2 ButtonPromptPosition;

inline Sprite SelectedSprite;
inline Sprite SelectedLabelSprite;
inline Sprite SelectedDotSprite;
inline Sprite VoiceMutedSprite;

inline Sprite BasicSettingsSprite;
inline Sprite TextSettingsSprite;
inline Sprite SoundSettingsSprite;
inline Sprite VoiceSettingsSprite;

inline Sprite SliderBarBaseSprite;
inline Sprite SliderBarFillSprite;

inline Sprite SettingInstantSprite;
inline Sprite SettingFastSprite;
inline Sprite SettingNormalSprite;
inline Sprite SettingSlowSprite;
inline Sprite SettingShortSprite;
inline Sprite SettingLongSprite;
inline Sprite SettingDoSprite;
inline Sprite SettingDontSprite;
inline Sprite SettingYesSprite;
inline Sprite SettingNoSprite;
inline Sprite SettingReadSprite;
inline Sprite SettingAllSprite;
inline Sprite SettingOnTriggerSprite;
inline Sprite SettingOnSceneSprite;
inline Sprite SettingOnTriggerAndSceneSprite;
inline Sprite SettingTypeASprite;
inline Sprite SettingTypeBSprite;

void Configure();

}  // namespace OptionsMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto