#include "savesystem.h"

#include "../../io/stream.h"
#include "../../io/physicalfilestream.h"
#include "../../mem.h"
#include "../../vm/vm.h"
#include "../../profile/data/savesystem.h"
#include "../../profile/scriptvars.h"
#include "../../profile/vm.h"
#include "../../ui/mapsystem.h"
#include "../../renderer/renderer.h"
#include "../../profile/configsystem.h"

#include "yesnotrigger.h"

#include <cstdint>
#include <ctime>
#include <system_error>

namespace Impacto {
namespace CCLCC {

using namespace Impacto::Vm;
using namespace Impacto::Profile::SaveSystem;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::Vm;
using namespace Impacto::Profile::ConfigSystem;

uint32_t CalculateChecksum(std::span<const uint8_t> bufferData,
                           uint16_t initSum = 0, uint16_t initXor = 0,
                           bool swapSrcBytes = false) {
  // Initialize checksum variables

  uint32_t checksumSum = initSum;

  uint32_t checksumXor = initXor;

  for (int i = 0; i < bufferData.size() - 1; i += 2) {
    uint16_t dataShort;
    memcpy(&dataShort, &bufferData[i], 2);
    if (swapSrcBytes) {
      dataShort = SDL_Swap16(dataShort);
    }
    checksumSum = checksumSum + dataShort;
    checksumXor = checksumXor ^ dataShort;
  }

  uint32_t result = (checksumSum << 16) | checksumXor & 0xFFFF;
  return result;
}

SaveError SaveSystem::CheckSaveFile() {
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
  auto checkPermsBit = [](Io::FilePermissionsFlags perms,
                          Io::FilePermissionsFlags flag) {
    return to_underlying(perms) & to_underlying(flag);
  };

  Io::FilePermissionsFlags perms;
  IoError permsState = Io::GetFilePermissions(SaveFilePath, perms);
  if (permsState == IoError_Fail) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Failed to get save file permissions, error: \"{:s}\"\n",
           ec.message());
    return SaveError::Failed;
  } else if ((!checkPermsBit(perms, Io::FilePermissionsFlags::owner_read) ||
              !checkPermsBit(perms, Io::FilePermissionsFlags::owner_write))) {
    return SaveError::WrongUser;
  }
  return SaveError::OK;
}

SaveError SaveSystem::CreateSaveFile() {
  using CF = Io::PhysicalFileStream::CreateFlagsMode;
  Io::Stream* stream;
  IoError err = Io::PhysicalFileStream::Create(
      SaveFilePath, &stream,
      CF::CREATE_IF_NOT_EXISTS | CF::CREATE_DIRS | CF::WRITE);
  if (err != IoError_OK) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Failed to open save file for writing\n");
    return SaveError::Failed;
  }

  assert(stream->Meta.Size == 0);
  std::vector<uint8_t> emptyData(SaveFileSize, 0);
  Io::WriteArrayBE<uint8_t>(emptyData.data(), stream, SaveFileSize);
  assert(stream->Position == SaveFileSize);

  stream->Seek(0x8AC, SEEK_SET);
  Io::WriteLE(stream, (Uint16)(Default::TextSpeed * 60));
  Io::WriteLE(stream, (Uint16)(Default::AutoSpeed * 60));
  Io::WriteLE(stream, (Uint8)(Default::GroupVolumes[Audio::ACG_Voice] *
                              128));  // VOICE2vol
  Io::WriteLE(stream, (Uint8)(Default::GroupVolumes[Audio::ACG_Voice] *
                              128));  // VOICEvol
  Io::WriteLE(stream, (Uint8)(Default::GroupVolumes[Audio::ACG_BGM] * 256));
  Io::WriteLE(stream,
              (Uint8)(Default::GroupVolumes[Audio::ACG_SE] * 128));  // SEvol
  Io::WriteLE(
      stream,
      (Uint8)(Default::GroupVolumes[Audio::ACG_SE] * 0.6 * 128));  // SYSSEvol
  Io::WriteLE(stream, (Uint8)(Default::GroupVolumes[Audio::ACG_Movie] * 128));
  Io::WriteLE(stream, Default::SyncVoice);
  Io::WriteLE(stream, !Default::SkipRead);

  stream->Seek(0x8BE, SEEK_SET);
  for (size_t i = 0; i < 33; i++) Io::WriteLE(stream, !Default::VoiceMuted[i]);
  for (size_t i = 0; i < 33; i++)
    Io::WriteLE(stream, (Uint8)(Default::VoiceVolume[i] * 128));

  stream->Seek(0x901, SEEK_SET);
  Io::WriteLE(stream, Default::SkipVoice);
  Io::WriteLE(stream, Default::ShowTipsNotification);

  stream->Seek(0x905, SEEK_SET);
  Io::WriteLE(stream, Default::AdvanceTextOnDirectionalInput);
  Io::WriteLE(stream, Default::DirectionalInputForTrigger);
  Io::WriteLE(stream, Default::TriggerStopSkip);

  delete stream;

  return SaveError::OK;
}

void SaveSystem::LoadEntryBuffer(Io::MemoryStream& stream, SaveFileEntry& entry,
                                 SaveType saveType, Texture& tex) {
  entry.Status = Io::ReadLE<uint8_t>(&stream);
  Io::ReadLE<uint8_t>(&stream);
  if (entry.Status == 1 && saveType == SaveType::Quick) {
    QuickSaveCount++;
  }
  uint16_t checksumSum = Io::ReadLE<uint16_t>(&stream);
  uint16_t checksumXor = Io::ReadLE<uint16_t>(&stream);

  entry.Checksum = checksumSum << 16 | checksumXor;
  Io::ReadLE<uint16_t>(&stream);
  uint16_t saveYear = Io::ReadLE<uint16_t>(&stream);
  uint8_t saveDay = Io::ReadLE<uint8_t>(&stream);
  uint8_t saveMonth = Io::ReadLE<uint8_t>(&stream);
  uint8_t saveSecond = Io::ReadLE<uint8_t>(&stream);
  uint8_t saveMinute = Io::ReadLE<uint8_t>(&stream);
  uint8_t saveHour = Io::ReadLE<uint8_t>(&stream);
  std::tm t{};
  t.tm_sec = saveSecond;
  t.tm_min = saveMinute;
  t.tm_hour = saveHour;
  t.tm_mday = saveDay;
  t.tm_mon = saveMonth - 1;
  t.tm_year = saveYear - 1900;
  entry.SaveDate = t;

  Io::ReadLE<uint8_t>(&stream);
  entry.PlayTime = Io::ReadLE<uint32_t>(&stream);
  entry.SwTitle = Io::ReadLE<uint32_t>(&stream);
  Io::ReadLE<uint32_t>(&stream);
  entry.Flags = Io::ReadLE<uint8_t>(&stream);
  stream.Seek(7, SEEK_CUR);
  entry.SaveType = Io::ReadLE<uint32_t>(&stream);
  assert(stream.Position == 0x28);
  stream.Seek(0x58, SEEK_CUR);
  Io::ReadArrayLE<uint8_t>(entry.FlagWorkScript1, &stream, 50);
  assert(stream.Position == 178);
  Io::ReadArrayLE<uint8_t>(entry.FlagWorkScript2, &stream, 100);
  Io::ReadLE<uint16_t>(&stream);
  assert(stream.Position == 280);
  Io::ReadArrayLE<int>(entry.ScrWorkScript1, &stream, 600);
  assert(stream.Position == 2680);
  Io::ReadArrayLE<int>(entry.ScrWorkScript2, &stream, 3000);

  assert(stream.Position == 0x3958);
  entry.MainThreadExecPriority = Io::ReadLE<uint32_t>(&stream);
  entry.MainThreadGroupId = Io::ReadLE<uint32_t>(&stream);
  entry.MainThreadWaitCounter = Io::ReadLE<uint32_t>(&stream);
  entry.MainThreadScriptParam = Io::ReadLE<uint32_t>(&stream);
  entry.MainThreadIp = Io::ReadLE<uint32_t>(&stream);
  entry.MainThreadLoopCounter = Io::ReadLE<uint32_t>(&stream);
  entry.MainThreadLoopAdr = Io::ReadLE<uint32_t>(&stream);
  entry.MainThreadCallStackDepth = Io::ReadLE<uint32_t>(&stream);
  for (int j = 0; j < 8; j++) {
    entry.MainThreadReturnIds[j] = Io::ReadLE<uint32_t>(&stream);
  }
  for (int j = 0; j < 8; j++) {
    entry.MainThreadReturnBufIds[j] = Io::ReadLE<uint32_t>(&stream);
  }
  Io::ReadLE<uint32_t>(&stream);
  assert(stream.Position == 0x39bc);
  entry.MainThreadScriptBufferId = Io::ReadLE<uint32_t>(&stream);
  Io::ReadArrayBE<int>(entry.MainThreadVariables, &stream, 16);
  entry.MainThreadDialoguePageId = Io::ReadLE<uint32_t>(&stream);
  assert(stream.Position == 0x3a04);
  stream.Seek(1212, SEEK_CUR);
  assert(stream.Position == 0x3ec0);
  Io::ReadArrayLE<uint8_t>(entry.MapLoadData, &stream, 0x6ac8);
  Io::ReadArrayLE<uint8_t>(entry.YesNoData, &stream, 0x54);

  Sprite& thumbnail = entry.SaveThumbnail;
  thumbnail.Sheet = SpriteSheet(SaveThumbnailWidth, SaveThumbnailHeight);
  thumbnail.Bounds = RectF(0.0f, 0.0f, SaveThumbnailWidth, SaveThumbnailHeight);

  tex.Init(TexFmt_RGB, SaveThumbnailWidth, SaveThumbnailHeight);

  int thumbnailPadding = 0xA14;
  stream.Seek(thumbnailPadding, SEEK_CUR);
  Io::ReadArrayLE<uint8_t>(entry.ThumbnailData.data(), &stream,
                           entry.ThumbnailData.size());

  for (int i = 0; i < entry.ThumbnailData.size() / 2; i++) {
    uint16_t pixel =
        entry.ThumbnailData[i * 2] | (entry.ThumbnailData[i * 2 + 1] << 8);
    uint8_t r = (pixel & 0xF800) >> 8;
    uint8_t g = (uint8_t)((pixel & 0x07E0) >> 3);
    uint8_t b = (pixel & 0x001F) << 3;
    tex.Buffer[3 * i] = r;
    tex.Buffer[3 * i + 1] = g;
    tex.Buffer[3 * i + 2] = b;
  }
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
  WorkingSaveEntry = std::optional<SaveFileEntry>(SaveFileEntry());
  WorkingSaveThumbnail.Sheet =
      SpriteSheet((float)Window->WindowWidth, (float)Window->WindowHeight);
  WorkingSaveThumbnail.Bounds = RectF(0.0f, 0.0f, (float)Window->WindowWidth,
                                      (float)Window->WindowHeight);

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

  textures.reserve(MaxSaveEntries * 2);
  for (auto& entryArray : {FullSaveEntries, QuickSaveEntries}) {
    SaveType saveType =
        (entryArray == QuickSaveEntries) ? SaveType::Quick : SaveType::Full;
    int64_t saveDataPos = stream->Position;
    for (int i = 0; i < MaxSaveEntries; i++) {
      assert(stream->Position - saveDataPos == 0x1b110 * i);
      entryArray[i] = new SaveFileEntry();

      std::array<uint8_t, 0x1b110> entrySlotBuf;
      Io::ReadArrayLE<uint8_t>(entrySlotBuf.data(), stream,
                               entrySlotBuf.size());
      Io::MemoryStream saveEntryDataStream(entrySlotBuf.data(),
                                           entrySlotBuf.size(), false);

      QueuedTexture tex{
          .Id = std::ref(entryArray[i]->SaveThumbnail.Sheet.Texture),
      };
      LoadEntryBuffer(saveEntryDataStream,
                      static_cast<SaveFileEntry&>(*entryArray[i]), saveType,
                      tex.Tex);
      textures.push_back(tex);

      // Todo, validate checksum?
    }
  }
  delete stream;
  return SaveError::OK;
}

void SaveSystem::FlushWorkingSaveEntry(SaveType type, int id,
                                       int autoSaveType) {
  SaveFileEntry* entry = nullptr;
  switch (type) {
    case SaveType::Quick:
      entry = (SaveFileEntry*)QuickSaveEntries[id];
      break;
    case SaveType::Full:
      entry = (SaveFileEntry*)FullSaveEntries[id];
      break;
  }

  if (entry != nullptr && !(GetSaveFlags(type, id) & WriteProtect)) {
    Renderer->FreeTexture(entry->SaveThumbnail.Sheet.Texture);
    *entry = *WorkingSaveEntry;
    if (type == SaveType::Quick) {
      entry->SaveType = autoSaveType;
    }
    time_t rawtime;
    time(&rawtime);
    entry->SaveDate = CurrentDateTime();
    auto captureBuffer =
        Renderer->GetSpriteSheetImage(WorkingSaveThumbnail.Sheet);

    Texture tex;
    tex.Init(TexFmt_RGBA, SaveThumbnailWidth, SaveThumbnailHeight);

    entry->SaveThumbnail.Sheet =
        SpriteSheet(SaveThumbnailWidth, SaveThumbnailHeight);
    entry->SaveThumbnail.Bounds =
        RectF(0.0f, 0.0f, SaveThumbnailWidth, SaveThumbnailHeight);

    int result = ResizeImage(
        WorkingSaveThumbnail.Bounds, entry->SaveThumbnail.Bounds, captureBuffer,
        std::span{tex.Buffer, static_cast<size_t>(tex.BufferSize)});
    if (result < 0) {
      ImpLog(LogLevel::Error, LogChannel::General,
             "Failed to resize save thumbnail\n");
    }
    entry->SaveThumbnail.Sheet.Texture = tex.Submit();
  }
}

void SaveSystem::SaveEntryBuffer(Io::MemoryStream& memoryStream,
                                 SaveFileEntry& entry, SaveType saveType) {
  Io::WriteLE<uint16_t>(&memoryStream, entry.Status);
  Io::WriteLE<uint32_t>(&memoryStream, entry.Checksum);
  Io::WriteLE<uint16_t>(&memoryStream, 0);

  Io::WriteLE<uint16_t>(&memoryStream,
                        (uint16_t)(entry.SaveDate.tm_year + 1900));
  Io::WriteLE<uint8_t>(&memoryStream, (uint8_t)entry.SaveDate.tm_mday);
  Io::WriteLE<uint8_t>(&memoryStream, (uint8_t)(entry.SaveDate.tm_mon + 1));
  Io::WriteLE<uint8_t>(&memoryStream, (uint8_t)entry.SaveDate.tm_sec);
  Io::WriteLE<uint8_t>(&memoryStream, (uint8_t)entry.SaveDate.tm_min);
  Io::WriteLE<uint8_t>(&memoryStream, (uint8_t)entry.SaveDate.tm_hour);
  Io::WriteLE<uint8_t>(&memoryStream, 0);

  Io::WriteLE<uint32_t>(&memoryStream, entry.PlayTime);
  Io::WriteLE<uint32_t>(&memoryStream, entry.SwTitle);
  Io::WriteLE<uint32_t>(&memoryStream, 0);
  Io::WriteLE<uint8_t>(&memoryStream, entry.Flags);
  memoryStream.Seek(7, SEEK_CUR);
  Io::WriteLE<uint32_t>(&memoryStream, entry.SaveType);
  assert(memoryStream.Position == 0x28);
  memoryStream.Seek(0x58, SEEK_CUR);
  Io::WriteArrayLE<uint8_t>(entry.FlagWorkScript1, &memoryStream, 50);
  assert(memoryStream.Position == 178);
  Io::WriteArrayLE<uint8_t>(entry.FlagWorkScript2, &memoryStream, 100);
  Io::WriteLE<uint16_t>(&memoryStream, 0);
  assert(memoryStream.Position == 280);
  Io::WriteArrayLE<int>(entry.ScrWorkScript1, &memoryStream, 600);
  assert(memoryStream.Position == 2680);
  Io::WriteArrayLE<int>(entry.ScrWorkScript2, &memoryStream, 3000);

  assert(memoryStream.Position == 0x3958);
  Io::WriteLE<uint32_t>(&memoryStream, entry.MainThreadExecPriority);
  Io::WriteLE<uint32_t>(&memoryStream, entry.MainThreadGroupId);
  Io::WriteLE<uint32_t>(&memoryStream, entry.MainThreadWaitCounter);
  Io::WriteLE<uint32_t>(&memoryStream, entry.MainThreadScriptParam);
  Io::WriteLE<uint32_t>(&memoryStream, entry.MainThreadIp);
  Io::WriteLE<uint32_t>(&memoryStream, entry.MainThreadLoopCounter);
  Io::WriteLE<uint32_t>(&memoryStream, entry.MainThreadLoopAdr);
  Io::WriteLE<uint32_t>(&memoryStream, entry.MainThreadCallStackDepth);
  for (int j = 0; j < 8; j++) {
    Io::WriteLE<uint32_t>(&memoryStream, entry.MainThreadReturnIds[j]);
  }
  for (int j = 0; j < 8; j++) {
    Io::WriteLE<uint32_t>(&memoryStream, entry.MainThreadReturnBufIds[j]);
  }
  Io::WriteLE<uint32_t>(&memoryStream, 0);
  assert(memoryStream.Position == 0x39bc);
  Io::WriteLE<uint32_t>(&memoryStream, entry.MainThreadScriptBufferId);
  Io::WriteArrayBE<int>(entry.MainThreadVariables, &memoryStream, 16);
  Io::WriteLE<uint32_t>(&memoryStream, entry.MainThreadDialoguePageId);
  assert(memoryStream.Position == 0x3a04);
  memoryStream.Seek(1212, SEEK_CUR);
  assert(memoryStream.Position == 0x3ec0);
  Io::WriteArrayLE<uint8_t>(entry.MapLoadData, &memoryStream, 0x6ac8);
  Io::WriteArrayLE<uint8_t>(entry.YesNoData, &memoryStream, 0x54);

  int thumbnailPadding = 0xA14;
  memoryStream.Seek(thumbnailPadding, SEEK_CUR);

  Io::WriteArrayLE<uint8_t>(entry.ThumbnailData.data(), &memoryStream,
                            entry.ThumbnailData.size());
}

void SaveSystem::SaveThumbnailData() {
  std::vector<uint8_t> thumbnailBuffer(SaveThumbnailSize * 2);

  for (auto* entryArray : {FullSaveEntries, QuickSaveEntries}) {
    for (int i = 0; i < MaxSaveEntries; i++) {
      SaveFileEntry* entry = (SaveFileEntry*)entryArray[i];
      if (entry->Status == 0) continue;

      Renderer->GetSpriteSheetImage(entry->SaveThumbnail.Sheet,
                                    thumbnailBuffer);

      std::array<uint8_t, SaveThumbnailSize>& thumbnailData =
          entry->ThumbnailData;

      for (int j = 0; j < thumbnailBuffer.size(); j += 4) {
        uint8_t r = thumbnailBuffer[j];
        uint8_t g = thumbnailBuffer[j + 1];
        uint8_t b = thumbnailBuffer[j + 2];
        uint16_t pixel = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        pixel = SDL_SwapBE16(pixel);
        thumbnailData[j / 2] = pixel >> 8;
        thumbnailData[j / 2 + 1] = pixel & 0xFF;
      }
    }
  }
}

SaveError SaveSystem::LoadSystemData() {
  Io::MemoryStream stream =
      Io::MemoryStream(SystemData.data(), SystemData.size(), false);

  /*
  uint16_t systemSum = Io::ReadLE<uint16_t>(&stream);
  uint16_t systemXor = Io::ReadLE<uint16_t>(&stream);
  */
  stream.Seek(0x14, SEEK_SET);

  stream.Seek(0x80, SEEK_SET);
  Io::ReadArrayLE<uint8_t>(&FlagWork[100], &stream, 50);
  Io::ReadArrayLE<uint8_t>(&FlagWork[460], &stream, 40);
  stream.Seek(0xDC, SEEK_SET);
  Io::ReadArrayLE<int>(&ScrWork[1600], &stream, 400);
  Io::ReadArrayLE<int>(&ScrWork[2000], &stream, 100);

  // Config settings
  stream.Seek(0x8AC, SEEK_SET);
  TextSpeed = Io::ReadLE<Uint16>(&stream) / 60.0f;
  AutoSpeed = Io::ReadLE<Uint16>(&stream) / 60.0f;
  stream.Seek(1, SEEK_CUR);  // VOICE2vol
  Audio::GroupVolumes[Audio::ACG_Voice] = Io::ReadLE<Uint8>(&stream) / 128.0f;
  Audio::GroupVolumes[Audio::ACG_BGM] = Io::ReadLE<Uint8>(&stream) / 256.0f;
  Audio::GroupVolumes[Audio::ACG_SE] = Io::ReadLE<Uint8>(&stream) / 128.0f;
  stream.Seek(1, SEEK_CUR);  // SYSSEvol
  Audio::GroupVolumes[Audio::ACG_Movie] = Io::ReadLE<Uint8>(&stream) / 128.0f;
  SyncVoice = Io::ReadLE<bool>(&stream);
  SkipRead = !Io::ReadLE<bool>(&stream);

  stream.Seek(0x8BE, SEEK_SET);
  for (size_t i = 0; i < 33; i++) VoiceMuted[i] = !Io::ReadLE<bool>(&stream);
  for (size_t i = 0; i < 33; i++)
    VoiceVolume[i] = Io::ReadLE<Uint8>(&stream) / 128.0f;

  stream.Seek(0x901, SEEK_SET);
  SkipVoice = Io::ReadLE<bool>(&stream);
  ShowTipsNotification = Io::ReadLE<bool>(&stream);

  stream.Seek(0x905, SEEK_SET);
  AdvanceTextOnDirectionalInput = Io::ReadLE<bool>(&stream);
  DirectionalInputForTrigger = Io::ReadLE<bool>(&stream);
  TriggerStopSkip = Io::ReadLE<bool>(&stream);

  // EV Flags
  stream.Seek(0xC0E, SEEK_SET);
  for (int i = 0; i < 150; i++) {
    auto val = Io::ReadU8(&stream);
    EVFlags[8 * i] = val & 1;
    EVFlags[8 * i + 1] = (val & 2) != 0;
    EVFlags[8 * i + 2] = (val & 4) != 0;
    EVFlags[8 * i + 3] = (val & 8) != 0;
    EVFlags[8 * i + 4] = (val & 0x10) != 0;
    EVFlags[8 * i + 5] = (val & 0x20) != 0;
    EVFlags[8 * i + 6] = (val & 0x40) != 0;
    EVFlags[8 * i + 7] = val >> 7;
  }

  stream.Seek(0xCA4, SEEK_SET);
  Io::ReadArrayLE<uint8_t>(BGMFlags, &stream, 200);

  stream.Seek(0xd6c, SEEK_SET);
  Io::ReadArrayLE<uint8_t>(MessageFlags, &stream, 10000);

  // EPnewList goes here

  stream.Seek(0x347c, SEEK_SET);
  Io::ReadArrayLE<uint8_t>(GameExtraData, &stream, 1024);

  return SaveError::OK;
}

void SaveSystem::SaveSystemData() {
  Io::MemoryStream systemSaveStream =
      Io::MemoryStream(SystemData.data(), SystemData.size(), false);

  systemSaveStream.Seek(0x14, SEEK_SET);

  systemSaveStream.Seek(0x80, SEEK_SET);
  Io::WriteArrayLE<uint8_t>(&FlagWork[100], &systemSaveStream, 50);
  Io::WriteArrayLE<uint8_t>(&FlagWork[460], &systemSaveStream, 40);
  systemSaveStream.Seek(0xDC, SEEK_SET);
  Io::WriteArrayLE<int>(&ScrWork[1600], &systemSaveStream, 400);
  Io::WriteArrayLE<int>(&ScrWork[2000], &systemSaveStream, 100);

  // Config settings
  systemSaveStream.Seek(0x8AC, SEEK_SET);
  Io::WriteLE(&systemSaveStream, (Uint16)(TextSpeed * 60));
  Io::WriteLE(&systemSaveStream, (Uint16)(AutoSpeed * 60));
  Io::WriteLE(
      &systemSaveStream,
      (Uint8)(Audio::GroupVolumes[Audio::ACG_Voice] * 128));  // VOICE2vol
  Io::WriteLE(
      &systemSaveStream,
      (Uint8)(Audio::GroupVolumes[Audio::ACG_Voice] * 128));  // VOICEvol
  Io::WriteLE(&systemSaveStream,
              (Uint8)(Audio::GroupVolumes[Audio::ACG_BGM] * 256));
  Io::WriteLE(&systemSaveStream,
              (Uint8)(Audio::GroupVolumes[Audio::ACG_SE] * 128));  // SEvol
  Io::WriteLE(
      &systemSaveStream,
      (Uint8)(Audio::GroupVolumes[Audio::ACG_SE] * 0.6 * 128));  // SYSSEvol
  Io::WriteLE(&systemSaveStream,
              (Uint8)(Audio::GroupVolumes[Audio::ACG_Movie] * 128));
  Io::WriteLE(&systemSaveStream, SyncVoice);
  Io::WriteLE(&systemSaveStream, !SkipRead);

  systemSaveStream.Seek(0x8BE, SEEK_SET);
  for (size_t i = 0; i < 33; i++)
    Io::WriteLE(&systemSaveStream, !VoiceMuted[i]);
  for (size_t i = 0; i < 33; i++)
    Io::WriteLE(&systemSaveStream, (Uint8)(VoiceVolume[i] * 128));

  systemSaveStream.Seek(0x901, SEEK_SET);
  Io::WriteLE(&systemSaveStream, SkipVoice);
  Io::WriteLE(&systemSaveStream, ShowTipsNotification);

  systemSaveStream.Seek(0x905, SEEK_SET);
  Io::WriteLE(&systemSaveStream, AdvanceTextOnDirectionalInput);
  Io::WriteLE(&systemSaveStream, DirectionalInputForTrigger);
  Io::WriteLE(&systemSaveStream, TriggerStopSkip);

  // EV Flags
  systemSaveStream.Seek(0xC0E, SEEK_SET);
  for (int i = 0; i < 150; i++) {
    const uint8_t evByte = (static_cast<uint8_t>(EVFlags[8 * i])) |
                           (static_cast<uint8_t>(EVFlags[8 * i + 1]) << 1) |
                           (static_cast<uint8_t>(EVFlags[8 * i + 2]) << 2) |
                           (static_cast<uint8_t>(EVFlags[8 * i + 3]) << 3) |
                           (static_cast<uint8_t>(EVFlags[8 * i + 4]) << 4) |
                           (static_cast<uint8_t>(EVFlags[8 * i + 5]) << 5) |
                           (static_cast<uint8_t>(EVFlags[8 * i + 6]) << 6) |
                           (static_cast<uint8_t>(EVFlags[8 * i + 7]) << 7);
    Io::WriteLE<uint8_t>(&systemSaveStream, evByte);
  }

  systemSaveStream.Seek(0xCA4, SEEK_SET);
  Io::WriteArrayLE<uint8_t>(BGMFlags, &systemSaveStream, 200);

  systemSaveStream.Seek(0xd6c, SEEK_SET);
  Io::WriteArrayLE<uint8_t>(MessageFlags, &systemSaveStream, 10000);

  // EPnewList goes here

  systemSaveStream.Seek(0x347c, SEEK_SET);
  Io::WriteArrayLE<uint8_t>(GameExtraData, &systemSaveStream, 1024);
}

SaveError SaveSystem::WriteSaveFile() {
  using CF = Io::PhysicalFileStream::CreateFlagsMode;
  Io::Stream* stream;
  IoError err = Io::PhysicalFileStream::Create(SaveFilePath, &stream,
                                               CF::WRITE | CF::READ);
  if (err != IoError_OK) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Failed to open save file for writing\n");
    return SaveError::Failed;
  }

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
    int64_t saveDataPos = stream->Position;
    for (int i = 0; i < MaxSaveEntries; i++) {
      SaveFileEntry* entry = (SaveFileEntry*)entryArray[i];
      if (entry->Status == 0) {
        stream->Seek(0x1b110, SEEK_CUR);
      } else {
        assert(stream->Position - saveDataPos == 0x1b110 * i);
        std::array<uint8_t, 0x1b110> entrySlotBuf{};
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

uint32_t SaveSystem::GetSavePlayTime(SaveType type, int id) {
  switch (type) {
    case SaveType::Full:
      return ((SaveFileEntry*)FullSaveEntries[id])->PlayTime;
    case SaveType::Quick:
      return ((SaveFileEntry*)QuickSaveEntries[id])->PlayTime;
    default:
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Failed to get save play time: unknown save type, returning 0\n");
      return 0;
  }
}

uint8_t SaveSystem::GetSaveFlags(SaveType type, int id) {
  switch (type) {
    case SaveType::Full:
      return ((SaveFileEntry*)FullSaveEntries[id])->Flags;
    case SaveType::Quick:
      return ((SaveFileEntry*)QuickSaveEntries[id])->Flags;
    default:
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Failed to get save flags: unknown save type, returning 0\n");
      return 0;
  }
}

tm const& SaveSystem::GetSaveDate(SaveType type, int id) {
  static const tm t{
      .tm_sec = 0,
      .tm_min = 0,
      .tm_hour = 0,
      .tm_mday = 1,
      .tm_mon = 0,
      .tm_year = 0,
  };
  switch (type) {
    case SaveType::Full:
      return ((SaveFileEntry*)FullSaveEntries[id])->SaveDate;
    case SaveType::Quick:
      return ((SaveFileEntry*)QuickSaveEntries[id])->SaveDate;
    default:
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Failed to read save date: Unknown save type, returning empty "
             "time\n");
      return t;
  }
}

void SaveSystem::SaveMemory() {
  // TODO: Sys save data

  if (WorkingSaveEntry) {
    WorkingSaveEntry->Status = 1;
    time_t rawtime;
    time(&rawtime);
    const tm timeinfo = CurrentDateTime();
    WorkingSaveEntry->SaveDate = timeinfo;
    WorkingSaveEntry->PlayTime = ScrWork[SW_PLAYTIME];
    WorkingSaveEntry->SwTitle = ScrWork[SW_TITLE];

    memcpy(WorkingSaveEntry->FlagWorkScript1, &FlagWork[50], 50);
    memcpy(WorkingSaveEntry->FlagWorkScript2, &FlagWork[300], 100);
    memcpy(WorkingSaveEntry->ScrWorkScript1, &ScrWork[1000], 2400);
    memcpy(WorkingSaveEntry->ScrWorkScript2, &ScrWork[4300], 12000);
    int threadId = ScrWork[SW_MAINTHDP];
    Sc3VmThread* thd = &ThreadPool[threadId & 0x7FFFFFFF];
    if (thd->GroupId - 5 < 3) {
      WorkingSaveEntry->MainThreadExecPriority = thd->ExecPriority;
      WorkingSaveEntry->MainThreadWaitCounter = thd->WaitCounter;
      WorkingSaveEntry->MainThreadScriptParam = thd->ScriptParam;
      WorkingSaveEntry->MainThreadGroupId = thd->GroupId;
      WorkingSaveEntry->MainThreadScriptBufferId = thd->ScriptBufferId;
      // Checkpoint id should already be set by SetCheckpointId
      WorkingSaveEntry->MainThreadCallStackDepth = thd->CallStackDepth;
      for (size_t i = 0; i < thd->CallStackDepth; i++) {
        WorkingSaveEntry->MainThreadReturnBufIds[i] =
            thd->ReturnScriptBufferIds[i];
        WorkingSaveEntry->MainThreadReturnIds[i] = thd->ReturnIds[i];
      }
      memcpy(WorkingSaveEntry->MainThreadVariables, thd->Variables, 64);
      WorkingSaveEntry->MainThreadDialoguePageId = thd->DialoguePageId;
    }
    UI::MapSystem::MapSave(WorkingSaveEntry->MapLoadData);
    CCLCC::YesNoTrigger::YesNoTriggerPtr->Save(WorkingSaveEntry->YesNoData);
  }
}

void SaveSystem::LoadEntry(SaveType type, int id) {
  if (!WorkingSaveEntry) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Failed to load save memory: no working save\n");
    return;
  }
  switch (type) {
    case SaveType::Quick:
      WorkingSaveEntry = *static_cast<SaveFileEntry*>(QuickSaveEntries[id]);
      break;
    case SaveType::Full:
      WorkingSaveEntry = *static_cast<SaveFileEntry*>(FullSaveEntries[id]);
      break;
    default:
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Failed to load save memory: unknown save type, doing nothing\n");
      return;
  }
}

void SaveSystem::LoadMemoryNew(LoadProcess load) {
  if (!WorkingSaveEntry || WorkingSaveEntry->Status == 0) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Failed to load entry: save is empty\n");
    return;
  }
  if (load == LoadProcess::Vars) {
    ScrWork[SW_PLAYTIME] = WorkingSaveEntry->PlayTime;
    ScrWork[SW_TITLE] = WorkingSaveEntry->SwTitle;
    ScrWork[SW_AUTOSAVERESTART] = WorkingSaveEntry->SaveType;

    memcpy(&FlagWork[50], WorkingSaveEntry->FlagWorkScript1, 50);
    memcpy(&FlagWork[300], WorkingSaveEntry->FlagWorkScript2, 100);
    memcpy(&ScrWork[1000], WorkingSaveEntry->ScrWorkScript1, 2400);
    memcpy(&ScrWork[4300], WorkingSaveEntry->ScrWorkScript2, 12000);
    UI::MapSystem::MapLoad(WorkingSaveEntry->MapLoadData);
    CCLCC::YesNoTrigger::YesNoTriggerPtr->Load(WorkingSaveEntry->YesNoData);
    // TODO: What to do about this mess I wonder...
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
  } else if (ScrWork[SW_MAINTHDP] != 0) {
    int threadId = ScrWork[SW_MAINTHDP];
    Sc3VmThread* thd = &ThreadPool[threadId & 0x7FFFFFFF];

    if (thd->GroupId - 5 < 3) {
      thd->ExecPriority = WorkingSaveEntry->MainThreadExecPriority;
      thd->WaitCounter = WorkingSaveEntry->MainThreadWaitCounter;
      thd->ScriptParam = WorkingSaveEntry->MainThreadScriptParam;
      thd->GroupId = WorkingSaveEntry->MainThreadGroupId;
      thd->ScriptBufferId = WorkingSaveEntry->MainThreadScriptBufferId;
      thd->IpOffset =
          ScriptGetRetAddress(WorkingSaveEntry->MainThreadScriptBufferId,
                              WorkingSaveEntry->MainThreadIp);
      thd->CallStackDepth = WorkingSaveEntry->MainThreadCallStackDepth;

      for (size_t i = 0; i < thd->CallStackDepth; i++) {
        thd->ReturnScriptBufferIds[i] =
            WorkingSaveEntry->MainThreadReturnBufIds[i];
        thd->ReturnIds[i] = (uint16_t)WorkingSaveEntry->MainThreadReturnIds[i];
      }

      memcpy(thd->Variables, WorkingSaveEntry->MainThreadVariables, 64);
      thd->DialoguePageId = WorkingSaveEntry->MainThreadDialoguePageId;
    }
  }
}

uint8_t SaveSystem::GetSaveStatus(SaveType type, int id) {
  switch (type) {
    case SaveType::Quick:
      return QuickSaveEntries[id] != nullptr
                 ? ((SaveFileEntry*)QuickSaveEntries[id])->Status
                 : 0;
    case SaveType::Full:
      return FullSaveEntries[id] != nullptr
                 ? ((SaveFileEntry*)FullSaveEntries[id])->Status
                 : 0;
    default:
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Failed to get save status: unknown save type, returning 0\n");
      return 0;
  }
}

int SaveSystem::GetSaveTitle(SaveType type, int id) {
  switch (type) {
    case SaveType::Quick:
      return ((SaveFileEntry*)QuickSaveEntries[id])->SwTitle;
    case SaveType::Full:
      return ((SaveFileEntry*)FullSaveEntries[id])->SwTitle;
    default:
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Failed to get save title: unknown save type, returning 0\n");
      return 0;
  }
}

uint32_t SaveSystem::GetTipStatus(int tipId) {
  tipId *= 3;
  uint8_t lockStatus = (GameExtraData[tipId >> 3] & Flbit[tipId & 7]) != 0;
  uint8_t newStatus =
      (GameExtraData[(tipId + 1) >> 3] & Flbit[(tipId + 1) & 7]) != 0;
  uint8_t unreadStatus =
      (GameExtraData[(tipId + 2) >> 3] & Flbit[(tipId + 2) & 7]) != 0;
  return (lockStatus | (unreadStatus << 1)) | (newStatus << 2);
}

void SaveSystem::SetTipStatus(int tipId, bool isLocked, bool isUnread,
                              bool isNew) {
  tipId *= 3;
  if (isLocked) {
    GameExtraData[tipId >> 3] &= ~(Flbit[tipId & 7]);
  } else {
    GameExtraData[tipId >> 3] |= Flbit[tipId & 7];
  }
  if (isUnread) {
    GameExtraData[(tipId + 2) >> 3] &= ~(Flbit[(tipId + 2) & 7]);
  } else {
    GameExtraData[(tipId + 2) >> 3] |= Flbit[(tipId + 2) & 7];
  }
  if (isNew) {
    GameExtraData[(tipId + 1) >> 3] &= ~(Flbit[(tipId + 1) & 7]);
  } else {
    GameExtraData[(tipId + 1) >> 3] |= Flbit[(tipId + 1) & 7];
  }
}

void SaveSystem::SetLineRead(int scriptId, int lineId) {
  if (scriptId >= 255) return;

  int offset = ScriptMessageData[scriptId].SaveDataOffset + lineId;
  if (offset == 0xFFFFFFFF) return;

  // TODO: update some ScrWorks (2003, 2005 & 2006)

  MessageFlags[offset >> 3] |= Flbit[offset & 0b111];
}

bool SaveSystem::IsLineRead(int scriptId, int lineId) {
  if (scriptId >= 255) return false;

  uint32_t offset = ScriptMessageData[scriptId].SaveDataOffset + lineId;
  uint8_t flbit = Flbit[offset & 0b111];
  uint8_t viewed = MessageFlags[offset >> 3];

  return (bool)(flbit & viewed);
}

void SaveSystem::GetReadMessagesCount(int* totalMessageCount,
                                      int* readMessageCount) {
  *totalMessageCount = 0;
  *readMessageCount = 0;

  for (int scriptId = 0; scriptId < StoryScriptCount; scriptId++) {
    ScriptMessageDataPair script = ScriptMessageData[scriptId];
    *totalMessageCount += script.LineCount;

    for (size_t lineId = 0; lineId < script.LineCount; lineId++) {
      *readMessageCount += IsLineRead(scriptId, (int)lineId);
    }
  }
}

void SaveSystem::GetViewedEVsCount(int* totalEVCount, int* viewedEVCount) {
  for (int i = 0; i < MaxAlbumEntries; i++) {
    if (AlbumEvData[i][0] == 0xFFFF) break;
    for (int j = 0; j < MaxAlbumSubEntries; j++) {
      if (AlbumEvData[i][j] == 0xFFFF) break;
      *totalEVCount += 1;
      *viewedEVCount += EVFlags[AlbumEvData[i][j]];
    }
  }
}
void SaveSystem::GetEVStatus(int evId, int* totalVariations,
                             int* viewedVariations) {
  *totalVariations = 0;
  *viewedVariations = 0;
  for (int i = 0; i < MaxAlbumSubEntries; i++) {
    if (AlbumEvData[evId][i] == 0xFFFF) break;
    *totalVariations += 1;
    *viewedVariations += EVFlags[AlbumEvData[evId][i]];
  }
}

void SaveSystem::SetEVStatus(int id) { EVFlags[id] = true; }

bool SaveSystem::GetEVVariationIsUnlocked(size_t evId, size_t variationIdx) {
  if (AlbumEvData[evId][variationIdx] == 0xFFFF) return false;
  return EVFlags[AlbumEvData[evId][variationIdx]];
}

bool SaveSystem::GetBgmFlag(int id) { return BGMFlags[id]; }
void SaveSystem::SetBgmFlag(int id, bool flag) { BGMFlags[id] = flag; }

void SaveSystem::SetCheckpointId(int id) {
  if (WorkingSaveEntry) WorkingSaveEntry->MainThreadIp = id;
}

Sprite& SaveSystem::GetSaveThumbnail(SaveType type, int id) {
  switch (type) {
    case SaveType::Quick:
      return ((SaveFileEntry*)QuickSaveEntries[id])->SaveThumbnail;
    case SaveType::Full:
      return ((SaveFileEntry*)FullSaveEntries[id])->SaveThumbnail;
  }

  throw std::invalid_argument(fmt::format(
      "Tried to get thumbnail of unimplemented save entry type {}", (int)type));
}

}  // namespace CCLCC
}  // namespace Impacto