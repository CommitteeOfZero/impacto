#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace DelusionTrigger {

inline Sprite BackgroundSprite;
inline Sprite BackgroundSpriteMask;
inline Sprite ScreenMask;
inline Sprite LeftDelusionHeartSprite;
inline Sprite RightDelusionHeartSprite;
inline glm::vec2 LeftDelusionHeartPos;
inline glm::vec2 RightDelusionHeartPos;
inline float DelusionHeartPulseDuration;
inline float DelusionHeartPulseScale;
inline std::array<std::vector<Sprite>, 21> DelusionTextGlyphs;
inline float DelusionTextXVelocity;
inline float DelusionTextFadeDuration;
inline float DelusionScaledGlyphWidth;
inline float DelusionScaledGlyphHeight;
inline float DelusionTextLineSpacing;

void Configure();

}  // namespace DelusionTrigger
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto
