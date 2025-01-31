#pragma once

#include "../../ui/ui.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace OptionsMenu {

inline Impacto::UI::OptionsMenuType Type = Impacto::UI::OptionsMenuType::None;

inline float FadeInDuration;
inline float FadeOutDuration;

inline float MinButtonHoldTime;
inline float ButtonHoldFireInterval;

void Configure();

}  // namespace OptionsMenu
}  // namespace Profile
}  // namespace Impacto