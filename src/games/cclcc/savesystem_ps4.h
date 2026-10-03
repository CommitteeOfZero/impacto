#pragma once
#include "savesystem.h"

#include "../../data/savesystem.h"
#include "../../texture/texture.h"
#include "../../io/memorystream.h"
#include "../../spritesheet.h"
#include <optional>

namespace Impacto {
namespace CCLCC_PS4 {

using namespace Impacto::SaveSystem;

constexpr size_t SaveEntrySize = 0x1b110;
constexpr size_t SystemSaveSize = 0x387c;
constexpr int SaveFileSize =
    SaveEntrySize * MaxSaveEntries * 2 + SystemSaveSize;

constexpr int SaveThumbnailWidth = 240;
constexpr int SaveThumbnailHeight = 135;
// CCLCC PS4 Save thumbnails are 240x135 RGB16
constexpr int SaveThumbnailSize =
    SaveThumbnailWidth * SaveThumbnailHeight * 4 / 2;

class SaveSystem final : public CCLCC::SaveSystem {
 public:
  SaveSystem() {
    GameExtraData.assign(1024, 0);
    MessageFlags.assign(10000, 0);
    SystemData.assign(SystemSaveSize, 0);
    EVFlags.assign(1200, 0);
    BGMFlags.assign(200, 0);
  }

  SaveError CheckSaveFile() const override;
  SaveError MountSaveFile(std::vector<QueuedTexture>& textures) override;

  CCLCC::SaveLayout GetSaveLayout() const override;

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
};

}  // namespace CCLCC_PS4
}  // namespace Impacto