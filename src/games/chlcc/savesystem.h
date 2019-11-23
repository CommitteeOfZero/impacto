#pragma once

#include "../../savesystem.h"

namespace Impacto {
namespace CHLCC {

using namespace Impacto::SaveSystem;

class SaveFileEntry : public SaveFileEntryBase {
 public:
  uint8_t FlagWorkScript1[50];    // 50 bytes from &FlagWork[50]
  uint8_t FlagWorkScript2[100];  // 100 bytes from &FlagWork[300]
  int ScrWorkScript1[300];       // 1200 bytes from &ScrWork[300]
  int ScrWorkScript2[1300];      // 5200 bytes from &ScrWork[2300]
};

class SaveSystem : public SaveSystemBase {
 public:
  SaveError MountSaveFile();
  void SaveMemory(SaveType type, int id);
  void LoadMemory(SaveType type, int id);
};

}  // namespace CHLCC
}  // namespace Impacto