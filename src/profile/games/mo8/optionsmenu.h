#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO8 {
namespace OptionsMenu {

extern Sprite NextButtonSprite;
extern Sprite NextButtonHighlightedSprite;
extern glm::vec2 NextButtonPosition;
extern Sprite BackButtonSprite;
extern Sprite BackButtonHighlightedSprite;
extern glm::vec2 BackButtonPosition;

extern Sprite ButtonHighlight;
extern glm::vec2 PageLabelPosition;
extern glm::vec2 ListStartingPosition;
extern glm::vec2 ListPadding;
extern glm::vec2 OptionGroupItemsOffset;
extern glm::vec2 OptionGroupSliderOffset;

static int const TextSpeedOptionsNumMax = 4;

extern Sprite TextPageLabel;
extern Sprite TextSpeedOptionsLabel;
extern Sprite TextSpeedOptionsLabelH;
extern int TextSpeedOptionsNum;
extern Sprite TextSpeedOptionsSprites[TextSpeedOptionsNumMax];
extern Sprite TextSpeedOptionsHSprites[TextSpeedOptionsNumMax];

static int const AutoModeOptionsNumMax = 3;

extern Sprite AutoModeOptionsLabel;
extern Sprite AutoModeOptionsLabelH;
extern int AutoModeOptionsNum;
extern Sprite AutoModeOptionsSprites[AutoModeOptionsNumMax];
extern Sprite AutoModeOptionsHSprites[AutoModeOptionsNumMax];

static int const SkipModeOptionsNumMax = 3;

extern Sprite SkipModeOptionsLabel;
extern Sprite SkipModeOptionsLabelH;
extern int SkipModeOptionsNum;
extern Sprite SkipModeOptionsSprites[SkipModeOptionsNumMax];
extern Sprite SkipModeOptionsHSprites[SkipModeOptionsNumMax];

static int const SoundOptionsNumMax = 2;

extern int SoundModeOptionsNum;
extern Sprite SoundPageLabel;
extern Sprite SoundModeOptionsSprites[SoundOptionsNumMax];
extern Sprite SoundModeOptionsHSprites[SoundOptionsNumMax];

extern Sprite VoiceSyncOptionsLabel;
extern Sprite VoiceSyncOptionsLabelH;

extern Sprite VoiceSkipOptionsLabel;
extern Sprite VoiceSkipOptionsLabelH;

extern Sprite VoiceHighlightOptionsLabel;
extern Sprite VoiceHighlightOptionsLabelH;

extern Sprite BgmVolumeLabel;
extern Sprite BgmVolumeLabelH;
extern Sprite VoiceVolumeLabel;
extern Sprite VoiceVolumeLabelH;
extern Sprite SeVolumeLabel;
extern Sprite SeVolumeLabelH;
extern Sprite SystemSeVolumeLabel;
extern Sprite SystemSeVolumeLabelH;
extern Sprite CharacterVoiceVolumeLabel;
extern Sprite CharacterVoiceVolumeLabelH;

extern Sprite OtherPageLabel;

static int const QuickSaveOptionsNumMax = 3;

extern Sprite QuickSaveOptionsLabel;
extern Sprite QuickSaveOptionsLabelH;
extern int QuickSaveOptionsNum;
extern Sprite QuickSaveOptionsSprites[QuickSaveOptionsNumMax];
extern Sprite QuickSaveOptionsHSprites[QuickSaveOptionsNumMax];

void Configure();

}  // namespace OptionsMenu
}  // namespace MO8
}  // namespace Profile
}  // namespace Impacto