#pragma once

#include "../../data/achievementsystem.h"

namespace Impacto {
namespace Profile {
namespace AchievementSystem {

inline Impacto::AchievementSystem::AchievementDataType Type =
    Impacto::AchievementSystem::AchievementDataType::None;

inline std::string AchievementDataPath;

void Configure();

}  // namespace AchievementSystem
}  // namespace Profile
}  // namespace Impacto