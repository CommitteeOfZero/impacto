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
inline float SelectedSlideDuration;
inline Sprite SelectedLabelSprite;
inline glm::vec2 SelectedLabelOffset;
inline Sprite SelectedDotSprite;
inline glm::vec2 SelectedDotOffset;
inline glm::vec2 SelectedDotVoicesOffset;
inline Sprite VoiceMutedSprite;

inline float SelectedLabelBaseSpeed;
inline float SelectedLabelModalDistancePerEntry;

inline Sprite BasicSettingsSprite;
inline glm::vec2 BasicSettingsPos;
inline Sprite TextSettingsSprite;
inline glm::vec2 TextSettingsPos;
inline Sprite SoundSettingsSprite;
inline glm::vec2 SoundSettingsPos;
inline Sprite VoiceSettingsSprite;
inline glm::vec2 VoiceSettingsPos;

inline Sprite SliderBarBaseSprite;
inline Sprite SliderBarFillSprite;
inline float SliderBarFadeDuration;
inline glm::vec2 SliderBarTopRightOffset;
inline glm::vec2 SliderBarFillOffset;

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
inline glm::vec2 SettingButtonTopRightOffset;

inline std::array<glm::vec2, 8> TextPageEntryPositions;
inline std::array<glm::vec2, 6> SoundPageEntryPositions;
inline std::array<glm::vec2, 11> VoicePageEntryPositions;

inline std::array<bool, 2> TriggerStopSkipValues;
inline std::array<bool, 2> ShowTipsNotificationValues;
inline std::array<uint8_t, 4> AutoQuickSaveValues;
inline std::array<uint8_t, 2> ControllerTypeValues;
inline std::array<float, 4> TextSpeedValues;
inline std::array<float, 3> AutoSpeedValues;
inline std::array<bool, 2> SkipReadValues;
inline std::array<bool, 2> SyncVoiceValues;
inline std::array<bool, 2> SkipVoiceValues;

void Configure();

}  // namespace OptionsMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto