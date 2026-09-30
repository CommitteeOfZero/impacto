#include "savesystem_switch.h"

#include "../../io/stream.h"
#include "../../io/physicalfilestream.h"
#include "../../mem.h"
#include "../../vm/vm.h"
#include "../../profile/data/savesystem.h"
#include "../../data/savesystem.h"
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
#include <regex>

namespace Impacto {
namespace CCLCC_Switch {

using namespace Impacto::Vm;
using namespace Impacto::Effects;
using namespace Impacto::SaveSystem;
using namespace Impacto::Profile::SaveSystem;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::Vm;
using namespace Impacto::Profile::ConfigSystem;

static constexpr CCLCC::SaveLayout SwitchLayout{
    .ConfigOffset = 0x58C,
    .VoiceOffset = 0x59E,
    .SkipVoiceOffset = 0x5E1,
    .AdvanceTextOffset = 0x5E5,
    .QuickSortedIdOffset = 0x90E,
    .QuickSortedIdIsLoaded = false,
    .QuickSortedIdBackCompat = false,
    .EVFlagsOffset = 0x96E,
    .BgmFlagsOffset = 0xA0E,
    .MessageFlagsOffset = 0xA2E,
    .ExtraDataOffset = 0x20A2E,

    .SysFlagWorkDst1 = 200,
    .SysFlagWorkDst2 = 560,
    .SysScrWorkDst1 = 1800,
    .SysScrWorkLen1 = 200,

    .FlagWork1Len = 150,  // 150 bytes from &FlagWork[50]
    .FlagWork2Len = 100,  // 100 bytes from &FlagWork[400]
    .FlagWork2Src = 400,
    .ScrWork1Len = 800,   // (in ints) 3200 bytes from &ScrWork[1000]
    .ScrWork2Len = 3000,  // (in ints) 12000 bytes from &ScrWork[4300]
    .MainThreadOffset = 0x7CDC,
    .MainThreadBufIdOffset = 0x7D40,
    .WaveOffset = 0x7D88,
    .MapLoadOffset = 0x82C8,
    .ThumbnailPadding = 0xED0,
    .MapLoadLen = 0x6ac8,
    .YesNoLen = 0x68,
    .ThumbnailWidth = 240,
    .ThumbnailHeight = 135,
};

SaveError SaveSystem::CheckSaveFile() const {
  auto checkFile = [](std::string path, int size) {
    std::error_code ec;

    IoError existsState = Io::PathExists(path);
    if (existsState == IoError_NotFound) {
      return SaveError::NotFound;
    } else if (existsState == IoError_Fail) {
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Failed to check if save file exists, error: \"{:s}\"\n",
             ec.message());
      return SaveError::Failed;
    }

    auto saveFileSize = Io::GetFileSize(path);
    if (saveFileSize == IoError_Fail) {
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Failed to get save file size, error: \"{:s}\"\n", ec.message());
      return SaveError::Failed;
    } else if (saveFileSize != size) {
      return SaveError::Corrupted;
    }

    Io::FilePermissionsFlags perms;
    IoError permsState = Io::GetFilePermissions(path, perms);
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
  };

  auto error = checkFile(SaveFilePath, SaveFileSize);
  if (error != SaveError::OK) {
    return error;
  }

  error = checkFile(*SystemDataPath, SystemSaveSize);
  if (error != SaveError::OK) {
    return error;
  }

  return SaveError::OK;
}

static std::optional<SaveError> IoErrorToSaveError(IoError err) {
  switch (err) {
    case IoError_NotFound:
      return SaveError::NotFound;
    case IoError_Fail:
    case IoError_Eof:
      return SaveError::Corrupted;
    case IoError_OK:
      return std::nullopt;
  }
  return SaveError::Failed;
}

SaveError SaveSystem::MountSaveFile(std::vector<QueuedTexture>& textures) {
  namespace fs = std::filesystem;

  Io::Stream* systemDataStream;
  IoError err =
      Io::PhysicalFileStream::Create(*SystemDataPath, &systemDataStream);

  if (auto saveErr = IoErrorToSaveError(err)) return *saveErr;

  const RectF viewport = Window->GetViewport();
  const auto l = GetSaveLayout();
  WorkingSaveEntry = CCLCC::SaveFileEntry(l);
  WorkingSaveThumbnail.Sheet = SpriteSheet(viewport.Width, viewport.Height);
  WorkingSaveThumbnail.Bounds.SetSize(viewport.GetSize());

  QueuedTexture txt{
      .Id = std::ref(WorkingSaveThumbnail.Sheet.Texture),
  };
  txt.Tex.LoadSolidColor((int)WorkingSaveThumbnail.Bounds.Width,
                         (int)WorkingSaveThumbnail.Bounds.Height, 0x000000);
  textures.push_back(txt);

  Io::ReadArrayLE<uint8_t>(SystemData.data(), systemDataStream,
                           SystemData.size());
  delete systemDataStream;

  Io::Stream* saveDataStream;
  err = Io::PhysicalFileStream::Create(SaveFilePath, &saveDataStream);
  if (auto saveErr = IoErrorToSaveError(err)) return *saveErr;

  /*
  uint32_t systemSaveChecksum =
      CalculateChecksum(std::span(SystemData).subspan(4));
  */

  textures.reserve(MaxSaveEntries * 2);

  auto readEntry = [&](Io::Stream* stream, SaveFileEntryBase** entries, int i,
                       SaveType saveType) {
    std::array<uint8_t, SaveEntrySize> entrySlotBuf;
    Io::ReadArrayLE<uint8_t>(entrySlotBuf.data(), stream, entrySlotBuf.size());
    Io::MemoryStream saveEntryDataStream(entrySlotBuf.data(),
                                         entrySlotBuf.size(), false);

    QueuedTexture tex{
        .Id = std::ref(entries[i]->SaveThumbnail.Sheet.Texture),
    };
    LoadEntryBuffer(saveEntryDataStream,
                    static_cast<CCLCC::SaveFileEntry&>(*entries[i]), saveType,
                    tex.Tex);

    textures.push_back(tex);
  };

  // init and read full saves + init empty quick saves
  for (int i = 0; i < MaxSaveEntries; i++) {
    FullSaveEntries[i] = new CCLCC::SaveFileEntry(l);
    QuickSaveEntries[i] = new CCLCC::SaveFileEntry(l);

    readEntry(saveDataStream, FullSaveEntries, i, SaveType::Full);

    // Todo, validate checksum?
  }
  delete saveDataStream;

  // read quick saves
  fs::path qsavePath = *QuickDataPath;
  fs::path folder = qsavePath.parent_path();
  std::string stem = qsavePath.stem().string();      // MDATA_CC
  std::string ext = qsavePath.extension().string();  // .DAT

  std::regex pattern(stem + R"((\d{3}))" + "\\" + ext, std::regex::icase);
  std::vector<std::pair<uint8_t, fs::file_time_type>> idsWithTime;

  int lockedQuickSaveSlots = 0;
  for (const auto& entry : fs::directory_iterator(folder)) {
    if (!entry.is_regular_file()) continue;

    std::string filename = entry.path().filename().string();
    std::smatch match;

    if (!std::regex_match(filename, match, pattern) || match.size() < 1)
      continue;

    fs::file_time_type writeTime = fs::last_write_time(entry.path());

    Io::Stream* qsaveStream;
    err = Io::PhysicalFileStream::Create(entry.path().string(), &qsaveStream);
    if (auto saveErr = IoErrorToSaveError(err)) return *saveErr;

    int id = std::stoi(match[1].str());
    if (id < 0 || id >= MaxSaveEntries) {
      assert(false);
      continue;
    }
    idsWithTime.push_back({static_cast<uint8_t>(id), writeTime});

    readEntry(qsaveStream, QuickSaveEntries, id, SaveType::Quick);

    lockedQuickSaveSlots +=
        static_cast<CCLCC::SaveFileEntry&>(*QuickSaveEntries[id]).Flags &
        WriteProtect;

    delete qsaveStream;
  }

  std::sort(idsWithTime.begin(), idsWithTime.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });

  // fill QuickSaveRecentSortedId based on the file modification time, since
  // engine does not update System save file on quicksave file write
  std::array<bool, MaxSaveEntries> used{};
  size_t i = 0;
  for (const auto& [id, time] : idsWithTime) {
    QuickSaveRecentSortedId[i++] = id;
    used[id] = true;
  }

  // fill the rest of the ids
  if (idsWithTime.size() != MaxSaveEntries) {
    for (uint8_t j = 0; j < MaxSaveEntries && i < MaxSaveEntries; ++j) {
      if (!used[j]) QuickSaveRecentSortedId[i++] = j;
    }
  }
  SetLockedQuickSaveCount(lockedQuickSaveSlots);
  SetFlag(SF_SAVEALLPROTECTED, LockedQuickSaveCount == MaxSaveEntries);

  return SaveError::OK;
}

static void ChecksumAndWriteEntry(Io::Stream* destStream,
                                  std::span<uint8_t>& entrySlotBuf,
                                  Io::MemoryStream& saveEntryMemoryStream) {
  uint32_t entryCheckSum = CCLCC::SaveSystem::CalculateChecksum(
      std::span(entrySlotBuf).subspan(6, 64710 * 2), 18198, 5250, false);
  saveEntryMemoryStream.Seek(2, SEEK_SET);
  Io::WriteLE<uint16_t>(&saveEntryMemoryStream, entryCheckSum >> 16);
  Io::WriteLE<uint16_t>(&saveEntryMemoryStream, entryCheckSum & 0xFFFF);
  Io::WriteArrayLE<uint8_t>(entrySlotBuf.data(), destStream,
                            entrySlotBuf.size());
}

SaveError SaveSystem::WriteSaveFile() {
  using CF = Io::PhysicalFileStream::CreateFlagsMode;
  Io::Stream* stream;
  IoError err = Io::PhysicalFileStream::Create(
      *SystemDataPath, &stream, CF::CREATE | CF::CREATE_DIRS | CF::WRITE);
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
  delete stream;

  err = Io::PhysicalFileStream::Create(
      SaveFilePath, &stream, CF::CREATE | CF::CREATE_DIRS | CF::WRITE);
  if (err != IoError_OK) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Failed to open save file for writing\n");
    return SaveError::Failed;
  }

  stream->Seek(0, SEEK_SET);

  for (int i = 0; i < MaxSaveEntries; i++) {
    CCLCC::SaveFileEntry* entry = (CCLCC::SaveFileEntry*)FullSaveEntries[i];
    if (entry == nullptr || entry->Status == 0) {
      Io::WriteLE<uint8_t>(stream, 0, SaveEntrySize);
    } else {
      assert(stream->Position == static_cast<int>(SaveEntrySize) * i);
      std::vector<uint8_t> entrySlotBuf = std::vector<uint8_t>(SaveEntrySize);
      Io::MemoryStream saveEntryMemoryStream(entrySlotBuf.data(),
                                             entrySlotBuf.size(), false);
      SaveEntryBuffer(saveEntryMemoryStream, *entry, SaveType::Full);
      auto spanned = std::span(entrySlotBuf);
      ChecksumAndWriteEntry(stream, spanned, saveEntryMemoryStream);
    }
  }
  return SaveError::OK;
}

SaveError SaveSystem::WriteQuickSaveFile() {
  int id = QuickSaveRecentSortedId[0];
  auto entry = GetSaveEntry<CCLCC::SaveFileEntry>(SaveType::Quick, 0);

  namespace fs = std::filesystem;
  fs::path qsavePath = *QuickDataPath;
  std::string stem = qsavePath.stem().string();      // MDATA_CC
  std::string ext = qsavePath.extension().string();  // .DAT
  std::string filename = fmt::format("{}{:03d}{}", stem, id, ext);
  std::string targetPath = qsavePath.replace_filename(filename).string();

  using CF = Io::PhysicalFileStream::CreateFlagsMode;
  Io::Stream* stream;
  IoError err = Io::PhysicalFileStream::Create(
      targetPath, &stream,
      CF::CREATE | CF::CREATE_DIRS | CF::WRITE | CF::TRUNCATE);
  if (err != IoError_OK) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Failed to open save file for writing\n");
    return SaveError::Failed;
  }

  stream->Seek(0, SEEK_SET);

  std::vector<uint8_t> entrySlotBuf = std::vector<uint8_t>(SaveEntrySize);
  Io::MemoryStream saveEntryMemoryStream(entrySlotBuf.data(),
                                         entrySlotBuf.size(), false);
  SaveEntryBuffer(saveEntryMemoryStream, *entry, SaveType::Quick);
  auto spanned = std::span(entrySlotBuf);
  ChecksumAndWriteEntry(stream, spanned, saveEntryMemoryStream);
  return SaveError::OK;
}

CCLCC::SaveLayout SaveSystem::GetSaveLayout() const { return SwitchLayout; }

void SaveSystem::ReadWorkScriptData(Io::MemoryStream& stream,
                                    CCLCC::SaveFileEntry& entry) {
  Io::ReadArrayLE<uint8_t>(entry.FlagWorkScript2.data(), &stream,
                           entry.FlagWorkScript2.size());
  Io::ReadArrayLE<uint8_t>(entry.FlagWorkScript1.data(), &stream,
                           entry.FlagWorkScript1.size());
  Io::ReadLE<uint16_t>(&stream);
  assert(stream.Position == 380);
  Io::ReadArrayLE<int>(entry.ScrWorkScript2.data(), &stream,
                       entry.ScrWorkScript2.size());
  assert(stream.Position == 12380);
  Io::ReadArrayLE<int>(entry.ScrWorkScript1.data(), &stream,
                       entry.ScrWorkScript1.size());
  // StrWork goes here, skipping
  stream.Seek(0x4000, SEEK_CUR);
}

void SaveSystem::WriteWorkScriptData(Io::MemoryStream& stream,
                                     CCLCC::SaveFileEntry& entry) {
  Io::WriteArrayLE<uint8_t>(entry.FlagWorkScript2.data(), &stream,
                            entry.FlagWorkScript2.size());
  Io::WriteArrayLE<uint8_t>(entry.FlagWorkScript1.data(), &stream,
                            entry.FlagWorkScript1.size());
  Io::WriteLE<uint16_t>(&stream, 0);
  assert(stream.Position == 380);
  Io::WriteArrayLE<int>(entry.ScrWorkScript2.data(), &stream,
                        entry.ScrWorkScript2.size());
  assert(stream.Position == 12380);
  Io::WriteArrayLE<int>(entry.ScrWorkScript1.data(), &stream,
                        entry.ScrWorkScript1.size());
  // StrWork goes here, skipping
  stream.Seek(0x4000, SEEK_CUR);
}

void SaveSystem::WriteBGMFlags(Io::MemoryStream& stream) {
  for (size_t i = 0; i < 32; i++) {
    const uint8_t bgmByte = (static_cast<uint8_t>(BGMFlags[8 * i])) |
                            (static_cast<uint8_t>(BGMFlags[8 * i + 1]) << 1) |
                            (static_cast<uint8_t>(BGMFlags[8 * i + 2]) << 2) |
                            (static_cast<uint8_t>(BGMFlags[8 * i + 3]) << 3) |
                            (static_cast<uint8_t>(BGMFlags[8 * i + 4]) << 4) |
                            (static_cast<uint8_t>(BGMFlags[8 * i + 5]) << 5) |
                            (static_cast<uint8_t>(BGMFlags[8 * i + 6]) << 6) |
                            (static_cast<uint8_t>(BGMFlags[8 * i + 7]) << 7);
    Io::WriteLE<uint8_t>(&stream, bgmByte);
  }
}
void SaveSystem::ReadBGMFlags(Io::MemoryStream& stream) {
  for (int i = 0; i < 32; i++) {
    auto val = Io::ReadU8(&stream);
    BGMFlags[8 * i] = val & 1;
    BGMFlags[8 * i + 1] = (val & 2) != 0;
    BGMFlags[8 * i + 2] = (val & 4) != 0;
    BGMFlags[8 * i + 3] = (val & 8) != 0;
    BGMFlags[8 * i + 4] = (val & 0x10) != 0;
    BGMFlags[8 * i + 5] = (val & 0x20) != 0;
    BGMFlags[8 * i + 6] = (val & 0x40) != 0;
    BGMFlags[8 * i + 7] = val >> 7;
  }
}

void SaveSystem::WriteWaveData(Io::MemoryStream& stream,
                               CCLCC::SaveFileEntry& entry) {
  Io::WriteArrayLE<int>(entry.WaveData.data(), &stream, entry.WaveData.size());
  stream.Seek(132, SEEK_CUR);  // skip ripple data (part of the wave data, but
                               // not used in CCLCC)
};

void SaveSystem::ReadWaveData(Io::MemoryStream& stream,
                              CCLCC::SaveFileEntry& entry) {
  Io::ReadArrayLE<int>(entry.WaveData.data(), &stream, entry.WaveData.size());
  stream.Seek(132, SEEK_CUR);  // skip ripple data
};

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
  }
  for (int i = 0; i < 10; i++) {
    const auto charOffset = i * ScrWorkChaStructSize;

    ScrWork[SW_SVCHANO1 + i] = ScrWork[SW_CHA1NO + charOffset] +
                               (ScrWork[SW_CHA1FACE + charOffset] << 16);
  }

  for (int i = 0; i < 8; i++) {
    ScrWork[SW_PIC_ARCHIVENO1 + i] = ScrWork[SW_PIC_REQ_ARCHIVENO1 + 2 * i];
    ScrWork[SW_PIC_FILENO1 + i] = ScrWork[SW_PIC_REQ_FILENO1 + 2 * i];
  }
}

}  // namespace CCLCC_Switch
}  // namespace Impacto