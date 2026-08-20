#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include <memory>

#include "achievementsystem.h"
#include "../texture/texture.h"

namespace Impacto {
namespace AchievementSystem {

enum class AchievementRarity : uint8_t {
  Bronze,
  Silver,
  Gold,
  Platinum,
};

struct QueuedAchievement {
  std::string Name;
  std::string Description;
  bool Hidden;
  AchievementRarity Rarity;
  std::optional<Texture> IconTexture;
};

class CommonAchievement : public Achievement {
  AchievementRarity Rarity;

 public:
  CommonAchievement(std::string name, std::string description, bool hidden,
                    AchievementRarity rarity, Sprite const& icon,
                    Sprite const& lockedIcon)
      : Achievement(std::move(name), std::move(description), hidden, icon,
                    lockedIcon),
        Rarity(rarity) {}

  AchievementRarity GetRarity() const { return Rarity; }
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
