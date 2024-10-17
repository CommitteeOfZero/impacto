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

inline Sprite PointerSprite;

inline Sprite HeaderSprite;
inline Sprite PageHeaderSprites[PageHeaderSpriteCount];
inline SpriteAnimationDef PoleAnimation;

inline Sprite SliderTrackSprite;
inline Sprite VoiceSliderTrackSprite;
inline Sprite BinaryBoxSprite;

inline Sprite SkipReadSprite;
inline Sprite SkipAllSprite;
inline Sprite OnSprite;
inline Sprite OffSprite;
inline Sprite YesSprite;
inline Sprite NoSprite;

inline Sprite GuideSprite;
inline Sprite VoiceGuideSprite;

inline Sprite LabelSprites[LabelSpriteCount];
inline Sprite NametagSprites[NametagSpriteCount];
inline Sprite PortraitSprites[PortraitSpriteCount];

void Configure();

}  // namespace OptionsMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto