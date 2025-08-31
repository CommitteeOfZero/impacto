#pragma once
#include "../impacto.h"
#include "../vm/vm.h"

#include <string>
#include <enum.h>
#include <ctime>
#include "../log.h"
#include "../spritesheet.h"
#include "../texture/texture.h"
#include "../loadable.h"

namespace Impacto {
namespace SaveSystem {

BETTER_ENUM(SaveDataType, int, None, CHLCC, CCLCC, MO6TW)

enum SaveFlagsMode { WriteProtect = 1 };

enum class SaveError {
  OK = 0,
  InProgress = 1,
  NotFound = 2,
  WrongUser = 3,
  Restart = 4,
  OutOfDiskSpace = 4,
  CorruptedFixing = 5,
  Restart10 = 10,
  Failed = 100,
  Corrupted = 255
};

enum class SaveType { Full = 0, Quick = 1 };

enum class LoadProcess { Vars = 0, Thread = 1 };

int constexpr MaxSaveEntries = 48;

constexpr std::array<uint8_t, 8> Flbit = {1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80};

struct ScriptMessageDataPair {
  uint32_t LineCount;
  uint32_t SaveDataOffset;
};

// A delayed Texture::Submit() operation to be executed on the main thread
struct QueuedTexture {
  std::reference_wrapper<uint32_t> Id;
  Texture Tex{};
};

class SaveFileEntryBase {
 public:
  uint8_t Status = 0;
  uint32_t Checksum = 0;
  std::tm SaveDate{};
  uint32_t PlayTime = 0;
  uint32_t SwTitle = 0;
  uint8_t Flags = 0;
  uint32_t SaveType = 0;
  uint32_t MainThreadExecPriority = 0;
  uint32_t MainThreadGroupId = 0;
  uint32_t MainThreadWaitCounter = 0;
  uint32_t MainThreadScriptParam = 0;
  uint32_t MainThreadIp = 0;
  uint32_t MainThreadLoopCounter = 0;
  uint32_t MainThreadLoopAdr = 0;
  uint32_t MainThreadCallStackDepth = 0;
  union {
    std::array<uint32_t, 8> MainThreadReturnAddresses{};
    std::array<uint32_t, 8> MainThreadReturnIds;
  };
  std::array<uint32_t, 8> MainThreadReturnBufIds{};
  uint32_t MainThreadScriptBufferId = 0;
  std::array<int, 16> MainThreadVariables{};
  uint32_t MainThreadDialoguePageId = 0;
  Sprite SaveThumbnail{};
};

class SaveSystemBase {
 public:
  virtual SaveError CheckSaveFile() = 0;
  virtual SaveError MountSaveFile(std::vector<QueuedTexture>& textures) = 0;

  virtual void SaveMemory() = 0;
  virtual void LoadEntry(SaveType type, int id) = 0;
  virtual void LoadMemoryNew(LoadProcess){};
  virtual void FlushWorkingSaveEntry(SaveType type, int id,
                                     int autoSaveType) = 0;

  // Reads and writes to system data are only safe if no other VM threads are
  // executing. This data should be copied to a buffer so that
  // flushing can happen without blocking other VM threads.
  virtual void SaveSystemData() = 0;
  virtual SaveError LoadSystemData() = 0;
  virtual void InitializeSystemData() = 0;

  virtual void SaveThumbnailData() = 0;
  virtual SaveError WriteSaveFile() = 0;
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
  virtual void SetEVStatus(int id) = 0;
  virtual bool GetEVVariationIsUnlocked(size_t evId, size_t variationIdx) = 0;
  virtual bool GetBgmFlag(int id) = 0;
  virtual void SetBgmFlag(int id, bool flag) = 0;
  virtual void SetCheckpointId(int id) = 0;
  virtual Sprite& GetSaveThumbnail(SaveType type, int id) = 0;
  int GetQuickSaveOpenSlot() {
    for (int i = 0; i < MaxSaveEntries; i++) {
      if (QuickSaveEntries[i]->Status == 0) return i;
    }
    for (int i = 0; i < MaxSaveEntries; i++) {
      if (!(GetSaveFlags(SaveType::Quick, i) & WriteProtect)) return i;
    }
    return -1;
  }
  Sprite& GetWorkingSaveThumbnail() { return WorkingSaveThumbnail; }

 protected:
  SaveFileEntryBase* FullSaveEntries[MaxSaveEntries];
  SaveFileEntryBase* QuickSaveEntries[MaxSaveEntries];
  int QuickSaveCount;
  Sprite WorkingSaveThumbnail;
};

inline SaveSystemBase* Implementation = nullptr;

void Init();

LoadStatus GetLoadStatus();
void CheckSaveFile();
void MountSaveFile();
void SaveMemory();
void SaveThumbnailData();
void SaveSystemData();
SaveError LoadSystemData();
void InitializeSystemData();
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
void SetEVStatus(int id);
bool GetEVVariationIsUnlocked(size_t evId, size_t variationIdx);
bool GetBgmFlag(int id);
void SetBgmFlag(int id, bool setFlag);
void SetCheckpointId(int id);
int GetQuickSaveOpenSlot();
Sprite& GetSaveThumbnail(SaveType type, int id);
Sprite& GetWorkingSaveThumbnail();

}  // namespace SaveSystem
}  // namespace Impacto