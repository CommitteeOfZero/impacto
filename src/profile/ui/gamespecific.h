#pragma once

#include "../../ui/ui.h"

namespace Impacto {
namespace Profile {
namespace GameSpecific {

inline Impacto::UI::GameSpecificType GameSpecificType =
    Impacto::UI::GameSpecificType::None;

inline std::optional<std::vector<Sprite>> UIButtonGuideSprites;

void Configure();

}  // namespace GameSpecific
}  // namespace Profile
}  // namespace Impacto