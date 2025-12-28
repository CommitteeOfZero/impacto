#pragma once

#include "../../data/savesystem.h"
#include "../../texture/texture.h"
#include "../../io/memorystream.h"
#include "../../spritesheet.h"
#include <optional>

namespace Impacto {
namespace CCLCC {

using namespace Impacto::SaveSystem;

constexpr size_t SaveEntrySize = 0x1b110;
constexpr int SaveFileSize = SaveEntrySize * MaxSaveEntries * 2 + 0x387c;

constexpr int SaveThumbnailWidth = 240;
constexpr int SaveThumbnailHeight = 135;
// CCLCC PS4 Save thumbnails are 240x135 RGB16
constexpr int SaveThumbnailSize =
    SaveThumbnailWidth * SaveThumbnailHeight * 4 / 2;

class SaveFileEntry : public SaveFileEntryBase {
 public:
  std::array<uint8_t, 50> FlagWorkScript1;   // 50 bytes from &FlagWork[50]
  std::array<uint8_t, 100> FlagWorkScript2;  // 100 bytes from &FlagWork[300]
  std::array<int, 600> ScrWorkScript1;       // 2400 bytes from &ScrWork[1000]
  std::array<int, 3000> ScrWorkScript2;      // 12000 bytes from &ScrWork[4300]
  std::array<uint8_t, 0x6ac8> MapLoadData;
  std::array<uint8_t, 0x54> YesNoData;
  std::array<int, 303>
      WaveData;  // 3 wave types * 20 waves * 5 fields + 3 counts
  std::array<uint8_t, SaveThumbnailSize> ThumbnailData;
};

class SaveSystem : public SaveSystemBase {
 public:
  SaveError CheckSaveFile() override;
  SaveError MountSaveFile(std::vector<QueuedTexture>& textures) override;

  SaveError LoadSystemData() override;
  void SaveSystemData() override;
  void InitializeSystemData() override;

  void SaveThumbnailData() override;
  Sprite& GetSaveThumbnail(SaveType type, int id) override;

  void LoadEntryBuffer(Io::MemoryStream& memoryStream, SaveFileEntry& entry,
                       SaveType saveType, Texture& tex);
  void SaveEntryBuffer(Io::MemoryStream& memoryStream, SaveFileEntry& entry,
                       SaveType saveType);
  void LoadEntry(SaveType type, int id) override;
  void FlushWorkingSaveEntry(SaveType type, int id, int autoSaveType) override;

  void SaveMemory() override;
  void LoadMemoryNew(LoadProcess load) override;

  SaveError WriteSaveFile() override;
  uint32_t GetSavePlayTime(SaveType type, int id) override;
  uint8_t GetSaveFlags(SaveType type, int id) override;
  void SetSaveFlags(SaveType type, int id, uint8_t flags) override;
  tm const& GetSaveDate(SaveType type, int id) override;
  uint8_t GetSaveStatus(SaveType type, int id) override;
  int GetSaveTitle(SaveType type, int id) override;

  uint32_t GetTipStatus(size_t tipId) override;
  void SetTipStatus(size_t tipId, bool isLocked, bool isUnread,
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

  void SetCheckpointId(int id) override;

  void WaveSave(std::span<int> data);
  void WaveLoad(std::span<const int> data) const;

 private:
  uint8_t GameExtraData[1024];
  uint8_t MessageFlags[10000];
  std::array<uint8_t, 0x387c> SystemData;
  bool EVFlags[1200];
  uint8_t BGMFlags[200];
  std::optional<SaveFileEntry> WorkingSaveEntry;
};

}  // namespace CCLCC
}  // namespace Impacto