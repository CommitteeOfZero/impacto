#pragma once

#include <cstdint>

#include "achievementsystem.h"

#define PS3_MAGIC 0xDCA24D00

namespace Impacto {
namespace AchievementSystem {

typedef enum { Bronze, Silver, Gold, Platinum } TrophyType;

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

struct Trophy {
  int id;
  bool hidden;
  TrophyType ttype;
  char name[50];
  char detail[50];
};

class AchievementSystemPS3 : public AchievementSystemBase {
 public:
  virtual bool MountAchievementFile() = 0;
};

}  // namespace AchievementSystem
}  // namespace Impacto