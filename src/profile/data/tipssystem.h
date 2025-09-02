#pragma once

#include "../../data/tipssystem.h"

namespace Impacto {
namespace Profile {
namespace TipsSystem {

inline Impacto::TipsSystem::TipsSystemType Type =
    Impacto::TipsSystem::TipsSystemType::None;

inline size_t MaxTipsCount;

void Configure();

}  // namespace TipsSystem
}  // namespace Profile
}  // namespace Impacto