#pragma once

#include <enum.h>
#include "../spritesheet.h"
#include "../loadable.h"

namespace Impacto {
namespace AchievementSystem {

BETTER_ENUM(AchievementDataType, int, None, PS3)

enum class AchievementError {
  OK = 0,
  InProgress = 1,
  OutOfDiskSpace = 4,
  Failed = 100,
};

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
  virtual AchievementError MountAchievementFile(
      std::function<void(void)> &mainThreadCallback) = 0;
  //  virtual bool UnlockAchievement(int id) = 0;
  virtual const Achievement *GetAchievement(int id) = 0;
  virtual size_t GetAchievementCount() const = 0;
};

inline AchievementSystemBase *Implementation = nullptr;

void Init();

LoadStatus GetLoadStatus();
void MountAchievementFile();
const Achievement *GetAchievement(int id);
size_t GetAchievementCount();

}  // namespace AchievementSystem
}  // namespace Impacto