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

void Configure();

}  // namespace GameSpecific
}  // namespace Profile
}  // namespace Impacto