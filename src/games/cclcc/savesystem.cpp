#include "savesystem.h"

#include "../../io/stream.h"
#include "../../io/physicalfilestream.h"
#include "../../mem.h"
#include "../../vm/vm.h"
#include "../../profile/data/savesystem.h"
#include "../../profile/scriptvars.h"
#include "../../profile/vm.h"
#include "../../ui/mapsystem.h"

#include <cstdint>
#include <ctime>

namespace Impacto {
namespace CCLCC {

using namespace Impacto::Vm;
using namespace Impacto::Profile::SaveSystem;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::Vm;

SaveFileEntry* WorkingSaveEntry = 0;

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

  WorkingSaveEntry = new SaveFileEntry();

  stream->Seek(0x14, SEEK_SET);

  Io::ReadArrayLE<uint8_t>(&FlagWork[100], stream, 50);
  Io::ReadArrayLE<uint8_t>(&FlagWork[460], stream, 40);
  Io::ReadArrayLE<int>(&ScrWork[600], stream, 400);

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

  stream->Seek(0x387c, SEEK_SET);  // TODO: Actually load system data

  Impacto::SaveSystem::SaveFileEntryBase** entryTypes[] = {FullSaveEntries,
                                                           QuickSaveEntries};

  for (auto& entryArray : entryTypes) {
    int64_t saveDataPos = stream->Position;
    for (int i = 0; i < MaxSaveEntries; i++) {
      int64_t startPos = stream->Position;
      assert(startPos - saveDataPos == 0xd888 * i);
      entryArray[i] = new SaveFileEntry();

      entryArray[i]->Status = Io::ReadLE<uint16_t>(stream);
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
      stream->Seek(12032, SEEK_CUR);
    }
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

      memcpy(entry->FlagWorkScript1, &FlagWork[50], 50);
      memcpy(entry->FlagWorkScript2, &FlagWork[300], 100);
      memcpy(entry->ScrWorkScript1, &ScrWork[1000], 2400);
      memcpy(entry->ScrWorkScript2, &ScrWork[4300], 12000);

      entry->MainThreadExecPriority = WorkingSaveEntry->MainThreadExecPriority;
      entry->MainThreadWaitCounter = WorkingSaveEntry->MainThreadWaitCounter;
      entry->MainThreadScriptParam = WorkingSaveEntry->MainThreadScriptParam;
      entry->MainThreadGroupId = WorkingSaveEntry->MainThreadGroupId;
      entry->MainThreadGroupId = WorkingSaveEntry->MainThreadGroupId;
      entry->MainThreadIp = WorkingSaveEntry->MainThreadIp;
      entry->MainThreadCallStackDepth =
          WorkingSaveEntry->MainThreadCallStackDepth;

      for (int j = 0; j < 8; j++) {
        entry->MainThreadReturnIds[j] =
            WorkingSaveEntry->MainThreadReturnIds[j];
        entry->MainThreadReturnBufIds[j] =
            WorkingSaveEntry->MainThreadReturnBufIds[j];
      }

      memcpy(entry->MainThreadVariables, WorkingSaveEntry->MainThreadVariables,
             64);
      memcpy(entry->MapLoadData, WorkingSaveEntry->MapLoadData,
             sizeof(entry->MapLoadData));
      entry->MainThreadDialoguePageId =
          WorkingSaveEntry->MainThreadDialoguePageId;
    }
  }
}

void SaveSystem::WriteSaveFile() {
  Io::PhysicalFileStream* stream;
  IoError err = Io::PhysicalFileStream::CreateWrite(
      SaveFilePath, reinterpret_cast<Io::Stream**>(stream));

  stream->Seek(0x387c, SEEK_SET);  // TODO: Actually save system data

  SaveFileEntryBase** entryTypes[] = {FullSaveEntries, QuickSaveEntries};
  for (auto* entryArray : entryTypes) {
    int64_t saveDataPos = stream->Position;
    for (int i = 0; i < MaxSaveEntries; i++) {
      SaveFileEntry* entry = (SaveFileEntry*)entryArray[i];
      if (entry->Status == 0) {
        stream->Seek(0xD888, SEEK_CUR);
      } else {
        // int64_t startPos = stream->Position;
        // assert(startPos - saveDataPos == 0xd888 * i);
        // uint8_t buffer[0xd888] = {0};
        // uint64_t bufferPos = 0;
        // buffer[bufferPos] stream->Write(&entry->Status,
        // sizeof(entry->Status),
        //                                 1);
        // stream->Write(&entry->Checksum, sizeof(entry->Checksum), 1);
        // Io::ReadLE<uint32_t>(stream);
        // uint16_t saveYear = Io::ReadLE<uint16_t>(stream);
        // uint8_t saveDay = Io::ReadLE<uint8_t>(stream);
        // uint8_t saveMonth = Io::ReadLE<uint8_t>(stream);
        // uint8_t saveSecond = Io::ReadLE<uint8_t>(stream);
        // uint8_t saveMinute = Io::ReadLE<uint8_t>(stream);
        // uint8_t saveHour = Io::ReadLE<uint8_t>(stream);
        // std::tm t{};
        // t.tm_sec = saveSecond;
        // t.tm_min = saveMinute;
        // t.tm_hour = saveHour;
        // t.tm_mday = saveDay;
        // t.tm_mon = saveMonth - 1;
        // t.tm_year = saveYear - 1900;
        // entryArray[i]->SaveDate = t;

        // Io::ReadLE<uint8_t>(stream);
        // entryArray[i]->PlayTime = Io::ReadLE<uint32_t>(stream);
        // entryArray[i]->SwTitle = Io::ReadLE<uint32_t>(stream);
        // Io::ReadLE<uint32_t>(stream);
        // entryArray[i]->Flags = Io::ReadLE<uint8_t>(stream);
        // stream->Seek(7, SEEK_CUR);
        // entryArray[i]->SaveType = Io::ReadLE<uint32_t>(stream);
        // assert(stream->Position - startPos == 0x28);
        // stream->Seek(0x58, SEEK_CUR);
        // Io::ReadArrayLE<uint8_t>(
        //     ((SaveFileEntry*)entryArray[i])->FlagWorkScript1, stream, 50);
        // assert(stream->Position - startPos == 178);
        // Io::ReadArrayLE<uint8_t>(
        //     ((SaveFileEntry*)entryArray[i])->FlagWorkScript2, stream, 100);
        // Io::ReadLE<uint16_t>(stream);
        // assert(stream->Position - startPos == 280);
        // Io::ReadArrayLE<int>(((SaveFileEntry*)entryArray[i])->ScrWorkScript1,
        //                      stream, 600);
        // assert(stream->Position - startPos == 2680);
        // Io::ReadArrayLE<int>(((SaveFileEntry*)entryArray[i])->ScrWorkScript2,
        //                      stream, 3000);

        // assert(stream->Position - startPos == 0x3958);
        // entryArray[i]->MainThreadExecPriority = Io::ReadLE<uint32_t>(stream);
        // entryArray[i]->MainThreadGroupId = Io::ReadLE<uint32_t>(stream);
        // entryArray[i]->MainThreadWaitCounter = Io::ReadLE<uint32_t>(stream);
        // entryArray[i]->MainThreadScriptParam = Io::ReadLE<uint32_t>(stream);
        // entryArray[i]->MainThreadIp = Io::ReadLE<uint32_t>(stream);
        // entryArray[i]->MainThreadLoopCounter = Io::ReadLE<uint32_t>(stream);
        // entryArray[i]->MainThreadLoopAdr = Io::ReadLE<uint32_t>(stream);
        // entryArray[i]->MainThreadCallStackDepth =
        // Io::ReadLE<uint32_t>(stream); for (int j = 0; j < 8; j++) {
        //   entryArray[i]->MainThreadReturnIds[j] =
        //   Io::ReadLE<uint32_t>(stream);
        // }
        // for (int j = 0; j < 8; j++) {
        //   entryArray[i]->MainThreadReturnBufIds[j] =
        //       Io::ReadLE<uint32_t>(stream);
        // }
        // Io::ReadLE<uint32_t>(stream);
        // assert(stream->Position - startPos == 0x39bc);
        // entryArray[i]->MainThreadScriptBufferId =
        // Io::ReadLE<uint32_t>(stream);
        // Io::ReadArrayBE<int>(entryArray[i]->MainThreadVariables, stream, 16);
        // entryArray[i]->MainThreadDialoguePageId =
        // Io::ReadLE<uint32_t>(stream); assert(stream->Position - startPos ==
        // 0x3a04); stream->Seek(1212, SEEK_CUR); assert(stream->Position -
        // startPos == 0x3ec0);
        // Io::ReadArrayLE<uint8_t>(((SaveFileEntry*)entryArray[i])->MapLoadData,
        //                          stream, 0x6ac8);
        // stream->Seek(12032, SEEK_CUR);
      }
    }
  }
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
  // TODO: Sys save data

  if (WorkingSaveEntry != 0) {
    WorkingSaveEntry->Status = 1;

    WorkingSaveEntry->PlayTime = ScrWork[2304];
    WorkingSaveEntry->SwTitle = ScrWork[2300];

    memcpy(WorkingSaveEntry->FlagWorkScript1, &FlagWork[50], 50);
    memcpy(WorkingSaveEntry->FlagWorkScript2, &FlagWork[300], 100);
    memcpy(WorkingSaveEntry->ScrWorkScript1, &ScrWork[1000], 2400);
    memcpy(WorkingSaveEntry->ScrWorkScript2, &ScrWork[4300], 12000);
    int threadId = ScrWork[SW_MAINTHDP];
    Sc3VmThread* thd = &ThreadPool[threadId & 0x7FFFFFFF];
    if (thd->GroupId - 5 < 3) {
      WorkingSaveEntry->MainThreadCallStackDepth = thd->CallStackDepth;

      for (int i = 0; i < thd->CallStackDepth; i++) {
        WorkingSaveEntry->MainThreadReturnBufIds[i] =
            thd->ReturnScriptBufferIds[i];
        WorkingSaveEntry->MainThreadReturnIds[i] = thd->ReturnIds[i];
      }
      for (size_t i = 0; i < thd->CallStackDepth; i++) {
        WorkingSaveEntry->MainThreadReturnBufIds[i] =
            thd->ReturnScriptBufferIds[i];
        WorkingSaveEntry->MainThreadReturnIds[i] = thd->ReturnIds[i];
        ;
      }
      memcpy(WorkingSaveEntry->MainThreadVariables, thd->Variables, 64);
      WorkingSaveEntry->MainThreadDialoguePageId = thd->DialoguePageId;
    }
    int size = 0;
    UI::MapSystem::MapSave(WorkingSaveEntry->MapLoadData, size);
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
      memcpy(&ScrWork[1000], entry->ScrWorkScript1, 2400);
      memcpy(&ScrWork[4300], entry->ScrWorkScript2, 12000);
      int dataSize = 0;
      UI::MapSystem::MapLoad(entry->MapLoadData, dataSize);
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
      ScrWork[SW_SVBGNO1] = ScrWork[SW_BG1NO + ScrWorkBgStructSize * 0];
      ScrWork[SW_SVBGNO1 + 1] = ScrWork[SW_BG1NO + ScrWorkBgStructSize * 1];
      ScrWork[SW_SVBGNO1 + 2] = ScrWork[SW_BG1NO + ScrWorkBgStructSize * 2];
      ScrWork[SW_SVBGNO1 + 3] = ScrWork[SW_BG1NO + ScrWorkBgStructSize * 3];
      ScrWork[SW_SVBGNO1 + 4] = ScrWork[SW_BG1NO + ScrWorkBgStructSize * 4];
      ScrWork[SW_SVBGNO1 + 5] = ScrWork[SW_BG1NO + ScrWorkBgStructSize * 5];
      ScrWork[SW_SVBGNO1 + 6] = ScrWork[SW_BG1NO + ScrWorkBgStructSize * 6];
      ScrWork[SW_SVBGNO1 + 7] = ScrWork[SW_BG1NO + ScrWorkBgStructSize * 7];
      ScrWork[SW_SVCHANO1] = ScrWork[SW_CHA1NO + ScrWorkChaStructSize * 0];
      ScrWork[SW_SVCHANO1 + 1] = ScrWork[SW_CHA1NO + ScrWorkChaStructSize * 1];
      ScrWork[SW_SVCHANO1 + 2] = ScrWork[SW_CHA1NO + ScrWorkChaStructSize * 2];
      ScrWork[SW_SVCHANO1 + 3] = ScrWork[SW_CHA1NO + ScrWorkChaStructSize * 3];
      ScrWork[SW_SVCHANO1 + 4] = ScrWork[SW_CHA1NO + ScrWorkChaStructSize * 4];
      ScrWork[SW_SVCHANO1 + 5] = ScrWork[SW_CHA1NO + ScrWorkChaStructSize * 5];
      ScrWork[SW_SVCHANO1 + 6] = ScrWork[SW_CHA1NO + ScrWorkChaStructSize * 6];
      ScrWork[SW_SVCHANO1 + 7] = ScrWork[SW_CHA1NO + ScrWorkChaStructSize * 7];

      int threadId = ScrWork[SW_MAINTHDP];
      Sc3VmThread* thd = &ThreadPool[threadId & 0x7FFFFFFF];

      // Load scripts
      if (ScrWork[SW_SVSCRNO1] != 65535) LoadScript(2, ScrWork[SW_SVSCRNO1]);
      if (ScrWork[SW_SVSCRNO2] != 65535) LoadScript(3, ScrWork[SW_SVSCRNO2]);
      if (ScrWork[SW_SVSCRNO4] != 65535) LoadScript(5, ScrWork[SW_SVSCRNO4]);

      ScrWork[SW_SVSCRNO1] = 65535;
      ScrWork[SW_SVSCRNO2] = 65535;
      ScrWork[SW_SVSCRNO4] = 65535;

      if (thd->GroupId - 5 < 3) {
        thd->ExecPriority = entry->MainThreadExecPriority;
        thd->WaitCounter = entry->MainThreadWaitCounter;
        thd->ScriptParam = entry->MainThreadScriptParam;
        thd->GroupId = entry->MainThreadGroupId;
        thd->ScriptBufferId = entry->MainThreadScriptBufferId;
        thd->Ip =
            ScriptGetRetAddress(ScriptBuffers[entry->MainThreadScriptBufferId],
                                entry->MainThreadIp);
        thd->CallStackDepth = entry->MainThreadCallStackDepth;

        for (int i = 0; i < thd->CallStackDepth; i++) {
          thd->ReturnScriptBufferIds[i] = entry->MainThreadReturnBufIds[i];
          thd->ReturnIds[i] = entry->MainThreadReturnIds[i];
        }

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

void SaveSystem::SetCheckpointId(int id) {
  if (WorkingSaveEntry != nullptr) WorkingSaveEntry->MainThreadIp = id;
}

}  // namespace CCLCC
}  // namespace Impacto