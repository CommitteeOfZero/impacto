#pragma once

#include <vector>

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace SG0 {
namespace OptionsMenu {

void Configure();

int constexpr PageCount = 4;  // BASIC, TEXT, SOUND, VOICE
int constexpr VoicePage = 3;

// What a row edits. Choice rows list their choice words in the profile;
// the others are MIN-MAX sliders.
enum class Setting : int {
  FullScreen,
  Resolution,
  TriggerStopSkip,
  ShowTipsNotification,
  AutoQuickSave,
  TextSpeed,
  AutoSpeed,
  SkipRead,
  VoiceVolume,
  MusicVolume,
  SEVolume,
  MovieVolume,
  SyncVoice,
  SkipVoice,
  CharacterVoice,  // per-character volume and mute (Index = voice id)
};

struct Row {
  int Page = 0;
  Setting Kind = Setting::TextSpeed;
  int Index = 0;
  Sprite Label;
  std::vector<Sprite> Choices;
};

inline std::vector<Row> Rows;

inline Sprite BackgroundSprite;
inline Sprite GuideSprite;
inline glm::vec2 GuidePos;
inline Sprite VoiceGuideSprite;
inline glm::vec2 VoiceGuidePos;
inline Sprite PageButtonSprites[PageCount];  // highlighted page buttons
inline RectF PageButtonBounds[PageCount];
inline Sprite PageHeadingSprites[PageCount];
inline glm::vec2 HeadingPos;

inline float ContentX;
inline float RowsTop;
inline float RowHeight;
inline float VoiceRowHeight;
inline float RowPadding;
inline float RowWidth;

inline Sprite SliderFrameSprite;
inline Sprite SliderFillSprite;
inline Sprite SliderKnobSprite;
inline glm::vec2 SliderOffset;      // frame, relative to the row's label
inline glm::vec2 SliderFillOffset;  // fill, relative to the frame
inline float SliderKnobOffsetY;
inline float VoiceSliderScale;  // the VOICE page's 16 rows are tighter
inline float VoiceSliderOffsetY;
inline Sprite VoiceOffSprite;

inline glm::vec3 HighlightColor;
inline float HighlightOpacity;
inline float UnselectedBrightness;

// SW_SYSSUBMENUCT value at which the screen is fully shown.
inline int CountMax;

}  // namespace OptionsMenu
}  // namespace SG0
}  // namespace Profile
}  // namespace Impacto
