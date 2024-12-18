#pragma once

#include "../../../spritesheet.h"
#include "../../../spriteanimation.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace OptionsMenu {

int constexpr PageHeaderSpriteCount = 4;
int constexpr PagePanelSpriteCount = 8;
int constexpr LabelSpriteCount = 16;
int constexpr NametagSpriteCount = 13;
int constexpr PortraitSpriteCount = 26;

inline glm::vec2 BackgroundPosition;

inline glm::vec3 HighlightColor;

inline Sprite PointerSprite;
inline glm::vec2 PointerOffset;

inline Sprite HeaderSprite;
inline glm::vec2 HeaderPosition;
inline Sprite PageHeaderSprites[PageHeaderSpriteCount];
inline glm::vec2 PageHeaderPosition;

inline Sprite PagePanelSprite;
inline glm::vec2 PagePanelPosition;
inline Sprite PagePanelSprites[PagePanelSpriteCount];
inline glm::vec2 PagePanelIconOffsets[PagePanelSpriteCount];
inline SpriteAnimationDef PoleAnimation;

inline Sprite SliderTrackSprite;
inline glm::vec2 SliderTrackOffset;
inline Sprite VoiceSliderTrackSprite;
inline glm::vec2 VoiceSliderOffset;
inline Sprite BinaryBoxSprite;
inline glm::vec2 BinaryBoxOffset;
inline float SliderSpeed;

inline Sprite SkipReadSprite;
inline Sprite SkipAllSprite;
inline Sprite OnSprite;
inline Sprite OffSprite;
inline Sprite YesSprite;
inline Sprite NoSprite;

inline Sprite GuideSprite;
inline Sprite VoiceGuideSprite;
inline glm::vec2 GuidePosition;

inline glm::vec2 EntriesStartPosition;
inline int EntriesVerticalOffset;
inline glm::vec2 SoundEntriesStartPosition;
inline int SoundEntriesVerticalOffset;
inline glm::vec2 VoiceEntriesOffset;
inline glm::vec2 EntryDimensions;
inline glm::vec2 VoiceEntryDimensions;

inline Sprite LabelSprites[LabelSpriteCount];
inline glm::vec2 LabelOffset;
inline Sprite NametagSprites[NametagSpriteCount];
inline glm::vec2 NametagOffset;
inline Sprite PortraitSprites[PortraitSpriteCount];
inline glm::vec2 PortraitOffset;
inline glm::vec2 VoicePosition;

void Configure();

}  // namespace OptionsMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto