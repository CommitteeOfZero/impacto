#pragma once

#include "../../data/savesystem.h"
#include "../../io/memorystream.h"

namespace Impacto {
namespace CHLCC {

using namespace Impacto::SaveSystem;

constexpr size_t SaveEntrySize = 0x2000;
constexpr size_t SaveFileSize = SaveEntrySize * MaxSaveEntries * 2 + 0x3b06;

class SaveFileEntry : public SaveFileEntryBase {
 public:
  std::array<uint8_t, 50> FlagWorkScript1;   // 50 bytes from &FlagWork[50]
  std::array<uint8_t, 100> FlagWorkScript2;  // 100 bytes from &FlagWork[300]
  std::array<int, 300> ScrWorkScript1;       // 1200 bytes from &ScrWork[300]
  std::array<int, 1300> ScrWorkScript2;      // 5200 bytes from &ScrWork[2300]
};

class SaveSystem : public SaveSystemBase {
 public:
  SaveError CreateSaveFile() override;
  SaveError CheckSaveFile() override;
  SaveError MountSaveFile(std::vector<QueuedTexture>& textures) override;

  SaveError LoadSystemData() override;
  void SaveSystemData() override;

  void SaveThumbnailData() override {};  // Todo
  Sprite& GetSaveThumbnail(SaveType type, int id) override;

  void LoadEntryBuffer(Io::MemoryStream& memoryStream, SaveFileEntry& entry);
  void SaveEntryBuffer(Io::MemoryStream& memoryStream, SaveFileEntry& entry);
  void LoadEntry(SaveType type, int id) override;
  void FlushWorkingSaveEntry(SaveType type, int id, int autoSaveType) override;

  void SaveMemory() override;

  SaveError WriteSaveFile() override;
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
  void SetEVStatus(int id) override;
  bool GetEVVariationIsUnlocked(size_t evId, size_t variationIdx) override;

  bool GetBgmFlag(int id) override;
  void SetBgmFlag(int id, bool flag) override;

  void SetCheckpointId(int id) override {}

 private:
  uint8_t GameExtraData[1024];
  uint8_t MessageFlags[10000];
  std::array<uint8_t, 0x3b06> SystemData;
  bool EVFlags[1200];
  uint8_t BGMFlags[100];
};

}  // namespace CHLCC
}  // namespace Impacto