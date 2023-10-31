#pragma once

#include "enum.h"

namespace Impacto {
namespace AchievementSystem {

BETTER_ENUM(AchievementDataType, int, None, CHLCC)

enum AchievementError { AchievementOK = 0, AchievementNotFound = 2 };

class AchievementSystemBase {
 public:
  virtual AchievementError MountAchievementFile() = 0;

 private:
};

extern AchievementSystemBase *Implementation;

void Init();

AchievementError MountAchievementFile();

}  // namespace AchievementSystem
}  // namespace Impacto