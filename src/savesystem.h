#pragma once
#include "impacto.h"

#include <string>
#include <enum.h>
#include <time.h>
#include "log.h"

namespace Impacto {
namespace SaveSystem {

BETTER_ENUM(SaveDataType, int, None, CHLCC)

enum SaveError {
  SaveOK = 0,
  SaveNotFound = 2,
  SaveWrongUser = 3,
  SaveRestart = 4,
  SaveCorruptedFixing = 5,
  SaveRestart10 = 10,
  SaveFailed = 100,
  SaveCorrupted = 255
};

enum SaveType { SaveFull = 0, SaveQuick = 1 };

int const MaxSaveEntries = 48;

class SaveFileEntryBase {
 public:
  uint8_t Status;
  uint32_t Checksum;
  std::tm SaveDate;
  uint32_t PlayTime;
  uint16_t SwTitle;
  uint32_t MainThreadExecPriority;
  uint32_t MainThreadGroupId;
  uint32_t MainThreadWaitCounter;
  uint32_t MainThreadScriptParam;
  uint32_t MainThreadIp;
  uint32_t MainThreadLoopCounter;
  uint32_t MainThreadLoopAdr;
  uint32_t MainThreadCallStackDepth;
  uint32_t MainThreadReturnAddresses[8];
  uint32_t MainThreadReturnBufIds[8];
  int MainThreadVariables[16];
  uint32_t MainThreadDialoguePageId;
};

class SaveSystemBase {
 public:
  virtual SaveError MountSaveFile() = 0;
  virtual void SaveMemory() = 0;
  virtual void LoadMemory(SaveType type, int id) = 0;
  virtual void FlushWorkingSaveEntry(SaveType type, int id) = 0;
  virtual void WriteSaveFile() = 0;

  SaveFileEntryBase* FullSaveEntries[MaxSaveEntries];
  SaveFileEntryBase* QuickSaveEntries[MaxSaveEntries];
};

extern SaveSystemBase* Implementation;

void Init();

SaveError MountSaveFile();
void SaveMemory();
void LoadMemory(SaveType type, int id);
void FlushWorkingSaveEntry(SaveType type, int id);
void WriteSaveFile();

}  // namespace SaveSystem
}  // namespace Impacto