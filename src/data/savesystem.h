#pragma once
#include "../impacto.h"

#include <string>
#include <enum.h>
#include <ctime>
#include "../log.h"

namespace Impacto {
namespace SaveSystem {

BETTER_ENUM(SaveDataType, int, None, CHLCC, MO6TW)

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

uint8_t const Flbit[] = {1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80};

struct ScriptMessageDataPair {
  uint32_t LineCount;
  uint32_t SaveDataOffset;
};

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
  virtual uint8_t GetSaveSatus(SaveType type, int id) = 0;
  virtual int GetSaveTitle(SaveType type, int id) = 0;
  virtual uint32_t GetTipStatus(int tipId) = 0;
  virtual void SetTipStatus(int tipId, bool isLocked, bool isUnread,
                            bool isNew) = 0;
  virtual void GetReadMessagesCount(int* totalMessageCount,
                                    int* readMessageCount) = 0;

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
uint8_t GetSaveSatus(SaveType type, int id);
int GetSaveTitle(SaveType type, int id);
uint32_t GetTipStatus(int tipId);
void SetTipStatus(int tipId, bool isLocked, bool isUnread, bool isNew);
void GetReadMessagesCount(int* totalMessageCount, int* readMessageCount);

}  // namespace SaveSystem
}  // namespace Impacto