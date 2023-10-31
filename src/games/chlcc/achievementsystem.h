#pragma once

#include <cstdint>

#include "../../data/achievementsystem.h"

namespace Impacto {
namespace CHLCC {

using namespace Impacto::AchievementSystem;

typedef enum { Bronze, Silver, Gold, Platinum } TrophyType;

// From: https://www.psdevwiki.com/ps3/TROPHY.TRP
struct TrophyDataHeader {
  uint32_t magic;
  uint32_t version;  // 1
  uint64_t file_size;
  uint32_t files_count;
  uint32_t element_size;
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

class AchievementSystem : public AchievementSystemBase {
 public:
  AchievementError MountAchievementFile() override;

 private:
  TrophyDataEntry TrophyDataEntries[51];
  Trophy Trophies[51];
};
}  // namespace CHLCC
}  // namespace Impacto