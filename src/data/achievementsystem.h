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

  std::string const &Name() const { return name; }
  std::string const &Description() const { return description; }
  bool Hidden() const { return hidden; }
  Sprite const &Icon() const { return icon; }

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
  virtual const Achievement *GetAchievement(int id) = 0;
};

inline AchievementSystemBase *Implementation = nullptr;

void Init();

bool MountAchievementFile();
const Achievement *GetAchievement(int id);

}  // namespace AchievementSystem
}  // namespace Impacto