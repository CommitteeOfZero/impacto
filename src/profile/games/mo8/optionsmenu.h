#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO8 {
namespace OptionsMenu {

inline Sprite NextButtonSprite;
inline Sprite NextButtonHighlightedSprite;
inline glm::vec2 NextButtonPosition;
inline Sprite BackButtonSprite;
inline Sprite BackButtonHighlightedSprite;
inline glm::vec2 BackButtonPosition;

inline Sprite ButtonHighlight;
inline glm::vec2 PageLabelPosition;
inline glm::vec2 ListStartingPosition;
inline glm::vec2 ListPadding;
inline glm::vec2 OptionGroupItemsOffset;
inline glm::vec2 OptionGroupSliderOffset;

int constexpr TextSpeedOptionsNumMax = 4;

inline Sprite TextPageLabel;
inline Sprite TextSpeedOptionsLabel;
inline Sprite TextSpeedOptionsLabelH;
inline int TextSpeedOptionsNum;
inline Sprite TextSpeedOptionsSprites[TextSpeedOptionsNumMax];
inline Sprite TextSpeedOptionsHSprites[TextSpeedOptionsNumMax];

int constexpr AutoModeOptionsNumMax = 3;

inline Sprite AutoModeOptionsLabel;
inline Sprite AutoModeOptionsLabelH;
inline int AutoModeOptionsNum;
inline Sprite AutoModeOptionsSprites[AutoModeOptionsNumMax];
inline Sprite AutoModeOptionsHSprites[AutoModeOptionsNumMax];

int constexpr SkipModeOptionsNumMax = 3;

inline Sprite SkipModeOptionsLabel;
inline Sprite SkipModeOptionsLabelH;
inline int SkipModeOptionsNum;
inline Sprite SkipModeOptionsSprites[SkipModeOptionsNumMax];
inline Sprite SkipModeOptionsHSprites[SkipModeOptionsNumMax];

int constexpr SoundOptionsNumMax = 2;

inline int SoundModeOptionsNum;
inline Sprite SoundPageLabel;
inline Sprite SoundModeOptionsSprites[SoundOptionsNumMax];
inline Sprite SoundModeOptionsHSprites[SoundOptionsNumMax];

inline Sprite VoiceSyncOptionsLabel;
inline Sprite VoiceSyncOptionsLabelH;

inline Sprite VoiceSkipOptionsLabel;
inline Sprite VoiceSkipOptionsLabelH;

inline Sprite VoiceHighlightOptionsLabel;
inline Sprite VoiceHighlightOptionsLabelH;

inline Sprite BgmVolumeLabel;
inline Sprite BgmVolumeLabelH;
inline Sprite VoiceVolumeLabel;
inline Sprite VoiceVolumeLabelH;
inline Sprite SeVolumeLabel;
inline Sprite SeVolumeLabelH;
inline Sprite SystemSeVolumeLabel;
inline Sprite SystemSeVolumeLabelH;
inline Sprite CharacterVoiceVolumeLabel;
inline Sprite CharacterVoiceVolumeLabelH;

inline Sprite OtherPageLabel;

int constexpr QuickSaveOptionsNumMax = 3;

inline Sprite QuickSaveOptionsLabel;
inline Sprite QuickSaveOptionsLabelH;
inline int QuickSaveOptionsNum;
inline Sprite QuickSaveOptionsSprites[QuickSaveOptionsNumMax];
inline Sprite QuickSaveOptionsHSprites[QuickSaveOptionsNumMax];

void Configure();

}  // namespace OptionsMenu
}  // namespace MO8
}  // namespace Profile
}  // namespace Impacto