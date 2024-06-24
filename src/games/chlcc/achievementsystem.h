#pragma once

#include <cstdint>

#include "../../data/achievementsystemps3.h"
namespace Impacto {
namespace CHLCC {

int constexpr inline TROPHY_DATA_ENTRY_NUM = 54;
int constexpr inline TROPHY_NUM = 51;

using namespace Impacto::AchievementSystem;

class AchievementSystem : public AchievementSystemPS3 {
 public:
  bool MountAchievementFile() override;
  const Achievement* GetAchievement(int id) override;

 private:
  TrophyDataEntry TrophyDataEntries[TROPHY_DATA_ENTRY_NUM];
  Trophy* Trophies[TROPHY_NUM];
};
}  // namespace CHLCC
}  // namespace Impacto