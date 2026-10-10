#pragma once

#include "savesystem.h"
#include "../../data/savesystem.h"
#include "../../texture/texture.h"
#include "../../io/memorystream.h"
#include "../../spritesheet.h"
#include <optional>

namespace Impacto {
namespace CCLCC_Switch {

using namespace Impacto::SaveSystem;

constexpr size_t SaveEntrySize = 0x1F9E8;
constexpr size_t SystemSaveSize = 0x21020;
constexpr int SaveFileSize = SaveEntrySize * MaxSaveEntries;

constexpr int SaveThumbnailWidth = 240;
constexpr int SaveThumbnailHeight = 135;
// CCLCC Switch Save thumbnails are 240x135 RGB16
constexpr int SaveThumbnailSize =
    SaveThumbnailWidth * SaveThumbnailHeight * 4 / 2;

struct SaveFileEntry final : CCLCC::SaveFileEntry {
 public:
  std::array<uint8_t, 150> FlagWorkScript1{};  // 150 bytes from &FlagWork[50]
  std::array<uint8_t, 100> FlagWorkScript2{};  // 100 bytes from &FlagWork[400]
  std::array<int, 800> ScrWorkScript1{};       // 3200 bytes from &ScrWork[1000]
  std::array<int, 3000> ScrWorkScript2{};  // 12000 bytes from &ScrWork[4300]
  std::array<uint8_t, 0x6ac8> MapLoadData{};
  std::array<uint8_t, 0x68> YesNoData{};
  std::array<int, 303> WaveData{};
  std::array<uint8_t, SaveThumbnailSize> ThumbnailData{};

  std::span<uint8_t> GetFlagWorkScript1() override { return FlagWorkScript1; }
  std::span<uint8_t> GetFlagWorkScript2() override { return FlagWorkScript2; }
  std::span<int> GetScrWorkScript1() override { return ScrWorkScript1; }
  std::span<int> GetScrWorkScript2() override { return ScrWorkScript2; }
  std::span<uint8_t> GetMapLoadData() override { return MapLoadData; }
  std::span<uint8_t> GetYesNoData() override { return YesNoData; }
  std::span<int> GetWaveData() override { return WaveData; }
  std::span<uint8_t> GetThumbnailData() override { return ThumbnailData; }

  std::span<const uint8_t> GetFlagWorkScript1() const override {
    return FlagWorkScript1;
  }
  std::span<const uint8_t> GetFlagWorkScript2() const override {
    return FlagWorkScript2;
  }
  std::span<const int> GetScrWorkScript1() const override {
    return ScrWorkScript1;
  }
  std::span<const int> GetScrWorkScript2() const override {
    return ScrWorkScript2;
  }
  std::span<const uint8_t> GetMapLoadData() const override {
    return MapLoadData;
  }
  std::span<const uint8_t> GetYesNoData() const override { return YesNoData; }
  std::span<const int> GetWaveData() const override { return WaveData; }
  std::span<const uint8_t> GetThumbnailData() const override {
    return ThumbnailData;
  }
};

class SaveSystem final : public CCLCC::SaveSystem {
 private:
  std::array<uint8_t, 1024> GameExtraData{};
  std::array<uint8_t, 0x20000> MessageFlags{};
  std::array<uint8_t, SystemSaveSize> SystemData{};
  std::array<uint8_t, 1200> EVFlags{};
  std::array<uint8_t, 256> BGMFlags{};

  std::span<uint8_t> GetGameExtraData() override { return GameExtraData; }
  std::span<const uint8_t> GetGameExtraData() const override {
    return GameExtraData;
  }
  std::span<uint8_t> GetMessageFlags() override { return MessageFlags; }
  std::span<const uint8_t> GetMessageFlags() const override {
    return MessageFlags;
  }
  std::span<uint8_t> GetSystemData() override { return SystemData; }
  std::span<uint8_t> GetEVFlags() override { return EVFlags; }
  std::span<const uint8_t> GetEVFlags() const override { return EVFlags; }
  std::span<uint8_t> GetBGMFlags() override { return BGMFlags; }
  std::span<const uint8_t> GetBGMFlags() const override { return BGMFlags; }

 public:
  SaveSystem() = default;

  SaveError CheckSaveFile() const override;
  SaveError MountSaveFile(std::vector<QueuedTexture>& textures) override;
  void LoadEntry(SaveType type, int id) override;

  const CCLCC::SaveLayout& GetSaveLayout() const override;
  void InitSaveSlots() override;

  SaveError WriteQuickSaveFile() override;

  // Flags and Scr
  void WriteWorkScriptData(Io::MemoryStream& stream,
                           CCLCC::SaveFileEntry& entry) override;
  void ReadWorkScriptData(Io::MemoryStream& stream,
                          CCLCC::SaveFileEntry& entry) override;
  void WriteWaveData(Io::MemoryStream& stream,
                     CCLCC::SaveFileEntry& entry) override;
  void ReadWaveData(Io::MemoryStream& stream,
                    CCLCC::SaveFileEntry& entry) override;

  void WriteBGMFlags(Io::MemoryStream& stream) override;
  void ReadBGMFlags(Io::MemoryStream& stream) override;

  void LoadScrWork() override;

  SaveError WriteSaveFile() override;

  void UpdateWorkingSaveEntry(CCLCC::SaveFileEntry* entry) override;
};

}  // namespace CCLCC_Switch
}  // namespace Impacto