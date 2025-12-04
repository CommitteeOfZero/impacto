#pragma once

#include "../../ui/ui.h"

namespace Impacto {
namespace Profile {
namespace GameSpecific {

inline Impacto::UI::GameSpecificType GameSpecificType =
    Impacto::UI::GameSpecificType::None;

inline std::optional<std::vector<Sprite>> UIButtonGuideSprites;
inline std::optional<RectF> UIButtonGuideEndDisp;
inline Sprite MonitorScanline;

inline std::vector<Sprite> ButterflySprites;
inline uint8_t ButterflyFrameCount;
inline float ButterflyFlapFrameDuration;
inline float ButterflyFadeDuration;

void Configure();

}  // namespace GameSpecific
}  // namespace Profile
}  // namespace Impacto