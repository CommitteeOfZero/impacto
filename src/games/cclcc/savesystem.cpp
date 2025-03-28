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

SaveError SaveSystem::CheckSaveFile() {
  std::error_code ec;
  IoError existsState = Io::PathExists(SaveFilePath);
  if (existsState == IoError_NotFound) {
    return SaveNotFound;
  } else if (existsState == IoError_Fail) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Failed to check if save file exists, error: \"{:s}\"\n",
           ec.message());
    return SaveFailed;
  }
  auto saveFileSize = Io::GetFileSize(SaveFilePath);
  if (saveFileSize == IoError_Fail) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Failed to get save file size, error: \"{:s}\"\n", ec.message());
    return SaveFailed;
  } else if (saveFileSize != SaveFileSize) {
    return SaveCorrupted;
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
    return SaveFailed;
  } else if ((!checkPermsBit(perms, Io::FilePermissionsFlags::owner_read) ||
              !checkPermsBit(perms, Io::FilePermissionsFlags::owner_write))) {
    return SaveWrongUser;
  }
  return SaveOK;
}

SaveError SaveSystem::CreateSaveFile() {
  using CF = Io::PhysicalFileStream::CreateFlagsMode;
  Io::Stream* stream;
  IoError err = Io::PhysicalFileStream::Create(
      SaveFilePath, &stream,
      CF::CREATE_IF_NOT_EXISTS | CF::CREATE_DIRS | CF::WRITE);
  if (err != IoError_OK) {
    return SaveFailed;
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

  return MountSaveFile();
}

SaveError SaveSystem::MountSaveFile() {
  Io::Stream* stream;
  IoError err = Io::PhysicalFileStream::Create(SaveFilePath, &stream);
  switch (err) {
    case IoError_NotFound:
      return SaveNotFound;
    case IoError_Fail:
    case IoError_Eof:
      return SaveCorrupted;
    case IoError_OK:
      break;
  };
  WorkingSaveEntry = std::optional<SaveFileEntry>(SaveFileEntry());
  WorkingSaveThumbnail.Sheet =
      SpriteSheet(Window->WindowWidth, Window->WindowHeight);
  WorkingSaveThumbnail.Bounds =
      RectF(0.0f, 0.0f, Window->WindowWidth, Window->WindowHeight);
  Texture txt;
  txt.LoadSolidColor(WorkingSaveThumbnail.Bounds.Width,
                     WorkingSaveThumbnail.Bounds.Height, 0x000000);
  WorkingSaveThumbnail.Sheet.Texture = txt.Submit();

  stream->Seek(0x14, SEEK_SET);

  stream->Seek(0x80, SEEK_SET);
  Io::ReadArrayLE<uint8_t>(&FlagWork[100], stream, 50);
  Io::ReadArrayLE<uint8_t>(&FlagWork[460], stream, 40);
  stream->Seek(0xDC, SEEK_SET);
  Io::ReadArrayLE<int>(&ScrWork[1600], stream, 400);
  Io::ReadArrayLE<int>(&ScrWork[2000], stream, 100);

  // Config settings
  stream->Seek(0x8AC, SEEK_SET);
  TextSpeed = Io::ReadLE<Uint16>(stream) / 60.0f;
  AutoSpeed = Io::ReadLE<Uint16>(stream) / 60.0f;
  stream->Seek(1, SEEK_CUR);  // VOICE2vol
  Audio::GroupVolumes[Audio::ACG_Voice] = Io::ReadLE<Uint8>(stream) / 128.0f;
  Audio::GroupVolumes[Audio::ACG_BGM] = Io::ReadLE<Uint8>(stream) / 256.0f;
  Audio::GroupVolumes[Audio::ACG_SE] = Io::ReadLE<Uint8>(stream) / 128.0f;
  stream->Seek(1, SEEK_CUR);  // SYSSEvol
  Audio::GroupVolumes[Audio::ACG_Movie] = Io::ReadLE<Uint8>(stream) / 128.0f;
  SyncVoice = Io::ReadLE<bool>(stream);
  SkipRead = !Io::ReadLE<bool>(stream);

  stream->Seek(0x8BE, SEEK_SET);
  for (size_t i = 0; i < 33; i++) VoiceMuted[i] = !Io::ReadLE<bool>(stream);
  for (size_t i = 0; i < 33; i++)
    VoiceVolume[i] = Io::ReadLE<Uint8>(stream) / 128.0f;

  stream->Seek(0x901, SEEK_SET);
  SkipVoice = Io::ReadLE<bool>(stream);
  ShowTipsNotification = Io::ReadLE<bool>(stream);

  stream->Seek(0x905, SEEK_SET);
  AdvanceTextOnDirectionalInput = Io::ReadLE<bool>(stream);
  DirectionalInputForTrigger = Io::ReadLE<bool>(stream);
  TriggerStopSkip = Io::ReadLE<bool>(stream);

  // EV Flags
  stream->Seek(0xC0E, SEEK_SET);
  for (int i = 0; i < 150; i++) {
    auto val = Io::ReadU8(stream);
    EVFlags[8 * i] = val & 1;
    EVFlags[8 * i + 1] = (val & 2) != 0;
    EVFlags[8 * i + 2] = (val & 4) != 0;
    EVFlags[8 * i + 3] = (val & 8) != 0;
    EVFlags[8 * i + 4] = (val & 0x10) != 0;
    EVFlags[8 * i + 5] = (val & 0x20) != 0;
    EVFlags[8 * i + 6] = (val & 0x40) != 0;
    EVFlags[8 * i + 7] = val >> 7;
  }

  stream->Seek(0xCA4, SEEK_SET);
  Io::ReadArrayLE<uint8_t>(BGMFlags, stream, 100);

  stream->Seek(0xd6c, SEEK_SET);
  Io::ReadArrayLE<uint8_t>(MessageFlags, stream, 10000);

  // EPnewList goes here

  stream->Seek(0x347c, SEEK_SET);
  Io::ReadArrayLE<uint8_t>(GameExtraData, stream, 1024);

  stream->Seek(0x387c, SEEK_SET);  // TODO: Actually load system data

  for (auto& entryArray : {FullSaveEntries, QuickSaveEntries}) {
    int64_t saveDataPos = stream->Position;
    for (int i = 0; i < MaxSaveEntries; i++) {
      int64_t startPos = stream->Position;
      assert(startPos - saveDataPos == 0x1b110 * i);
      entryArray[i] = new SaveFileEntry();

      entryArray[i]->Status = Io::ReadLE<uint16_t>(stream);
      if (entryArray[i]->Status == 1 && entryArray == QuickSaveEntries) {
        QuickSaveCount++;
      }
      // Not sure about these two
      entryArray[i]->Checksum = Io::ReadLE<uint16_t>(stream);
      Io::ReadLE<uint32_t>(stream);
      uint16_t saveYear = Io::ReadLE<uint16_t>(stream);
      uint8_t saveDay = Io::ReadLE<uint8_t>(stream);
      uint8_t saveMonth = Io::ReadLE<uint8_t>(stream);
      uint8_t saveSecond = Io::ReadLE<uint8_t>(stream);
      uint8_t saveMinute = Io::ReadLE<uint8_t>(stream);
      uint8_t saveHour = Io::ReadLE<uint8_t>(stream);
      std::tm t{};
      t.tm_sec = saveSecond;
      t.tm_min = saveMinute;
      t.tm_hour = saveHour;
      t.tm_mday = saveDay;
      t.tm_mon = saveMonth - 1;
      t.tm_year = saveYear - 1900;
      entryArray[i]->SaveDate = t;

      Io::ReadLE<uint8_t>(stream);
      entryArray[i]->PlayTime = Io::ReadLE<uint32_t>(stream);
      entryArray[i]->SwTitle = Io::ReadLE<uint32_t>(stream);
      Io::ReadLE<uint32_t>(stream);
      entryArray[i]->Flags = Io::ReadLE<uint8_t>(stream);
      stream->Seek(7, SEEK_CUR);
      entryArray[i]->SaveType = Io::ReadLE<uint32_t>(stream);
      assert(stream->Position - startPos == 0x28);
      stream->Seek(0x58, SEEK_CUR);
      Io::ReadArrayLE<uint8_t>(((SaveFileEntry*)entryArray[i])->FlagWorkScript1,
                               stream, 50);
      assert(stream->Position - startPos == 178);
      Io::ReadArrayLE<uint8_t>(((SaveFileEntry*)entryArray[i])->FlagWorkScript2,
                               stream, 100);
      Io::ReadLE<uint16_t>(stream);
      assert(stream->Position - startPos == 280);
      Io::ReadArrayLE<int>(((SaveFileEntry*)entryArray[i])->ScrWorkScript1,
                           stream, 600);
      assert(stream->Position - startPos == 2680);
      Io::ReadArrayLE<int>(((SaveFileEntry*)entryArray[i])->ScrWorkScript2,
                           stream, 3000);

      assert(stream->Position - startPos == 0x3958);
      entryArray[i]->MainThreadExecPriority = Io::ReadLE<uint32_t>(stream);
      entryArray[i]->MainThreadGroupId = Io::ReadLE<uint32_t>(stream);
      entryArray[i]->MainThreadWaitCounter = Io::ReadLE<uint32_t>(stream);
      entryArray[i]->MainThreadScriptParam = Io::ReadLE<uint32_t>(stream);
      entryArray[i]->MainThreadIp = Io::ReadLE<uint32_t>(stream);
      entryArray[i]->MainThreadLoopCounter = Io::ReadLE<uint32_t>(stream);
      entryArray[i]->MainThreadLoopAdr = Io::ReadLE<uint32_t>(stream);
      entryArray[i]->MainThreadCallStackDepth = Io::ReadLE<uint32_t>(stream);
      for (int j = 0; j < 8; j++) {
        entryArray[i]->MainThreadReturnIds[j] = Io::ReadLE<uint32_t>(stream);
      }
      for (int j = 0; j < 8; j++) {
        entryArray[i]->MainThreadReturnBufIds[j] = Io::ReadLE<uint32_t>(stream);
      }
      Io::ReadLE<uint32_t>(stream);
      assert(stream->Position - startPos == 0x39bc);
      entryArray[i]->MainThreadScriptBufferId = Io::ReadLE<uint32_t>(stream);
      Io::ReadArrayBE<int>(entryArray[i]->MainThreadVariables, stream, 16);
      entryArray[i]->MainThreadDialoguePageId = Io::ReadLE<uint32_t>(stream);
      assert(stream->Position - startPos == 0x3a04);
      stream->Seek(1212, SEEK_CUR);
      assert(stream->Position - startPos == 0x3ec0);
      Io::ReadArrayLE<uint8_t>(((SaveFileEntry*)entryArray[i])->MapLoadData,
                               stream, 0x6ac8);
      Io::ReadArrayLE<uint8_t>(((SaveFileEntry*)entryArray[i])->YesNoData,
                               stream, 0x54);

      auto& thumbnail = entryArray[i]->SaveThumbnail;
      thumbnail.Sheet = SpriteSheet(SaveThumbnailWidth, SaveThumbnailHeight);
      thumbnail.Bounds =
          RectF(0.0f, 0.0f, SaveThumbnailWidth, SaveThumbnailHeight);

      Texture tex;
      tex.Init(TexFmt_RGB, SaveThumbnailWidth, SaveThumbnailHeight);

      // CCLCC PS4 Save thumbnails are 240x135 RGB16
      std::vector<uint16_t> thumbnailData(SaveThumbnailWidth *
                                          SaveThumbnailHeight);

      int thumbnailPadding = 0xA14;
      stream->Seek(thumbnailPadding, SEEK_CUR);
      Io::ReadArrayLE<uint16_t>(thumbnailData.data(), stream,
                                thumbnailData.size());

      for (int i = 0; i < thumbnailData.size(); i++) {
        uint16_t pixel = thumbnailData[i];
        uint8_t r = (pixel & 0xF800) >> 8;
        uint8_t g = (pixel & 0x07E0) >> 3;
        uint8_t b = (pixel & 0x001F) << 3;
        tex.Buffer[3 * i] = r;
        tex.Buffer[3 * i + 1] = g;
        tex.Buffer[3 * i + 2] = b;
      }

      thumbnail.Sheet.Texture = tex.Submit();
    }
  }
  delete stream;
  return SaveOK;
}

// uint16_t CalculateChecksum(int id) {
//  return 0;
//}

void SaveSystem::FlushWorkingSaveEntry(SaveType type, int id,
                                       int autoSaveType) {
  SaveFileEntry* entry = 0;
  switch (type) {
    case SaveQuick:
      entry = (SaveFileEntry*)QuickSaveEntries[id];
      break;
    case SaveFull:
      entry = (SaveFileEntry*)FullSaveEntries[id];
      break;
  }

  if (entry != 0 && !(GetSaveFlags(type, id) & WriteProtect)) {
    Renderer->FreeTexture(entry->SaveThumbnail.Sheet.Texture);
    *entry = *WorkingSaveEntry;
    if (type == SaveQuick) {
      entry->SaveType = autoSaveType;
    }
    time_t rawtime;
    time(&rawtime);
    entry->SaveDate = *localtime(&rawtime);
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

void SaveSystem::WriteSaveFile() {
  using CF = Io::PhysicalFileStream::CreateFlagsMode;
  Io::Stream* stream;
  IoError err = Io::PhysicalFileStream::Create(SaveFilePath, &stream,
                                               CF::WRITE | CF::READ);
  if (err != IoError_OK) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Failed to open save file for writing\n");
    return;
  }

  stream->Seek(0x14, SEEK_SET);

  stream->Seek(0x80, SEEK_SET);
  Io::WriteArrayLE<uint8_t>(&FlagWork[100], stream, 50);
  Io::WriteArrayLE<uint8_t>(&FlagWork[460], stream, 40);
  stream->Seek(0xDC, SEEK_SET);
  Io::WriteArrayLE<int>(&ScrWork[1600], stream, 400);
  Io::WriteArrayLE<int>(&ScrWork[2000], stream, 100);

  // Config settings
  stream->Seek(0x8AC, SEEK_SET);
  Io::WriteLE(stream, (Uint16)(TextSpeed * 60));
  Io::WriteLE(stream, (Uint16)(AutoSpeed * 60));
  Io::WriteLE(stream, (Uint8)(Audio::GroupVolumes[Audio::ACG_Voice] *
                              128));  // VOICE2vol
  Io::WriteLE(stream, (Uint8)(Audio::GroupVolumes[Audio::ACG_Voice] *
                              128));  // VOICEvol
  Io::WriteLE(stream, (Uint8)(Audio::GroupVolumes[Audio::ACG_BGM] * 256));
  Io::WriteLE(stream,
              (Uint8)(Audio::GroupVolumes[Audio::ACG_SE] * 128));  // SEvol
  Io::WriteLE(
      stream,
      (Uint8)(Audio::GroupVolumes[Audio::ACG_SE] * 0.6 * 128));  // SYSSEvol
  Io::WriteLE(stream, (Uint8)(Audio::GroupVolumes[Audio::ACG_Movie] * 128));
  Io::WriteLE(stream, SyncVoice);
  Io::WriteLE(stream, !SkipRead);

  stream->Seek(0x8BE, SEEK_SET);
  for (size_t i = 0; i < 33; i++) Io::WriteLE(stream, !VoiceMuted[i]);
  for (size_t i = 0; i < 33; i++)
    Io::WriteLE(stream, (Uint8)(VoiceVolume[i] * 128));

  stream->Seek(0x901, SEEK_SET);
  Io::WriteLE(stream, SkipVoice);
  Io::WriteLE(stream, ShowTipsNotification);

  stream->Seek(0x905, SEEK_SET);
  Io::WriteLE(stream, AdvanceTextOnDirectionalInput);
  Io::WriteLE(stream, DirectionalInputForTrigger);
  Io::WriteLE(stream, TriggerStopSkip);

  // EV Flags
  stream->Seek(0xC0E, SEEK_SET);
  for (int i = 0; i < 150; i++) {
    auto val = Io::ReadU8(stream);
    EVFlags[8 * i] = val & 1;
    EVFlags[8 * i + 1] = (val & 2) != 0;
    EVFlags[8 * i + 2] = (val & 4) != 0;
    EVFlags[8 * i + 3] = (val & 8) != 0;
    EVFlags[8 * i + 4] = (val & 0x10) != 0;
    EVFlags[8 * i + 5] = (val & 0x20) != 0;
    EVFlags[8 * i + 6] = (val & 0x40) != 0;
    EVFlags[8 * i + 7] = val >> 7;
  }

  stream->Seek(0xCA4, SEEK_SET);
  Io::WriteArrayLE<uint8_t>(BGMFlags, stream, 100);

  stream->Seek(0xd6c, SEEK_SET);
  Io::WriteArrayLE<uint8_t>(MessageFlags, stream, 10000);

  // EPnewList goes here

  stream->Seek(0x347c, SEEK_SET);
  Io::WriteArrayLE<uint8_t>(GameExtraData, stream, 1024);

  stream->Seek(0x387c, SEEK_SET);  // TODO: Actually save system data
  std::vector<uint8_t> thumbnailData(SaveThumbnailWidth * SaveThumbnailHeight *
                                     4);
  for (auto* entryArray : {FullSaveEntries, QuickSaveEntries}) {
    int64_t saveDataPos = stream->Position;
    for (int i = 0; i < MaxSaveEntries; i++) {
      SaveFileEntry* entry = (SaveFileEntry*)entryArray[i];
      if (entry->Status == 0) {
        stream->Seek(0x1b110, SEEK_CUR);
      } else {
        int64_t startPos = stream->Position;
        assert(startPos - saveDataPos == 0x1b110 * i);
        Io::WriteLE<uint16_t>(stream, entry->Status);
        Io::WriteLE<uint16_t>(stream, entry->Checksum);
        Io::WriteLE<uint32_t>(stream, 0);

        Io::WriteLE<uint16_t>(stream, entry->SaveDate.tm_year + 1900);
        Io::WriteLE<uint8_t>(stream, entry->SaveDate.tm_mday);
        Io::WriteLE<uint8_t>(stream, entry->SaveDate.tm_mon + 1);
        Io::WriteLE<uint8_t>(stream, entry->SaveDate.tm_sec);
        Io::WriteLE<uint8_t>(stream, entry->SaveDate.tm_min);
        Io::WriteLE<uint8_t>(stream, entry->SaveDate.tm_hour);
        Io::WriteLE<uint8_t>(stream, 0);

        Io::WriteLE<uint32_t>(stream, entry->PlayTime);
        Io::WriteLE<uint32_t>(stream, entry->SwTitle);
        Io::WriteLE<uint32_t>(stream, 0);
        Io::WriteLE<uint8_t>(stream, entry->Flags);
        stream->Seek(7, SEEK_CUR);
        Io::WriteLE<uint32_t>(stream, entry->SaveType);
        assert(stream->Position - startPos == 0x28);
        stream->Seek(0x58, SEEK_CUR);
        Io::WriteArrayLE<uint8_t>(entry->FlagWorkScript1, stream, 50);
        assert(stream->Position - startPos == 178);
        Io::WriteArrayLE<uint8_t>(entry->FlagWorkScript2, stream, 100);
        Io::WriteLE<uint16_t>(stream, 0);
        assert(stream->Position - startPos == 280);
        Io::WriteArrayLE<int>(entry->ScrWorkScript1, stream, 600);
        assert(stream->Position - startPos == 2680);
        Io::WriteArrayLE<int>(entry->ScrWorkScript2, stream, 3000);

        assert(stream->Position - startPos == 0x3958);
        Io::WriteLE<uint32_t>(stream, entry->MainThreadExecPriority);
        Io::WriteLE<uint32_t>(stream, entry->MainThreadGroupId);
        Io::WriteLE<uint32_t>(stream, entry->MainThreadWaitCounter);
        Io::WriteLE<uint32_t>(stream, entry->MainThreadScriptParam);
        Io::WriteLE<uint32_t>(stream, entry->MainThreadIp);
        Io::WriteLE<uint32_t>(stream, entry->MainThreadLoopCounter);
        Io::WriteLE<uint32_t>(stream, entry->MainThreadLoopAdr);
        Io::WriteLE<uint32_t>(stream, entry->MainThreadCallStackDepth);
        for (int j = 0; j < 8; j++) {
          Io::WriteLE<uint32_t>(stream, entry->MainThreadReturnIds[j]);
        }
        for (int j = 0; j < 8; j++) {
          Io::WriteLE<uint32_t>(stream, entry->MainThreadReturnBufIds[j]);
        }
        Io::WriteLE<uint32_t>(stream, 0);
        assert(stream->Position - startPos == 0x39bc);
        Io::WriteLE<uint32_t>(stream, entry->MainThreadScriptBufferId);
        Io::WriteArrayBE<int>(entry->MainThreadVariables, stream, 16);
        Io::WriteLE<uint32_t>(stream, entry->MainThreadDialoguePageId);
        assert(stream->Position - startPos == 0x3a04);
        stream->Seek(1212, SEEK_CUR);
        assert(stream->Position - startPos == 0x3ec0);
        Io::WriteArrayLE<uint8_t>(entry->MapLoadData, stream, 0x6ac8);
        Io::WriteArrayLE<uint8_t>(entry->YesNoData, stream, 0x54);

        // CCLCC PS4 Save thumbnails are 240x135 RGB16
        Renderer->GetSpriteSheetImage(entry->SaveThumbnail.Sheet,
                                      thumbnailData);

        int thumbnailPadding = 0xA14;
        stream->Seek(thumbnailPadding, SEEK_CUR);

        for (int i = 0; i < thumbnailData.size(); i += 4) {
          uint8_t r = thumbnailData[i];
          uint8_t g = thumbnailData[i + 1];
          uint8_t b = thumbnailData[i + 2];
          uint16_t pixel = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
          pixel = SDL_SwapBE16(pixel);
          thumbnailData[i / 2] = pixel >> 8;
          thumbnailData[i / 2 + 1] = pixel & 0xFF;
        }
        Io::WriteArrayLE<uint8_t>(thumbnailData.data(), stream,
                                  thumbnailData.size() / 2);
      }
    }
  }
  delete stream;
}

uint32_t SaveSystem::GetSavePlayTime(SaveType type, int id) {
  switch (type) {
    case SaveFull:
      return ((SaveFileEntry*)FullSaveEntries[id])->PlayTime;
    case SaveQuick:
      return ((SaveFileEntry*)QuickSaveEntries[id])->PlayTime;
    default:
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Failed to get save play time: unknown save type, returning 0\n");
      return 0;
  }
}

uint8_t SaveSystem::GetSaveFlags(SaveType type, int id) {
  switch (type) {
    case SaveFull:
      return ((SaveFileEntry*)FullSaveEntries[id])->Flags;
    case SaveQuick:
      return ((SaveFileEntry*)QuickSaveEntries[id])->Flags;
    default:
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Failed to get save flags: unknown save type, returning 0\n");
      return 0;
  }
}

tm const& SaveSystem::GetSaveDate(SaveType type, int id) {
  static const tm t = []() {
    tm t;
    t.tm_sec = 0;
    t.tm_min = 0;
    t.tm_hour = 0;
    t.tm_mday = 1;
    t.tm_mon = 0;
    t.tm_year = 0;
    return t;
  }();
  switch (type) {
    case SaveFull:
      return ((SaveFileEntry*)FullSaveEntries[id])->SaveDate;
    case SaveQuick:
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
    WorkingSaveEntry->Checksum = 0;  // CalculateChecksum(0);
    time_t rawtime;
    time(&rawtime);
    tm* timeinfo = localtime(&rawtime);
    WorkingSaveEntry->SaveDate = *timeinfo;
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
      for (int i = 0; i < thd->CallStackDepth; i++) {
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
    case SaveQuick:
      WorkingSaveEntry = *static_cast<SaveFileEntry*>(QuickSaveEntries[id]);
      break;
    case SaveFull:
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
  if (load == LoadProcess::LoadVars) {
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
      thd->Ip = ScriptGetRetAddress(
          ScriptBuffers[WorkingSaveEntry->MainThreadScriptBufferId],
          WorkingSaveEntry->MainThreadIp);
      thd->CallStackDepth = WorkingSaveEntry->MainThreadCallStackDepth;

      for (int i = 0; i < thd->CallStackDepth; i++) {
        thd->ReturnScriptBufferIds[i] =
            WorkingSaveEntry->MainThreadReturnBufIds[i];
        thd->ReturnIds[i] = WorkingSaveEntry->MainThreadReturnIds[i];
      }

      memcpy(thd->Variables, WorkingSaveEntry->MainThreadVariables, 64);
      thd->DialoguePageId = WorkingSaveEntry->MainThreadDialoguePageId;
    }
  }
}

uint8_t SaveSystem::GetSaveStatus(SaveType type, int id) {
  switch (type) {
    case SaveQuick:
      return QuickSaveEntries[id] != nullptr
                 ? ((SaveFileEntry*)QuickSaveEntries[id])->Status
                 : 0;
    case SaveFull:
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
    case SaveQuick:
      return ((SaveFileEntry*)QuickSaveEntries[id])->SwTitle;
    case SaveFull:
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

    for (int lineId = 0; lineId < script.LineCount; lineId++) {
      *readMessageCount += IsLineRead(scriptId, lineId);
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

bool SaveSystem::GetEVVariationIsUnlocked(int evId, int variationIdx) {
  if (AlbumEvData[evId][variationIdx] == 0xFFFF) return false;
  return EVFlags[AlbumEvData[evId][variationIdx]];
}

bool SaveSystem::GetBgmFlag(int id) { return BGMFlags[id]; }

void SaveSystem::SetCheckpointId(int id) {
  if (WorkingSaveEntry) WorkingSaveEntry->MainThreadIp = id;
}

Sprite& SaveSystem::GetSaveThumbnail(SaveType type, int id) {
  switch (type) {
    case SaveQuick:
      return ((SaveFileEntry*)QuickSaveEntries[id])->SaveThumbnail;
    case SaveFull:
      return ((SaveFileEntry*)FullSaveEntries[id])->SaveThumbnail;
  }
}
}  // namespace CCLCC
}  // namespace Impacto