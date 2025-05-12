#pragma once

#include "../../data/savesystem.h"
#include "../../texture/texture.h"
#include "../../io/memorystream.h"
#include "../../spritesheet.h"
#include <optional>

namespace Impacto {
namespace CCLCC {

using namespace Impacto::SaveSystem;

constexpr int SaveFileSize = 0x1b110 * MaxSaveEntries * 2 + 0x387c;
constexpr int SaveThumbnailWidth = 240;
constexpr int SaveThumbnailHeight = 135;

class SaveFileEntry : public SaveFileEntryBase {
 public:
  uint8_t FlagWorkScript1[50];   // 50 bytes from &FlagWork[50]
  uint8_t FlagWorkScript2[100];  // 100 bytes from &FlagWork[300]
  int ScrWorkScript1[600];       // 2400 bytes from &ScrWork[1000]
  int ScrWorkScript2[3000];      // 12000 bytes from &ScrWork[4300]
  uint8_t MapLoadData[0x6ac8];
  uint8_t YesNoData[0x54];
};

class SaveSystem : public SaveSystemBase {
 public:
  SaveError CreateSaveFile() override;
  SaveError CheckSaveFile() override;
  SaveError MountSaveFile() override;
  void LoadSystemBuffer(Io::MemoryStream& memoryStream);
  void SaveSystemBuffer(Io::MemoryStream& memoryStream);
  void LoadEntryBuffer(Io::MemoryStream& memoryStream, SaveFileEntry& entry,
                       SaveType saveType);
  void SaveEntryBuffer(Io::MemoryStream& memoryStream, SaveFileEntry& entry,
                       SaveType saveType);
  void SaveMemory() override;
  void LoadEntry(SaveType type, int id) override;
  void LoadMemoryNew(LoadProcess load) override;
  void FlushWorkingSaveEntry(SaveType type, int id, int autoSaveType) override;
  void WriteSaveFile() override;
  uint32_t GetSavePlayTime(SaveType type, int id) override;
  uint8_t GetSaveFlags(SaveType type, int id) override;
  tm const& GetSaveDate(SaveType type, int id) override;
  uint8_t GetSaveStatus(SaveType type, int id) override;
  int GetSaveTitle(SaveType type, int id) override;
  uint32_t GetTipStatus(int tipId) override;
  void SetTipStatus(int tipId, bool isLocked, bool isUnread,
                    bool isNew) override;
  void SetLineRead(int scriptId, int lineId) override;
  bool IsLineRead(int scriptId, int lineId) override;
  void GetReadMessagesCount(int* totalMessageCount,
                            int* readMessageCount) override;
  void GetViewedEVsCount(int* totalEVCount, int* viewedEVCount) override;
  void GetEVStatus(int evId, int* totalVariations,
                   int* viewedVariations) override;
  bool GetEVVariationIsUnlocked(int evId, int variationIdx) override;
  bool GetBgmFlag(int id) override;
  void SetCheckpointId(int id) override;
  Sprite& GetSaveThumbnail(SaveType type, int id) override;

 private:
  uint8_t GameExtraData[1024];
  uint8_t MessageFlags[10000];
  bool EVFlags[1200];
  uint8_t BGMFlags[200];
  std::optional<SaveFileEntry> WorkingSaveEntry;
};

}  // namespace CCLCC
}  // namespace Impacto