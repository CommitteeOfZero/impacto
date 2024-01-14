#pragma once

#include <cstdint>

#include "../../data/achievementsystemps3.h"

#define TROPHY_DATA_ENTRY_NUM 54
#define TROPHY_NUM 51

namespace Impacto {
namespace CHLCC {

using namespace Impacto::AchievementSystem;

class AchievementSystem : public AchievementSystemPS3 {
 public:
  bool MountAchievementFile() override;

 private:
  TrophyDataEntry TrophyDataEntries[TROPHY_DATA_ENTRY_NUM];
  Trophy Trophies[TROPHY_NUM];
};
}  // namespace CHLCC
}  // namespace Impacto