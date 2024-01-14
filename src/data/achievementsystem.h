#pragma once

#include "enum.h"
#include "../spritesheet.h"

namespace Impacto {
namespace AchievementSystem {

BETTER_ENUM(AchievementDataType, int, None, CHLCC)

class Achievement {
 public:
  Achievement(const std::string &name, const std::string &description,
              const Sprite &icon)
      : name(name), description(description), icon(icon) {}

 private:
  std::string name;
  std::string description;
  Sprite icon;
};

class AchievementSystemBase {
 public:
  virtual bool MountAchievementFile() = 0;
  //  virtual bool UnlockAchievement(int id) = 0;
  //  virtual Achievement GetAchievement(int id) = 0;
};

extern AchievementSystemBase *Implementation;

void Init();

bool MountAchievementFile();

}  // namespace AchievementSystem
}  // namespace Impacto