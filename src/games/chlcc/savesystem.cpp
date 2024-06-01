#include "savesystem.h"

#include "../../io/physicalfilestream.h"
#include "../../mem.h"
#include "../../vm/vm.h"
#include "../../profile/data/savesystem.h"
#include "../../profile/scriptvars.h"

#include <cstdint>
#include <ctime>

namespace Impacto {
namespace CHLCC {

using namespace Impacto::Vm;
using namespace Impacto::Profile::SaveSystem;
using namespace Impacto::Profile::ScriptVars;

SaveFileEntry* WorkingSaveEntry = 0;

SaveError SaveSystem::MountSaveFile() {
  Io::InputStream* stream;
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

  WorkingSaveEntry = new SaveFileEntry();

  stream->Seek(0x14, SEEK_SET);

  Io::ReadArrayLE<uint8_t>(&FlagWork[100], stream, 50);
  Io::ReadArrayLE<uint8_t>(&FlagWork[460], stream, 40);
  Io::ReadArrayBE<int>(&ScrWork[600], stream, 400);

  stream->Seek(0x7DA, SEEK_SET);
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

  stream->Seek(0xbc2, SEEK_SET);
  Io::ReadArrayLE<uint8_t>(BGMFlags, stream, 100);

  stream->Seek(0xc26, SEEK_SET);
  Io::ReadArrayLE<uint8_t>(MessageFlags, stream, 10000);

  stream->Seek(0x358E, SEEK_SET);
  Io::ReadArrayLE<uint8_t>(GameExtraData, stream, 1024);

  stream->Seek(0x3b06, SEEK_SET);  // TODO: Actually load system data

  for (int i = 0; i < MaxSaveEntries; i++) {
    QuickSaveEntries[i] = new SaveFileEntry();

    QuickSaveEntries[i]->Status = Io::ReadLE<uint8_t>(stream);
    QuickSaveEntries[i]->Checksum = Io::ReadLE<uint16_t>(stream);
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
    t.tm_mon = saveMonth;
    t.tm_year = saveYear;
    QuickSaveEntries[i]->SaveDate = t;

    Io::ReadLE<uint16_t>(stream);
    QuickSaveEntries[i]->PlayTime = Io::ReadLE<uint32_t>(stream);
    QuickSaveEntries[i]->SwTitle = Io::ReadLE<uint16_t>(stream);
    Io::ReadLE<uint8_t>(stream);
    QuickSaveEntries[i]->Flags = Io::ReadLE<uint8_t>(stream);
    stream->Seek(30, SEEK_CUR);
    Io::ReadArrayLE<uint8_t>(
        ((SaveFileEntry*)QuickSaveEntries[i])->FlagWorkScript1, stream, 50);
    Io::ReadArrayLE<uint8_t>(
        ((SaveFileEntry*)QuickSaveEntries[i])->FlagWorkScript2, stream, 100);
    Io::ReadArrayBE<int>(((SaveFileEntry*)QuickSaveEntries[i])->ScrWorkScript1,
                         stream, 300);
    Io::ReadArrayBE<int>(((SaveFileEntry*)QuickSaveEntries[i])->ScrWorkScript2,
                         stream, 1300);
    QuickSaveEntries[i]->MainThreadExecPriority = Io::ReadBE<uint32_t>(stream);
    QuickSaveEntries[i]->MainThreadGroupId = Io::ReadBE<uint32_t>(stream);
    QuickSaveEntries[i]->MainThreadWaitCounter = Io::ReadBE<uint32_t>(stream);
    QuickSaveEntries[i]->MainThreadScriptParam = Io::ReadBE<uint32_t>(stream);
    QuickSaveEntries[i]->MainThreadIp = Io::ReadBE<uint32_t>(stream);
    QuickSaveEntries[i]->MainThreadLoopCounter = Io::ReadBE<uint32_t>(stream);
    QuickSaveEntries[i]->MainThreadLoopAdr = Io::ReadBE<uint32_t>(stream);
    QuickSaveEntries[i]->MainThreadCallStackDepth =
        Io::ReadBE<uint32_t>(stream);
    for (int j = 0; j < 8; j++) {
      QuickSaveEntries[i]->MainThreadReturnAddresses[j] =
          Io::ReadBE<uint32_t>(stream);
      QuickSaveEntries[i]->MainThreadReturnBufIds[j] =
          Io::ReadBE<uint32_t>(stream);
    }
    Io::ReadArrayBE<int>(QuickSaveEntries[i]->MainThreadVariables, stream, 16);
    QuickSaveEntries[i]->MainThreadDialoguePageId =
        Io::ReadBE<uint32_t>(stream);
    stream->Seek(1428, SEEK_CUR);
  }

  for (int i = 0; i < MaxSaveEntries; i++) {
    FullSaveEntries[i] = new SaveFileEntry();

    FullSaveEntries[i]->Status = Io::ReadLE<uint8_t>(stream);
    FullSaveEntries[i]->Checksum = Io::ReadLE<uint16_t>(stream);
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
    t.tm_mon = saveMonth;
    t.tm_year = saveYear;
    FullSaveEntries[i]->SaveDate = t;

    Io::ReadLE<uint16_t>(stream);
    FullSaveEntries[i]->PlayTime = Io::ReadLE<uint32_t>(stream);
    FullSaveEntries[i]->SwTitle = Io::ReadLE<uint16_t>(stream);
    Io::ReadLE<uint8_t>(stream);
    FullSaveEntries[i]->Flags = Io::ReadLE<uint8_t>(stream);
    stream->Seek(30, SEEK_CUR);
    Io::ReadArrayLE<uint8_t>(
        ((SaveFileEntry*)FullSaveEntries[i])->FlagWorkScript1, stream, 50);
    Io::ReadArrayLE<uint8_t>(
        ((SaveFileEntry*)FullSaveEntries[i])->FlagWorkScript2, stream, 100);
    Io::ReadArrayBE<int>(((SaveFileEntry*)FullSaveEntries[i])->ScrWorkScript1,
                         stream, 300);
    Io::ReadArrayBE<int>(((SaveFileEntry*)FullSaveEntries[i])->ScrWorkScript2,
                         stream, 1300);
    FullSaveEntries[i]->MainThreadExecPriority = Io::ReadBE<uint32_t>(stream);
    FullSaveEntries[i]->MainThreadGroupId = Io::ReadBE<uint32_t>(stream);
    FullSaveEntries[i]->MainThreadWaitCounter = Io::ReadBE<uint32_t>(stream);
    FullSaveEntries[i]->MainThreadScriptParam = Io::ReadBE<uint32_t>(stream);
    FullSaveEntries[i]->MainThreadIp = Io::ReadBE<uint32_t>(stream);
    FullSaveEntries[i]->MainThreadLoopCounter = Io::ReadBE<uint32_t>(stream);
    FullSaveEntries[i]->MainThreadLoopAdr = Io::ReadBE<uint32_t>(stream);
    FullSaveEntries[i]->MainThreadCallStackDepth = Io::ReadBE<uint32_t>(stream);
    for (int j = 0; j < 8; j++) {
      FullSaveEntries[i]->MainThreadReturnAddresses[j] =
          Io::ReadBE<uint32_t>(stream);
      FullSaveEntries[i]->MainThreadReturnBufIds[j] =
          Io::ReadBE<uint32_t>(stream);
    }
    Io::ReadArrayBE<int>(FullSaveEntries[i]->MainThreadVariables, stream, 16);
    FullSaveEntries[i]->MainThreadDialoguePageId = Io::ReadBE<uint32_t>(stream);
    stream->Seek(1428, SEEK_CUR);
  }

  return SaveOK;
}

// uint16_t CalculateChecksum(int id) {
//  return 0;
//}

void SaveSystem::FlushWorkingSaveEntry(SaveType type, int id) {
  SaveFileEntry* entry = 0;
  switch (type) {
    case SaveQuick:
      entry = (SaveFileEntry*)QuickSaveEntries[id];
      break;
    case SaveFull:
      entry = (SaveFileEntry*)FullSaveEntries[id];
      break;
  }

  if (WorkingSaveEntry != 0) {
    if (entry != 0) {
      entry->Status = 1;

      entry->PlayTime = WorkingSaveEntry->PlayTime;
      entry->SwTitle = WorkingSaveEntry->SwTitle;

      memcpy(entry->FlagWorkScript1, WorkingSaveEntry->FlagWorkScript1, 50);
      memcpy(entry->FlagWorkScript2, WorkingSaveEntry->FlagWorkScript2, 100);
      memcpy(entry->ScrWorkScript1, WorkingSaveEntry->ScrWorkScript1, 1200);
      memcpy(entry->ScrWorkScript2, WorkingSaveEntry->ScrWorkScript2, 5200);

      entry->MainThreadExecPriority = WorkingSaveEntry->MainThreadExecPriority;
      entry->MainThreadWaitCounter = WorkingSaveEntry->MainThreadWaitCounter;
      entry->MainThreadScriptParam = WorkingSaveEntry->MainThreadScriptParam;
      entry->MainThreadGroupId = WorkingSaveEntry->MainThreadGroupId;
      entry->MainThreadGroupId = WorkingSaveEntry->MainThreadGroupId;
      entry->MainThreadIp = WorkingSaveEntry->MainThreadIp;
      entry->MainThreadCallStackDepth =
          WorkingSaveEntry->MainThreadCallStackDepth;

      for (int j = 0; j < 8; j++) {
        entry->MainThreadReturnAddresses[j] =
            WorkingSaveEntry->MainThreadReturnAddresses[j];
        entry->MainThreadReturnBufIds[j] =
            WorkingSaveEntry->MainThreadReturnBufIds[j];
      }

      memcpy(entry->MainThreadVariables, WorkingSaveEntry->MainThreadVariables,
             64);
      entry->MainThreadDialoguePageId =
          WorkingSaveEntry->MainThreadDialoguePageId;
    }
  }
}

void SaveSystem::WriteSaveFile() {
  // Io::InputStream* stream;
  // IoError err = Io::PhysicalFileStream::Create(SaveFilePath, &stream);

  // stream->Seek(0x3b06, SEEK_SET);  // TODO: Actually save system data

  // for (int i = 0; i < MaxSaveEntries; i++) {
  //  if (QuickSaveEntries[i]->Status == 0) {
  //    stream->Seek(0x2000, SEEK_CUR);
  //  } else {
  //    // TODO: We don't have writing to file...
  //  }
  //}

  // for (int i = 0; i < MaxSaveEntries; i++) {
  //  if (FullSaveEntries[i]->Status == 0) {
  //    stream->Seek(0x2000, SEEK_CUR);
  //  } else {
  //    // TODO: We don't have writing to file...
  //  }
  //}
}

uint32_t SaveSystem::GetSavePlayTime(SaveType type, int id) {
  switch (type) {
    case SaveFull:
      return ((SaveFileEntry*)FullSaveEntries[id])->PlayTime;
    case SaveQuick:
      return ((SaveFileEntry*)QuickSaveEntries[id])->PlayTime;
    default:
      ImpLog(LL_Error, LC_IO,
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
      ImpLog(LL_Error, LC_IO,
             "Failed to get save flags: unknown save type, returning 0\n");
      return 0;
  }
}

tm SaveSystem::GetSaveDate(SaveType type, int id) {
  switch (type) {
    case SaveFull:
      return ((SaveFileEntry*)FullSaveEntries[id])->SaveDate;
    case SaveQuick:
      return ((SaveFileEntry*)QuickSaveEntries[id])->SaveDate;
    default:
      ImpLog(LL_Error, LC_IO,
             "Failed to get save date: unknown save type, returning empty "
             "timestamp\n");
      return std::tm{};
  }
}

void SaveSystem::SaveMemory() {
  if (WorkingSaveEntry != 0) {
    WorkingSaveEntry->Status = 1;

    WorkingSaveEntry->PlayTime = ScrWork[2304];
    WorkingSaveEntry->SwTitle = ScrWork[2300];

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

void SaveSystem::LoadMemory(SaveType type, int id) {
  SaveFileEntry* entry = 0;
  switch (type) {
    case SaveQuick:
      entry = (SaveFileEntry*)QuickSaveEntries[id];
      break;
    case SaveFull:
      entry = (SaveFileEntry*)FullSaveEntries[id];
      break;
    default:
      ImpLog(LL_Error, LC_IO,
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
        thd->Ip = ScriptBuffers[thd->ScriptBufferId] + entry->MainThreadIp;
        // thd->CallStackDepth = entry->MainThreadCallStackDepth;

        LoadScript(entry->MainThreadReturnBufIds[0],
                   ScrWork[2004 + entry->MainThreadReturnBufIds[0]]);
        thd->CallStackDepth++;
        thd->ReturnScriptBufferIds[0] = entry->MainThreadReturnBufIds[0];
        thd->ReturnAddresses[0] =
            ScriptBuffers[entry->MainThreadReturnBufIds[0]] +
            entry->MainThreadReturnAddresses[0];

        memcpy(thd->Variables, entry->MainThreadVariables, 64);
        thd->DialoguePageId = entry->MainThreadDialoguePageId;
      }
    }
}

uint8_t SaveSystem::GetSaveSatus(SaveType type, int id) {
  switch (type) {
    case SaveQuick:
      return ((SaveFileEntry*)QuickSaveEntries[id])->Status;
    case SaveFull:
      return ((SaveFileEntry*)FullSaveEntries[id])->Status;
    default:
      ImpLog(LL_Error, LC_IO,
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
      ImpLog(LL_Error, LC_IO,
             "Failed to get save title: unknown save type, returning 0\n");
      return 0;
  }
}

uint32_t SaveSystem::GetTipStatus(int tipId) {
  tipId *= 3;
  return ((((GameExtraData[tipId >> 3] & Flbit[tipId & 7]) != 0) |
           (2 * ((Flbit[(tipId + 2) & 7] & GameExtraData[(tipId + 2) >> 3]) !=
                 0))) &
          0xFB) |
         (4 *
          ((GameExtraData[(tipId + 1) >> 3] & Flbit[(tipId + 1) & 7]) != 0));
}

void SaveSystem::SetTipStatus(int tipId, bool isLocked, bool isUnread,
                              bool isNew) {}
void SaveSystem::GetReadMessagesCount(int* totalMessageCount,
                                      int* readMessageCount) {}

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

}  // namespace CHLCC
}  // namespace Impacto