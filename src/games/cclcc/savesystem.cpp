#include "savesystem.h"

#include "../../io/stream.h"
#include "../../io/physicalfilestream.h"
#include "../../mem.h"
#include "../../vm/vm.h"
#include "../../profile/data/savesystem.h"
#include "../../profile/scriptvars.h"
#include "../../profile/vm.h"
#include "../../ui/mapsystem.h"
#include "yesnotrigger.h"

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
  Io::ReadArrayLE<int>(&ScrWork[1600], stream, 400);
  Io::ReadArrayLE<int>(&ScrWork[2000], stream, 100);

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
      stream->Seek(67380, SEEK_CUR);
    }
  }
  delete stream;
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
      *entry = *WorkingSaveEntry;
      time_t rawtime;
      time(&rawtime);
      entry->SaveDate = *localtime(&rawtime);
    }
  }
}

void SaveSystem::WriteSaveFile() {
  Io::Stream* stream;
  IoError err = Io::PhysicalFileStream::CreateWrite(SaveFilePath, &stream);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_IO, "Failed to open save file for writing\n");
    return;
  }

  stream->Seek(0x14, SEEK_SET);

  Io::WriteArrayLE<uint8_t>(&FlagWork[100], stream, 50);
  Io::WriteArrayLE<uint8_t>(&FlagWork[460], stream, 40);
  Io::WriteArrayLE<int>(&ScrWork[1600], stream, 400);
  Io::WriteArrayLE<int>(&ScrWork[2000], stream, 100);

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
        stream->Seek(67380, SEEK_CUR);
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
      UI::MapSystem::MapLoad(entry->MapLoadData);
      CCLCC::YesNoTrigger::YesNoTriggerPtr->Load(entry->YesNoData);
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
        ScrWork[SW_SVCHANO1 + i] =
            ScrWork[SW_CHA1NO + i * ScrWorkChaStructSize];
      }

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
  uint8_t lockStatus = (GameExtraData[tipId >> 3] & Flbit[tipId & 7]) != 0;
  uint8_t unreadStatus =
      (GameExtraData[(tipId + 2) >> 3] & Flbit[(tipId + 2) & 7]) != 0;
  uint8_t newStatus =
      (GameExtraData[(tipId + 1) >> 3] & Flbit[(tipId + 1) & 7]) != 0;
  return (lockStatus | (2 * unreadStatus) & 0xFB) | (4 * newStatus);
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
    GameExtraData[(tipId + 1) >> 3] &= ~(Flbit[(tipId + 1) & 7]);
  } else {
    GameExtraData[(tipId + 1) >> 3] |= Flbit[(tipId + 1) & 7];
  }
  if (isNew) {
    GameExtraData[(tipId + 2) >> 3] &= ~(Flbit[(tipId + 2) & 7]);
  } else {
    GameExtraData[(tipId + 2) >> 3] |= Flbit[(tipId + 2) & 7];
  }
}

void SaveSystem::SetLineRead(int scriptId, int lineId) {
  if (scriptId >= StoryScriptCount) return;

  int offset =
      ScriptMessageData[StoryScriptIDs[scriptId]].SaveDataOffset + lineId;
  if (offset == 0xFFFFFFFF) return;

  // TODO: update some ScrWorks (2003, 2005 & 2006)

  MessageFlags[offset >> 3] |= Flbit[offset & 0b111];
}

bool SaveSystem::IsLineRead(int scriptId, int lineId) {
  if (scriptId >= StoryScriptCount) return false;

  uint32_t offset =
      ScriptMessageData[StoryScriptIDs[scriptId]].SaveDataOffset + lineId;
  uint8_t flbit = Flbit[offset & 0b111];
  uint8_t viewed = MessageFlags[offset >> 3];

  return (bool)(flbit & viewed);
}

void SaveSystem::GetReadMessagesCount(int* totalMessageCount,
                                      int* readMessageCount) {
  *totalMessageCount = 0;
  *readMessageCount = 0;

  for (int scriptId = 0; scriptId < StoryScriptCount; scriptId++) {
    ScriptMessageDataPair script = ScriptMessageData[StoryScriptIDs[scriptId]];
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
  if (WorkingSaveEntry != nullptr) WorkingSaveEntry->MainThreadIp = id;
}

}  // namespace CCLCC
}  // namespace Impacto