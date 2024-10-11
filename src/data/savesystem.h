#pragma once
#include "../impacto.h"

#include <string>
#include <enum.h>
#include <ctime>
#include "../log.h"
#include "../spritesheet.h"

namespace Impacto {
namespace SaveSystem {

BETTER_ENUM(SaveDataType, int, None, CHLCC, CCLCC, MO6TW)

enum SaveFlagsMode { WriteProtect = 1 };

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

enum LoadProcess { LoadVars = 0, LoadThread = 1 };

int constexpr MaxSaveEntries = 48;

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
  uint32_t SwTitle;
  uint8_t Flags;
  uint32_t SaveType;
  uint32_t MainThreadExecPriority;
  uint32_t MainThreadGroupId;
  uint32_t MainThreadWaitCounter;
  uint32_t MainThreadScriptParam;
  uint32_t MainThreadIp;
  uint32_t MainThreadLoopCounter;
  uint32_t MainThreadLoopAdr;
  uint32_t MainThreadCallStackDepth;
  union {
    uint32_t MainThreadReturnAddresses[8];
    uint32_t MainThreadReturnIds[8];
  };
  uint32_t MainThreadReturnBufIds[8];
  uint32_t MainThreadScriptBufferId;
  int MainThreadVariables[16];
  uint32_t MainThreadDialoguePageId;
  Sprite SaveThumbnail;
};

class SaveSystemBase {
 public:
  virtual SaveError CreateSaveFile() = 0;
  virtual SaveError CheckSaveFile() = 0;
  virtual SaveError MountSaveFile() = 0;
  virtual void SaveMemory() = 0;
  virtual void LoadEntry(SaveType type, int id) = 0;
  virtual void LoadMemoryNew(LoadProcess){};
  virtual void FlushWorkingSaveEntry(SaveType type, int id,
                                     int autoSaveType) = 0;
  virtual void WriteSaveFile() = 0;
  virtual uint32_t GetSavePlayTime(SaveType type, int id) = 0;
  virtual uint8_t GetSaveFlags(SaveType type, int id) = 0;
  virtual tm const& GetSaveDate(SaveType type, int id) = 0;
  virtual uint8_t GetSaveStatus(SaveType type, int id) = 0;
  virtual int GetSaveTitle(SaveType type, int id) = 0;
  virtual uint32_t GetTipStatus(int tipId) = 0;
  virtual void SetTipStatus(int tipId, bool isLocked, bool isUnread,
                            bool isNew) = 0;
  virtual void SetLineRead(int scriptId, int lineId) = 0;
  virtual bool IsLineRead(int scriptId, int MessageId) = 0;
  virtual void GetReadMessagesCount(int* totalMessageCount,
                                    int* readMessageCount) = 0;
  virtual void GetViewedEVsCount(int* totalEVCount, int* viewedEVCount) = 0;
  virtual void GetEVStatus(int evId, int* totalVariations,
                           int* viewedVariations) = 0;
  virtual bool GetEVVariationIsUnlocked(int evId, int variationIdx) = 0;
  virtual bool GetBgmFlag(int id) = 0;
  virtual void SetCheckpointId(int id) = 0;
  virtual Sprite const& GetSaveThumbnail(SaveType type, int id) = 0;
  int GetQuickSaveOpenSlot() {
    for (int i = 0; i < MaxSaveEntries; i++) {
      if (QuickSaveEntries[i]->Status == 0) return i;
    }
    for (int i = 0; i < MaxSaveEntries; i++) {
      if (!(GetSaveFlags(SaveQuick, i) & WriteProtect)) return i;
    }
    return -1;
  }
  Sprite const& GetWorkingSaveThumbnail() { return WorkingSaveThumbnail; }

 protected:
  SaveFileEntryBase* FullSaveEntries[MaxSaveEntries];
  SaveFileEntryBase* QuickSaveEntries[MaxSaveEntries];
  int QuickSaveCount;
  Sprite WorkingSaveThumbnail;
};

inline SaveSystemBase* Implementation = nullptr;

void Init();

SaveError CreateSaveFile();
SaveError CheckSaveFile();
SaveError MountSaveFile();
void SaveMemory();
void LoadEntry(SaveType type, int id);
void LoadMemoryNew(LoadProcess process);
void FlushWorkingSaveEntry(SaveType type, int id, int autoSaveType = 0);
void WriteSaveFile();
uint32_t GetSavePlayTime(SaveType type, int id);
uint8_t GetSaveFlags(SaveType type, int id);
tm const& GetSaveDate(SaveType type, int id);
uint8_t GetSaveStatus(SaveType type, int id);
int GetSaveTitle(SaveType type, int id);
uint32_t GetTipStatus(int tipId);
void SetTipStatus(int tipId, bool isLocked, bool isUnread, bool isNew);
void SetLineRead(int scriptId, int lineId);
bool IsLineRead(int scriptId, int lineId);
void GetReadMessagesCount(int* totalMessageCount, int* readMessageCount);
void GetViewedEVsCount(int* totalEVCount, int* viewedEVCount);
void GetEVStatus(int evId, int* totalVariations, int* viewedVariations);
bool GetEVVariationIsUnlocked(int evId, int variationIdx);
bool GetBgmFlag(int id);
void SetCheckpointId(int id);
int GetQuickSaveOpenSlot();
Sprite const& GetSaveThumbnail(SaveType type, int id);
Sprite const& GetWorkingSaveThumbnail();

}  // namespace SaveSystem
}  // namespace Impacto