#pragma once

#include "enum.h"
#include "../spritesheet.h"

namespace Impacto {
namespace AchievementSystem {

BETTER_ENUM(AchievementDataType, int, None, CHLCC)

class Achievement {
 public:
  Achievement(std::string name, std::string description, bool hidden,
              const Sprite &icon)
      : name(std::move(name)),
        description(std::move(description)),
        hidden(hidden),
        icon(icon) {}

 protected:
  std::string name;
  std::string description;
  bool hidden;
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