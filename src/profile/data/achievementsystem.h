#pragma once

#include <string>
#include <vector>

#include "../../data/achievementsystem.h"
#include "../../data/achievementsystemcommon.h"

namespace Impacto {
namespace Profile {
namespace AchievementSystem {

inline Impacto::AchievementSystem::AchievementDataType Type =
    Impacto::AchievementSystem::AchievementDataType::None;

inline std::string AchievementDataPath;

struct AchievementDef {
  std::string Name;
  std::string Description;
  bool Hidden;
  Impacto::AchievementSystem::AchievementRarity Rarity;
  std::string IconPath;
};
inline std::vector<AchievementDef> Achievements;

void Configure();

}  // namespace AchievementSystem
}  // namespace Profile
}  // namespace Impacto