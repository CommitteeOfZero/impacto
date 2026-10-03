#include "savesystem_ps4.h"

#include "../../io/stream.h"
#include "../../io/physicalfilestream.h"
#include "../../mem.h"
#include "../../vm/vm.h"
#include "../../profile/data/savesystem.h"
#include "../../profile/scriptvars.h"
#include "../../profile/vm.h"
#include "../../renderer/renderer.h"
#include "../../profile/configsystem.h"
#include "../../effects/wave.h"
#include "../../audio/audiosystem.h"

#include "mapsystem.h"
#include "yesnotrigger.h"

#include <cstdint>
#include <ctime>
#include <system_error>
#include <ranges>

namespace Impacto {
namespace CCLCC_PS4 {

using namespace Impacto::Vm;
using namespace Impacto::Effects;
using namespace Impacto::Profile::SaveSystem;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::Vm;
using namespace Impacto::Profile::ConfigSystem;

static constexpr CCLCC::SaveLayout Ps4Layout{
    .ConfigOffset = 0x8AC,
    .VoiceOffset = 0x8BE,
    .SkipVoiceOffset = 0x901,
    .AdvanceTextOffset = 0x905,
    .QuickSortedIdOffset = 0xBCE,
    .QuickSortedIdIsLoaded = true,
    .QuickSortedIdBackCompat = true,
    .EVFlagsOffset = 0xC0E,
    .BgmFlagsOffset = 0xCA4,
    .MessageFlagsOffset = 0xD6C,
    .ExtraDataOffset = 0x347C,

    .SysFlagWorkDst1 = 100,
    .SysFlagWorkDst2 = 460,
    .SysScrWorkDst1 = 1600,
    .SysScrWorkLen1 = 400,

    .FlagWork1Len = 50,   // 50 bytes from &FlagWork[50]
    .FlagWork2Len = 100,  // 100 bytes from &FlagWork[300]
    .FlagWork2Src = 300,
    .ScrWork1Len = 600,   // (in ints) 2400 bytes from &ScrWork[1000]
    .ScrWork2Len = 3000,  // (in ints) 12000 bytes from &ScrWork[4300]
    .MainThreadOffset = 0x3958,
    .MainThreadBufIdOffset = 0x39bc,
    .WaveOffset = 0x3A04,
    .MapLoadOffset = 0x3EC0,
    .ThumbnailPadding = 0xA14,

    .MapLoadLen = 0x6ac8,
    .YesNoLen = 0x54,
    .ThumbnailWidth = 240,
    .ThumbnailHeight = 135,
};

SaveError SaveSystem::CheckSaveFile() const {
  std::error_code ec;

  IoError existsState = Io::PathExists(SaveFilePath);
  if (existsState == IoError_NotFound) {
    return SaveError::NotFound;
  } else if (existsState == IoError_Fail) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Failed to check if save file exists, error: \"{:s}\"\n",
           ec.message());
    return SaveError::Failed;
  }

  auto saveFileSize = Io::GetFileSize(SaveFilePath);
  if (saveFileSize == IoError_Fail) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Failed to get save file size, error: \"{:s}\"\n", ec.message());
    return SaveError::Failed;
  } else if (saveFileSize != SaveFileSize) {
    return SaveError::Corrupted;
  }

  Io::FilePermissionsFlags perms;
  IoError permsState = Io::GetFilePermissions(SaveFilePath, perms);
  using enum Io::FilePermissionsFlags;
  if (permsState == IoError_Fail) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Failed to get save file permissions, error: \"{:s}\"\n",
           ec.message());
    return SaveError::Failed;
  } else if ((perms & owner_read) == none || (perms & owner_write) == none) {
    return SaveError::WrongUser;
  }

  return SaveError::OK;
}

SaveError SaveSystem::MountSaveFile(std::vector<QueuedTexture>& textures) {
  Io::Stream* stream;
  IoError err = Io::PhysicalFileStream::Create(SaveFilePath, &stream);
  switch (err) {
    case IoError_NotFound:
      return SaveError::NotFound;
    case IoError_Fail:
    case IoError_Eof:
      return SaveError::Corrupted;
    case IoError_OK:
      break;
  };
  const auto l = GetSaveLayout();
  const RectF viewport = Window->GetViewport();
  WorkingSaveEntry = CCLCC::SaveFileEntry(l);
  WorkingSaveThumbnail.Sheet = SpriteSheet(viewport.Width, viewport.Height);
  WorkingSaveThumbnail.Bounds.SetSize(viewport.GetSize());

  QueuedTexture txt{
      .Id = std::ref(WorkingSaveThumbnail.Sheet.Texture),
  };
  txt.Tex.LoadSolidColor((int)WorkingSaveThumbnail.Bounds.Width,
                         (int)WorkingSaveThumbnail.Bounds.Height, 0x000000);
  textures.push_back(txt);

  Io::ReadArrayLE<uint8_t>(SystemData.data(), stream, SystemData.size());
  /*
  uint32_t systemSaveChecksum =
      CalculateChecksum(std::span(SystemData).subspan(4));
  */

  int lockedQuickSaveSlots = 0;
  textures.reserve(MaxSaveEntries * 2);
  for (auto& entryArray : {FullSaveEntries, QuickSaveEntries}) {
    SaveType saveType =
        (entryArray == QuickSaveEntries) ? SaveType::Quick : SaveType::Full;
    [[maybe_unused]] int64_t saveDataPos = stream->Position;
    for (int i = 0; i < MaxSaveEntries; i++) {
      assert(stream->Position - saveDataPos ==
             static_cast<int>(SaveEntrySize) * i);
      entryArray[i] = new CCLCC::SaveFileEntry(l);

      std::array<uint8_t, SaveEntrySize> entrySlotBuf;
      Io::ReadArrayLE<uint8_t>(entrySlotBuf.data(), stream,
                               entrySlotBuf.size());
      Io::MemoryStream saveEntryDataStream(entrySlotBuf.data(),
                                           entrySlotBuf.size(), false);

      QueuedTexture tex{
          .Id = std::ref(entryArray[i]->SaveThumbnail.Sheet.Texture),
      };
      LoadEntryBuffer(saveEntryDataStream,
                      static_cast<CCLCC::SaveFileEntry&>(*entryArray[i]),
                      saveType, tex.Tex);
      if (saveType == SaveType::Quick) {
        lockedQuickSaveSlots +=
            static_cast<CCLCC::SaveFileEntry&>(*entryArray[i]).Flags &
            WriteProtect;
      }
      textures.push_back(tex);

      // Todo, validate checksum?
    }
  }
  SetLockedQuickSaveCount(lockedQuickSaveSlots);
  SetFlag(SF_SAVEALLPROTECTED, LockedQuickSaveCount == MaxSaveEntries);

  delete stream;
  return SaveError::OK;
}

SaveError SaveSystem::WriteSaveFile() {
  using CF = Io::PhysicalFileStream::CreateFlagsMode;
  Io::Stream* stream;
  IoError err = Io::PhysicalFileStream::Create(
      SaveFilePath, &stream, CF::CREATE | CF::CREATE_DIRS | CF::WRITE);
  if (err != IoError_OK) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Failed to open save file for writing\n");
    return SaveError::Failed;
  }

  stream->Seek(0, SEEK_SET);
  Io::MemoryStream systemSaveStream =
      Io::MemoryStream(SystemData.data(), SystemData.size(), false);
  uint32_t systemChecksum = CalculateChecksum(std::span(SystemData).subspan(4));
  systemSaveStream.Seek(0, SEEK_SET);
  Io::WriteLE<uint16_t>(&systemSaveStream, systemChecksum >> 16);
  Io::WriteLE<uint16_t>(&systemSaveStream, systemChecksum & 0xFFFF);
  Io::WriteArrayLE<uint8_t>(SystemData.data(), stream, SystemData.size());
  // End system data
  for (auto* entryArray : {FullSaveEntries, QuickSaveEntries}) {
    SaveType saveType =
        (entryArray == QuickSaveEntries) ? SaveType::Quick : SaveType::Full;
    [[maybe_unused]] int64_t saveDataPos = stream->Position;
    for (int i = 0; i < MaxSaveEntries; i++) {
      CCLCC::SaveFileEntry* entry = (CCLCC::SaveFileEntry*)entryArray[i];
      if (entry == nullptr || entry->Status == 0) {
        Io::WriteLE<uint8_t>(stream, 0, SaveEntrySize);
      } else {
        assert(stream->Position - saveDataPos ==
               static_cast<int>(SaveEntrySize) * i);
        std::array<uint8_t, SaveEntrySize> entrySlotBuf{};
        Io::MemoryStream saveEntryMemoryStream(entrySlotBuf.data(),
                                               entrySlotBuf.size(), false);
        SaveEntryBuffer(saveEntryMemoryStream, *entry, saveType);
        uint32_t entryCheckSum = CalculateChecksum(
            std::span(entrySlotBuf).subspan(6, 23029 * 2), 18198, 5250, false);
        saveEntryMemoryStream.Seek(2, SEEK_SET);
        Io::WriteLE<uint16_t>(&saveEntryMemoryStream, entryCheckSum >> 16);
        Io::WriteLE<uint16_t>(&saveEntryMemoryStream, entryCheckSum & 0xFFFF);
        Io::WriteArrayLE<uint8_t>(entrySlotBuf.data(), stream,
                                  entrySlotBuf.size());
      }
    }
  }
  delete stream;
  return SaveError::OK;
}

CCLCC::SaveLayout SaveSystem::GetSaveLayout() const { return Ps4Layout; }

void SaveSystem::ReadWorkScriptData(Io::MemoryStream& stream,
                                    CCLCC::SaveFileEntry& entry) {
  Io::ReadArrayLE<uint8_t>(entry.FlagWorkScript1.data(), &stream,
                           entry.FlagWorkScript1.size());
  assert(stream.Position == 178);
  Io::ReadArrayLE<uint8_t>(entry.FlagWorkScript2.data(), &stream,
                           entry.FlagWorkScript2.size());
  Io::ReadLE<uint16_t>(&stream);
  assert(stream.Position == 280);
  Io::ReadArrayLE<int>(entry.ScrWorkScript1.data(), &stream,
                       entry.ScrWorkScript1.size());
  assert(stream.Position == 2680);
  Io::ReadArrayLE<int>(entry.ScrWorkScript2.data(), &stream,
                       entry.ScrWorkScript2.size());
}

void SaveSystem::WriteWorkScriptData(Io::MemoryStream& stream,
                                     CCLCC::SaveFileEntry& entry) {
  Io::WriteArrayLE<uint8_t>(entry.FlagWorkScript1.data(), &stream,
                            entry.FlagWorkScript1.size());
  assert(stream.Position == 178);
  Io::WriteArrayLE<uint8_t>(entry.FlagWorkScript2.data(), &stream,
                            entry.FlagWorkScript2.size());
  Io::WriteLE<uint16_t>(&stream, 0);
  assert(stream.Position == 280);
  Io::WriteArrayLE<int>(entry.ScrWorkScript1.data(), &stream,
                        entry.ScrWorkScript1.size());
  assert(stream.Position == 2680);
  Io::WriteArrayLE<int>(entry.ScrWorkScript2.data(), &stream,
                        entry.ScrWorkScript2.size());
}

void SaveSystem::WriteWaveData(Io::MemoryStream& stream,
                               CCLCC::SaveFileEntry& entry) {
  Io::WriteArrayLE<int>(entry.WaveData.data(), &stream, entry.WaveData.size());
};

void SaveSystem::ReadWaveData(Io::MemoryStream& stream,
                              CCLCC::SaveFileEntry& entry) {
  Io::ReadArrayLE<int>(entry.WaveData.data(), &stream, entry.WaveData.size());
};

void SaveSystem::WriteBGMFlags(Io::MemoryStream& stream) {
  Io::WriteArrayLE<uint8_t>(BGMFlags.data(), &stream, 200);
}
void SaveSystem::ReadBGMFlags(Io::MemoryStream& stream) {
  Io::ReadArrayLE<uint8_t>(BGMFlags.data(), &stream, 200);
}

void SaveSystem::LoadScrWork() {
  ScrWork[SW_SVSENO] = ScrWork[SW_SEREQNO];
  ScrWork[SW_SVSENO + 1] = ScrWork[SW_SEREQNO + 1];
  ScrWork[SW_SVSENO + 2] = ScrWork[SW_SEREQNO + 2];
  ScrWork[SW_SVBGMNO] = ScrWork[SW_BGMREQNO];
  ScrWork[SW_SVBGM2NO] = ScrWork[SW_BGMREQNO2];
  ScrWork[SW_SVSCRNO1] = ScrWork[SW_SCRIPTNO2];
  ScrWork[SW_SVSCRNO2] = ScrWork[SW_SCRIPTNO3];
  ScrWork[SW_SVSCRNO3] = ScrWork[SW_SCRIPTNO4];
  ScrWork[SW_SVSCRNO4] = ScrWork[SW_SCRIPTNO5];
  for (int i = 0; i < 8; i++) {
    ScrWork[SW_SVBGNO1 + i] = ScrWork[SW_BG1NO + i * ScrWorkBgStructSize];
    ScrWork[SW_SVCHANO1 + i] = ScrWork[SW_CHA1NO + i * ScrWorkChaStructSize];
  }
}

}  // namespace CCLCC_PS4
}  // namespace Impacto