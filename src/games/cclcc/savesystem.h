#pragma once

#include "../../data/savesystem.h"
#include "../../texture/texture.h"
#include "../../io/memorystream.h"
#include "../../spritesheet.h"
#include <optional>

namespace Impacto {
namespace CCLCC {

using namespace Impacto::SaveSystem;

struct SaveLayout {
  size_t ConfigOffset;
  size_t VoiceOffset;
  size_t SkipVoiceOffset;
  size_t AdvanceTextOffset;
  size_t QuickSortedIdOffset;
  bool QuickSortedIdIsLoaded;
  size_t EVFlagsOffset;
  size_t BgmFlagsOffset;
  size_t MessageFlagsOffset;
  size_t ExtraDataOffset;

  size_t SysFlagWorkDst1;
  size_t SysFlagWorkDst2;
  size_t SysScrWorkDst1;
  size_t SysScrWorkLen1;

  int64_t FlagWork2Src;
  int64_t MainThreadOffset;
  int64_t MainThreadBufIdOffset;
  int64_t WaveOffset;
  int64_t MapLoadOffset;
  int64_t ThumbnailPadding;

  int ThumbnailWidth, ThumbnailHeight;
};
struct SaveFileEntry : SaveFileEntryBase {
  virtual std::span<uint8_t> GetFlagWorkScript1() = 0;
  virtual std::span<const uint8_t> GetFlagWorkScript1() const = 0;
  virtual std::span<uint8_t> GetFlagWorkScript2() = 0;
  virtual std::span<const uint8_t> GetFlagWorkScript2() const = 0;
  virtual std::span<int> GetScrWorkScript1() = 0;
  virtual std::span<const int> GetScrWorkScript1() const = 0;
  virtual std::span<int> GetScrWorkScript2() = 0;
  virtual std::span<const int> GetScrWorkScript2() const = 0;
  virtual std::span<uint8_t> GetMapLoadData() = 0;
  virtual std::span<const uint8_t> GetMapLoadData() const = 0;
  virtual std::span<uint8_t> GetYesNoData() = 0;
  virtual std::span<const uint8_t> GetYesNoData() const = 0;
  virtual std::span<int> GetWaveData() = 0;
  virtual std::span<const int> GetWaveData() const = 0;
  virtual std::span<uint8_t> GetThumbnailData() = 0;
  virtual std::span<const uint8_t> GetThumbnailData() const = 0;

  virtual ~SaveFileEntry() = default;
};

class SaveSystem : public SaveSystemBase {
 public:
  uint32_t static CalculateChecksum(std::span<const uint8_t> bufferData,
                                    uint16_t initSum = 0, uint16_t initXor = 0,
                                    bool swapSrcBytes = false);

  // Flags and Scr
  virtual void WriteWorkScriptData(Io::MemoryStream& stream,
                                   SaveFileEntry& entry) = 0;
  virtual void ReadWorkScriptData(Io::MemoryStream& stream,
                                  SaveFileEntry& entry) = 0;
  virtual void WriteWaveData(Io::MemoryStream& stream,
                             SaveFileEntry& entry) = 0;
  virtual void ReadWaveData(Io::MemoryStream& stream, SaveFileEntry& entry) = 0;

  virtual void WriteBGMFlags(Io::MemoryStream& stream) = 0;
  virtual void ReadBGMFlags(Io::MemoryStream& stream) = 0;

  virtual void LoadScrWork() = 0;

  virtual const SaveLayout& GetSaveLayout() const = 0;
  virtual void InitSaveSlots() = 0;

  SaveError LoadSystemData() override;
  void SaveSystemData() override;
  void InitializeSystemData() override;

  void SaveThumbnailData() override;
  Sprite& GetSaveThumbnail(SaveType type, int id) override;

  void LoadEntryBuffer(Io::MemoryStream& memoryStream, SaveFileEntry& entry,
                       SaveType saveType, Texture& tex);
  void SaveEntryBuffer(Io::MemoryStream& memoryStream, SaveFileEntry& entry,
                       SaveType saveType);
  void FlushWorkingSaveEntry(SaveType type, int id, int autoSaveType) override;

  void SaveMemory() override;
  void LoadMemoryNew(LoadProcess load) override;
  uint32_t GetSavePlayTime(SaveType type, int id) const override;
  uint8_t GetSaveFlags(SaveType type, int id) const override;
  void SetSaveFlags(SaveType type, int id, uint8_t flags) override;
  tm const& GetSaveDate(SaveType type, int id) const override;
  uint8_t GetSaveStatus(SaveType type, int id) const override;
  int GetSaveTitle(SaveType type, int id) const override;

  uint32_t GetTipStatus(size_t tipId) const override;
  void SetTipStatus(size_t tipId, bool isLocked, bool isUnread,
                    bool isNew) override;

  void SetLineRead(size_t scriptId, size_t lineId) override;
  bool IsLineRead(size_t scriptId, size_t lineId) const override;
  void GetReadMessagesCount(int* totalMessageCount,
                            int* readMessageCount) const override;

  void GetViewedEVsCount(int* totalEVCount, int* viewedEVCount) const override;
  void GetEVStatus(int evId, int* totalVariations,
                   int* viewedVariations) const override;
  void SetEVStatus(int id) override;
  bool GetEVVariationIsUnlocked(size_t evId,
                                size_t variationIdx) const override;

  bool GetBgmFlag(int id) const override;
  void SetBgmFlag(int id, bool flag) override;

  void SetCheckpointId(int id) override;

  void WaveSave(std::span<int> data);
  void WaveLoad(std::span<const int> data) const;

  virtual void UpdateWorkingSaveEntry(SaveFileEntry* entry) = 0;

 protected:
  virtual std::span<uint8_t> GetGameExtraData() = 0;
  virtual std::span<const uint8_t> GetGameExtraData() const = 0;
  virtual std::span<uint8_t> GetMessageFlags() = 0;
  virtual std::span<const uint8_t> GetMessageFlags() const = 0;
  virtual std::span<uint8_t> GetSystemData() = 0;
  virtual std::span<uint8_t> GetEVFlags() = 0;
  virtual std::span<const uint8_t> GetEVFlags() const = 0;
  virtual std::span<uint8_t> GetBGMFlags() = 0;
  virtual std::span<const uint8_t> GetBGMFlags() const = 0;

  std::unique_ptr<SaveFileEntry> WorkingSaveEntry;
};

}  // namespace CCLCC
}  // namespace Impacto