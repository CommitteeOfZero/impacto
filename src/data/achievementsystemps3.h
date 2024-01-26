#pragma once

#include <cstdint>

#include "achievementsystem.h"

#define PS3_MAGIC 0xDCA24D00
#define TROPHY_TYPES "BSGP"

namespace Impacto {
namespace AchievementSystem {

typedef enum {
  Bronze = 'B',
  Silver = 'S',
  Gold = 'G',
  Platinum = 'P'
} TrophyType;

// From: https://www.psdevwiki.com/ps3/TROPHY.TRP
struct TrophyDataHeader {
  uint32_t magic;
  uint32_t version;  // 1
  uint64_t file_size;
  uint32_t file_count;
  uint32_t entry_size;
  uint32_t dev_flag;  // 1: dev
};

struct TrophyDataEntry {
  char name[32];
  uint64_t offset;
  uint64_t size;
};

class Trophy : public Achievement {
 private:
  TrophyType ttype;

 public:
  Trophy(std::string name, std::string description, bool hidden,
         TrophyType ttype, const Sprite icon)
      : Achievement(std::move(name), std::move(description), hidden, icon),
        ttype(ttype) {}
};

class AchievementSystemPS3 : public AchievementSystemBase {
 public:
  virtual bool MountAchievementFile() = 0;
};

}  // namespace AchievementSystem
}  // namespace Impacto