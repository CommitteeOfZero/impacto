#include "savesystem.h"

#include "../../io/memorystream.h"
#include "../../io/physicalfilestream.h"
#include "../../mem.h"
#include "../../vm/vm.h"
#include "../../profile/data/savesystem.h"
#include "../../profile/scriptvars.h"
#include "../../renderer/renderer.h"

#include <cstdint>
#include <ctime>

namespace Impacto {
namespace CHLCC {

using namespace Impacto::Vm;
using namespace Impacto::Profile::SaveSystem;
using namespace Impacto::Profile::ScriptVars;

SaveFileEntry* WorkingSaveEntry = 0;

void SaveSystem::SaveSystemData() {
  Io::MemoryStream stream =
      Io::MemoryStream(SystemData.data(), SystemData.size(), false);

  stream.Seek(0x14, SEEK_SET);

  Io::WriteArrayLE<uint8_t>(&FlagWork[100], &stream, 50);
  Io::WriteArrayLE<uint8_t>(&FlagWork[460], &stream, 40);
  Io::WriteArrayBE<int>(&ScrWork[600], &stream, 400);

  stream.Seek(0x7DA, SEEK_SET);

  for (int i = 0; i < 150; i++) {
    uint8_t val = (EVFlags[8 * i] & 1) | ((EVFlags[8 * i + 1] ? 1 : 0) << 1) |
                  ((EVFlags[8 * i + 2] ? 1 : 0) << 2) |
                  ((EVFlags[8 * i + 3] ? 1 : 0) << 3) |
                  ((EVFlags[8 * i + 4] ? 1 : 0) << 4) |
                  ((EVFlags[8 * i + 5] ? 1 : 0) << 5) |
                  ((EVFlags[8 * i + 6] ? 1 : 0) << 6) |
                  (EVFlags[8 * i + 7] << 7);
    Io::WriteU8(&stream, val);
  }

  stream.Seek(0xbc2, SEEK_SET);
  Io::WriteArrayLE<uint8_t>(BGMFlags, &stream, 100);

  stream.Seek(0xc26, SEEK_SET);
  Io::WriteArrayLE<uint8_t>(MessageFlags, &stream, 10000);

  stream.Seek(0x358E, SEEK_SET);
  Io::WriteArrayLE<uint8_t>(GameExtraData, &stream, 1024);

  stream.Seek(0x3b06, SEEK_SET);  // TODO: Actually write system data
}

SaveError SaveSystem::LoadSystemData() {
  Io::MemoryStream stream =
      Io::MemoryStream(SystemData.data(), SystemData.size(), false);

  stream.Seek(0x14, SEEK_SET);

  Io::ReadArrayLE<uint8_t>(&FlagWork[100], &stream, 50);
  Io::ReadArrayLE<uint8_t>(&FlagWork[460], &stream, 40);
  Io::ReadArrayBE<int>(&ScrWork[600], &stream, 400);

  stream.Seek(0x7DA, SEEK_SET);
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

  stream.Seek(0xbc2, SEEK_SET);
  Io::ReadArrayLE<uint8_t>(BGMFlags, &stream, 100);

  stream.Seek(0xc26, SEEK_SET);
  Io::ReadArrayLE<uint8_t>(MessageFlags, &stream, 10000);

  stream.Seek(0x358E, SEEK_SET);
  Io::ReadArrayLE<uint8_t>(GameExtraData, &stream, 1024);

  stream.Seek(0x3b06, SEEK_SET);  // TODO: Actually load system data

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

  WorkingSaveEntry = new SaveFileEntry();
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

  for (auto& entryArray : {QuickSaveEntries, FullSaveEntries}) {
    for (int i = 0; i < MaxSaveEntries; i++) {
      entryArray[i] = new SaveFileEntry();

      entryArray[i]->Status = Io::ReadLE<uint8_t>(stream);
      entryArray[i]->Checksum = Io::ReadLE<uint16_t>(stream);
      Io::ReadLE<uint8_t>(stream);
      uint8_t saveMonth = Io::ReadLE<uint8_t>(stream);
      uint8_t saveDay = Io::ReadLE<uint8_t>(stream);
      uint8_t saveHour = Io::ReadLE<uint8_t>(stream);
      uint8_t saveMinute = Io::ReadLE<uint8_t>(stream);
      uint8_t saveYear = Io::ReadLE<uint8_t>(stream);
      uint8_t saveSecond = Io::ReadLE<uint8_t>(stream);
      std::tm t{};
      t.tm_sec = saveSecond;
      t.tm_min = saveMinute;
      t.tm_hour = saveHour;
      t.tm_mday = saveDay;
      t.tm_mon = saveMonth - 1;
      t.tm_year = saveYear + 2000 - 1900;
      entryArray[i]->SaveDate = t;

      Io::ReadLE<uint16_t>(stream);
      entryArray[i]->PlayTime = Io::ReadLE<uint32_t>(stream);
      entryArray[i]->SwTitle = Io::ReadLE<uint16_t>(stream);
      Io::ReadLE<uint8_t>(stream);
      entryArray[i]->Flags = Io::ReadLE<uint8_t>(stream);
      stream->Seek(30, SEEK_CUR);
      Io::ReadArrayLE<uint8_t>(((SaveFileEntry*)entryArray[i])->FlagWorkScript1,
                               stream, 50);
      Io::ReadArrayLE<uint8_t>(((SaveFileEntry*)entryArray[i])->FlagWorkScript2,
                               stream, 100);
      Io::ReadArrayBE<int>(((SaveFileEntry*)entryArray[i])->ScrWorkScript1,
                           stream, 300);
      Io::ReadArrayBE<int>(((SaveFileEntry*)entryArray[i])->ScrWorkScript2,
                           stream, 1300);
      entryArray[i]->MainThreadExecPriority = Io::ReadBE<uint32_t>(stream);
      entryArray[i]->MainThreadGroupId = Io::ReadBE<uint32_t>(stream);
      entryArray[i]->MainThreadWaitCounter = Io::ReadBE<uint32_t>(stream);
      entryArray[i]->MainThreadScriptParam = Io::ReadBE<uint32_t>(stream);
      entryArray[i]->MainThreadIp = Io::ReadBE<uint32_t>(stream);
      entryArray[i]->MainThreadLoopCounter = Io::ReadBE<uint32_t>(stream);
      entryArray[i]->MainThreadLoopAdr = Io::ReadBE<uint32_t>(stream);
      entryArray[i]->MainThreadCallStackDepth = Io::ReadBE<uint32_t>(stream);
      for (int j = 0; j < 8; j++) {
        entryArray[i]->MainThreadReturnAddresses[j] =
            Io::ReadBE<uint32_t>(stream);
        entryArray[i]->MainThreadReturnBufIds[j] = Io::ReadBE<uint32_t>(stream);
      }
      Io::ReadArrayBE<int>(entryArray[i]->MainThreadVariables, stream, 16);
      entryArray[i]->MainThreadDialoguePageId = Io::ReadBE<uint32_t>(stream);
      stream->Seek(1428, SEEK_CUR);
    }
  }
  delete stream;
  return SaveError::OK;
}

// uint16_t CalculateChecksum(int id) {
//  return 0;
//}

void SaveSystem::FlushWorkingSaveEntry(SaveType type, int id,
                                       int autoSaveType) {
  SaveFileEntry* entry = 0;
  switch (type) {
    case SaveType::Quick:
      entry = (SaveFileEntry*)QuickSaveEntries[id];
      break;
    case SaveType::Full:
      entry = (SaveFileEntry*)FullSaveEntries[id];
      break;
  }

  if (WorkingSaveEntry != 0) {
    if (entry != 0 && !(GetSaveFlags(type, id) & WriteProtect)) {
      *entry = *WorkingSaveEntry;
      if (type == SaveType::Quick) {
        entry->SaveType = autoSaveType;
      }
      time_t rawtime;
      time(&rawtime);
      entry->SaveDate = CurrentDateTime();
    }
  }
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

  Io::WriteArrayLE<uint8_t>(SystemData.data(), stream, SystemData.size());

  for (auto& entryArray : {QuickSaveEntries, FullSaveEntries}) {
    int64_t saveDataPos = stream->Position;
    for (int i = 0; i < MaxSaveEntries; i++) {
      assert(stream->Position - saveDataPos == i * 0x2000);
      SaveFileEntry* entry = (SaveFileEntry*)entryArray[i];

      if (entry->Status == 0) {
        stream->Seek(0x2000, SEEK_CUR);
      } else {
        Io::WriteLE<uint8_t>(stream, entry->Status);
        Io::WriteLE<uint16_t>(stream, (uint16_t)entry->Checksum);
        Io::WriteLE<uint8_t>(stream, 0);

        Io::WriteLE<uint8_t>(stream, (uint8_t)(entry->SaveDate.tm_mon + 1));
        Io::WriteLE<uint8_t>(stream, (uint8_t)entry->SaveDate.tm_mday);
        Io::WriteLE<uint8_t>(stream, (uint8_t)entry->SaveDate.tm_hour);
        Io::WriteLE<uint8_t>(stream, (uint8_t)entry->SaveDate.tm_min);
        Io::WriteLE<uint8_t>(stream,
                             (uint8_t)(entry->SaveDate.tm_year + 1900 - 2000));
        Io::WriteLE<uint8_t>(stream, (uint8_t)entry->SaveDate.tm_sec);

        Io::WriteLE<uint16_t>(stream, 0);
        Io::WriteLE<uint32_t>(stream, entry->PlayTime);
        Io::WriteLE<uint16_t>(stream, (uint16_t)entry->SwTitle);
        Io::WriteLE<uint8_t>(stream, 0);
        Io::WriteLE<uint8_t>(stream, entry->Flags);
        stream->Seek(30, SEEK_CUR);
        Io::WriteArrayLE<uint8_t>(entry->FlagWorkScript1, stream, 50);
        Io::WriteArrayLE<uint8_t>(entry->FlagWorkScript2, stream, 100);
        Io::WriteArrayBE<int>(entry->ScrWorkScript1, stream, 300);
        Io::WriteArrayBE<int>(entry->ScrWorkScript2, stream, 1300);
        Io::WriteBE<uint32_t>(stream, entry->MainThreadExecPriority);
        Io::WriteBE<uint32_t>(stream, entry->MainThreadGroupId);
        Io::WriteBE<uint32_t>(stream, entry->MainThreadWaitCounter);
        Io::WriteBE<uint32_t>(stream, entry->MainThreadScriptParam);
        Io::WriteBE<uint32_t>(stream, entry->MainThreadIp);
        Io::WriteBE<uint32_t>(stream, entry->MainThreadLoopCounter);
        Io::WriteBE<uint32_t>(stream, entry->MainThreadLoopAdr);
        Io::WriteBE<uint32_t>(stream, entry->MainThreadCallStackDepth);
        for (int j = 0; j < 8; j++) {
          Io::WriteBE<uint32_t>(stream, entry->MainThreadReturnAddresses[j]);
          Io::WriteBE<uint32_t>(stream, entry->MainThreadReturnBufIds[j]);
        }
        Io::WriteArrayBE<int>(entry->MainThreadVariables, stream, 16);
        Io::WriteBE<uint32_t>(stream, entry->MainThreadDialoguePageId);
        stream->Seek(1428, SEEK_CUR);
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
  if (WorkingSaveEntry != 0) {
    WorkingSaveEntry->Status = 1;

    WorkingSaveEntry->PlayTime = ScrWork[SW_PLAYTIME];
    WorkingSaveEntry->SwTitle = ScrWork[SW_TITLE];

    memcpy(WorkingSaveEntry->FlagWorkScript1, &FlagWork[50], 50);
    memcpy(WorkingSaveEntry->FlagWorkScript2, &FlagWork[300], 100);
    memcpy(WorkingSaveEntry->ScrWorkScript1, &ScrWork[300], 1200);
    memcpy(WorkingSaveEntry->ScrWorkScript2, &ScrWork[2300], 5200);

    int threadId = ScrWork[SW_MAINTHDP];
    Sc3VmThread* thd = &ThreadPool[threadId & 0x7FFFFFFF];
    if (thd != 0 &&
        (thd->GroupId == 4 || thd->GroupId == 5 || thd->GroupId == 6)) {
      WorkingSaveEntry->MainThreadExecPriority = thd->ExecPriority;
      WorkingSaveEntry->MainThreadWaitCounter = thd->WaitCounter;
      WorkingSaveEntry->MainThreadScriptParam = thd->ScriptParam;
      WorkingSaveEntry->MainThreadGroupId = thd->GroupId << 16;
      WorkingSaveEntry->MainThreadGroupId |= thd->ScriptBufferId;
      WorkingSaveEntry->MainThreadIp =
          static_cast<uint32_t>(thd->Ip - ScriptBuffers[thd->ScriptBufferId]);
      WorkingSaveEntry->MainThreadCallStackDepth = thd->CallStackDepth;

      for (uint32_t j = 0; j < thd->CallStackDepth; j++) {
        WorkingSaveEntry->MainThreadReturnAddresses[j] =
            static_cast<uint32_t>(thd->ReturnAddresses[j] -
                                  ScriptBuffers[thd->ReturnScriptBufferIds[j]]);
        WorkingSaveEntry->MainThreadReturnBufIds[j] =
            thd->ReturnScriptBufferIds[j];
      }

      memcpy(WorkingSaveEntry->MainThreadVariables, thd->Variables, 64);
      WorkingSaveEntry->MainThreadDialoguePageId = thd->DialoguePageId;
    }
  }
}

void SaveSystem::LoadEntry(SaveType type, int id) {
  SaveFileEntry* entry = 0;
  switch (type) {
    case SaveType::Quick:
      entry = (SaveFileEntry*)QuickSaveEntries[id];
      break;
    case SaveType::Full:
      entry = (SaveFileEntry*)FullSaveEntries[id];
      break;
    default:
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Failed to load save memory: unknown save type, doing nothing\n");
      return;
  }

  if (entry != 0)
    if (entry->Status) {
      ScrWork[SW_PLAYTIME] = entry->PlayTime;
      ScrWork[SW_TITLE] = entry->SwTitle;

      memcpy(&FlagWork[50], entry->FlagWorkScript1, 50);
      memcpy(&FlagWork[300], entry->FlagWorkScript2, 100);
      memcpy(&ScrWork[300], entry->ScrWorkScript1, 1200);
      memcpy(&ScrWork[2300], entry->ScrWorkScript2, 5200);

      // TODO: What to do about this mess I wonder...
      ScrWork[SW_SVSENO] = ScrWork[SW_SEREQNO];
      ScrWork[SW_SVSENO + 1] = ScrWork[SW_SEREQNO + 1];
      ScrWork[SW_SVSENO + 2] = ScrWork[SW_SEREQNO + 2];
      ScrWork[SW_SVBGMNO] = ScrWork[SW_BGMREQNO];
      ScrWork[SW_SVSCRNO1] = ScrWork[SW_SCRIPTNO2];
      ScrWork[SW_SVSCRNO2] = ScrWork[SW_SCRIPTNO3];
      ScrWork[SW_SVSCRNO3] = ScrWork[SW_SCRIPTNO4];
      ScrWork[SW_SVSCRNO4] = ScrWork[SW_SCRIPTNO5];
      ScrWork[SW_SVBGNO1] = ScrWork[SW_BG1NO];
      ScrWork[SW_SVBGNO1 + 1] = ScrWork[2427];
      ScrWork[SW_SVBGNO1 + 2] = ScrWork[2447];
      ScrWork[SW_SVBGNO1 + 3] = ScrWork[2467];
      ScrWork[SW_SVBGNO1 + 4] = ScrWork[2487];
      ScrWork[SW_SVBGNO1 + 5] = ScrWork[2507];
      ScrWork[SW_SVBGNO1 + 6] = ScrWork[2527];
      ScrWork[SW_SVBGNO1 + 7] = ScrWork[2547];
      ScrWork[SW_SVCHANO1] = ScrWork[SW_CHA1NO];
      ScrWork[SW_SVCHANO1 + 1] = ScrWork[2629];
      ScrWork[SW_SVCHANO1 + 2] = ScrWork[2649];
      ScrWork[SW_SVCHANO1 + 3] = ScrWork[2669];
      ScrWork[SW_SVCHANO1 + 4] = ScrWork[2689];
      ScrWork[SW_SVCHANO1 + 5] = ScrWork[2709];
      ScrWork[SW_SVCHANO1 + 6] = ScrWork[2729];
      ScrWork[SW_SVCHANO1 + 7] = ScrWork[2749];
      ScrWork[SW_SVCHANO1 + 8] = ScrWork[2769];
      ScrWork[SW_SVCHANO1 + 9] = ScrWork[2789];
      ScrWork[SW_SVCHANO1 + 10] = ScrWork[2809];
      ScrWork[SW_SVCHANO1 + 11] = ScrWork[2829];
      ScrWork[SW_SVCHANO1 + 12] = ScrWork[2849];
      ScrWork[SW_SVCHANO1 + 13] = ScrWork[2869];
      ScrWork[SW_SVCHANO1 + 14] = ScrWork[2889];
      ScrWork[SW_SVCHANO1 + 15] = ScrWork[2909];
      ScrWork[2034] = ScrWork[3200];
      ScrWork[2035] = ScrWork[3201];
      ScrWork[2036] = ScrWork[3202];
      ScrWork[2037] = ScrWork[3203];
      ScrWork[2038] = ScrWork[3204];
      ScrWork[2039] = ScrWork[3205];
      ScrWork[2040] = ScrWork[3206];
      ScrWork[2041] = ScrWork[3207];
      ScrWork[2042] = ScrWork[3208];
      ScrWork[2043] = ScrWork[3209];
      ScrWork[2044] = ScrWork[3210];
      ScrWork[2045] = ScrWork[3211];
      ScrWork[2046] = ScrWork[3212];
      ScrWork[2047] = ScrWork[3213];
      ScrWork[2048] = ScrWork[3214];
      ScrWork[2049] = ScrWork[3215];
      ScrWork[2050] = ScrWork[3216];
      ScrWork[2051] = ScrWork[3220];
      ScrWork[2052] = ScrWork[3221];
      ScrWork[2053] = ScrWork[3222];
      ScrWork[2054] = ScrWork[3223];
      ScrWork[2055] = ScrWork[3224];
      ScrWork[2056] = ScrWork[3225];
      ScrWork[2057] = ScrWork[3226];
      ScrWork[2058] = ScrWork[3227];
      ScrWork[2059] = ScrWork[3228];
      ScrWork[2060] = ScrWork[3229];
      ScrWork[2061] = ScrWork[3230];
      ScrWork[2062] = ScrWork[3231];
      ScrWork[2063] = ScrWork[3232];
      ScrWork[2064] = ScrWork[3233];
      ScrWork[2065] = ScrWork[3234];
      ScrWork[2066] = ScrWork[3235];
      ScrWork[2067] = ScrWork[3236];

      int threadId = ScrWork[SW_MAINTHDP];
      Sc3VmThread* thd = &ThreadPool[threadId & 0x7FFFFFFF];
      if (thd != 0 &&
          (thd->GroupId == 4 || thd->GroupId == 5 || thd->GroupId == 6)) {
        thd->ExecPriority = entry->MainThreadExecPriority;
        thd->WaitCounter = entry->MainThreadWaitCounter;
        thd->ScriptParam = entry->MainThreadScriptParam;
        thd->GroupId = entry->MainThreadGroupId >> 16;
        thd->ScriptBufferId = entry->MainThreadGroupId & 0xFFFF;
        LoadScript(thd->ScriptBufferId, ScrWork[2004 + thd->ScriptBufferId]);
        ScrWork[2004 + thd->ScriptBufferId] = 65535;
        thd->Ip = ScriptBuffers[thd->ScriptBufferId] + entry->MainThreadIp;
        thd->CallStackDepth = entry->MainThreadCallStackDepth;

        for (size_t i = 0; i < thd->CallStackDepth; i++) {
          thd->ReturnScriptBufferIds[i] = entry->MainThreadReturnBufIds[i];
          LoadScript(entry->MainThreadReturnBufIds[i],
                     ScrWork[2004 + entry->MainThreadReturnBufIds[i]]);
          ScrWork[2004 + entry->MainThreadReturnBufIds[i]] = 65535;
          thd->ReturnAddresses[i] =
              ScriptBuffers[entry->MainThreadReturnBufIds[i]] +
              entry->MainThreadReturnAddresses[i];
        }

        memcpy(thd->Variables, entry->MainThreadVariables, 64);
        thd->DialoguePageId = entry->MainThreadDialoguePageId;
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
  if (ScriptMessageData.size() <= scriptId) return;
  int offset = ScriptMessageData[scriptId].SaveDataOffset + lineId;
  if (offset == 0xFFFFFFFF) return;

  // TODO: update some ScrWorks (2003, 2005 & 2006)

  MessageFlags[offset >> 3] |= Flbit[offset & 0b111];
}

bool SaveSystem::IsLineRead(int scriptId, int lineId) {
  if (scriptId >= StoryScriptCount) return false;
  if (ScriptMessageData.size() <= scriptId) return false;

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

Sprite& SaveSystem::GetSaveThumbnail(SaveType type, int id) {
  switch (type) {
    case SaveType::Quick:
      return QuickSaveEntries[id]->SaveThumbnail;
    case SaveType::Full:
      return FullSaveEntries[id]->SaveThumbnail;
  }

  throw std::invalid_argument(fmt::format(
      "Tried to get thumbnail of unimplemented save entry type {}", (int)type));
}

}  // namespace CHLCC
}  // namespace Impacto