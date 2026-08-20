#pragma once

#include <vector>
#include <memory>

#include "achievementsystem.h"
#include "../texture/texture.h"

namespace Impacto {
namespace AchievementSystem {

enum class AchievementRarity : int {
  Bronze,
  Silver,
  Gold,
  Platinum,
};

struct QueuedAchievement {
  std::string name;
  std::string description;
  bool hidden;
  AchievementRarity rarity;
  Texture texture;
  bool hasTexture;
};

class CommonAchievement : public Achievement {
  AchievementRarity rarity;

 public:
  CommonAchievement(std::string name, std::string description, bool hidden,
                    AchievementRarity rarity, Sprite const& icon)
      : Achievement(std::move(name), std::move(description), hidden, icon),
        rarity(rarity) {}

  AchievementRarity Rarity() const { return rarity; }
};

class AchievementSystemCommon : public AchievementSystemBase {
 public:
  AchievementError MountAchievementFile(
      std::function<void()>& mainThreadCallback) override;
  const CommonAchievement* GetAchievement(int id) override;
  size_t GetAchievementCount() const override;

 private:
  std::vector<std::unique_ptr<CommonAchievement>> Achievements;
};

}  // namespace AchievementSystem
}  // namespace Impacto
