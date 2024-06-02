#include "savesystem.h"

#include <time.h>
#include "../../io/physicalfilestream.h"
#include "../../mem.h"
#include "../../vm/vm.h"
#include "../../profile/data/savesystem.h"
#include "../../profile/scriptvars.h"

namespace Impacto {
namespace MO6TW {

using namespace Impacto::Vm;
using namespace Impacto::Profile::SaveSystem;
using namespace Impacto::Profile::ScriptVars;

SaveFileEntry* WorkingSaveEntry = 0;

SaveError SaveSystem::MountSaveFile() {
  Io::PhysicalFileStream* stream;
  Io::Stream* instream;
  IoError err = Io::PhysicalFileStream::Create(SaveFilePath, &instream);
  switch (err) {
    case IoError_NotFound:
      return SaveNotFound;
    case IoError_Fail:
    case IoError_Eof:
      return SaveCorrupted;
    case IoError_OK:
      break;
  };
  stream = (Io::PhysicalFileStream*)instream;

  WorkingSaveEntry = new SaveFileEntry();

  stream->Seek(0x14, SEEK_SET);

  Io::ReadArrayLE<uint8_t>(&FlagWork[100], stream, 50);
  Io::ReadArrayLE<uint8_t>(&FlagWork[460], stream, 40);
  Io::ReadArrayLE<uint8_t>((uint8_t*)&ScrWork[600], stream, 1600);

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

  stream->Seek(0x3336, SEEK_SET);
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
    t.tm_mon = saveMonth - 1;
    t.tm_year = saveYear + 100;
    QuickSaveEntries[i]->SaveDate = t;

    Io::ReadLE<uint16_t>(stream);
    QuickSaveEntries[i]->PlayTime = Io::ReadLE<uint32_t>(stream);
    QuickSaveEntries[i]->SwTitle = Io::ReadLE<uint16_t>(stream);
    Io::ReadLE<uint8_t>(stream);
    stream->Seek(31, SEEK_CUR);
    Io::ReadArrayLE<uint8_t>(
        ((SaveFileEntry*)QuickSaveEntries[i])->FlagWorkScript1, stream, 50);
    Io::ReadArrayLE<uint8_t>(
        ((SaveFileEntry*)QuickSaveEntries[i])->FlagWorkScript2, stream, 100);
    Io::ReadArrayLE<int>(((SaveFileEntry*)QuickSaveEntries[i])->ScrWorkScript1,
                         stream, 300);
    Io::ReadArrayLE<int>(((SaveFileEntry*)QuickSaveEntries[i])->ScrWorkScript2,
                         stream, 1100);
    QuickSaveEntries[i]->MainThreadExecPriority = Io::ReadLE<uint32_t>(stream);
    QuickSaveEntries[i]->MainThreadGroupId = Io::ReadLE<uint32_t>(stream);
    QuickSaveEntries[i]->MainThreadWaitCounter = Io::ReadLE<uint32_t>(stream);
    QuickSaveEntries[i]->MainThreadScriptParam = Io::ReadLE<uint32_t>(stream);
    QuickSaveEntries[i]->MainThreadIp = Io::ReadLE<uint32_t>(stream);
    QuickSaveEntries[i]->MainThreadLoopCounter = Io::ReadLE<uint32_t>(stream);
    QuickSaveEntries[i]->MainThreadLoopAdr = Io::ReadLE<uint32_t>(stream);
    QuickSaveEntries[i]->MainThreadCallStackDepth =
        Io::ReadLE<uint32_t>(stream);
    for (int j = 0; j < 8; j++) {
      QuickSaveEntries[i]->MainThreadReturnIds[j] =
          Io::ReadLE<uint32_t>(stream);
      QuickSaveEntries[i]->MainThreadReturnBufIds[j] =
          Io::ReadLE<uint32_t>(stream);
    }
    Io::ReadArrayLE<int>(QuickSaveEntries[i]->MainThreadVariables, stream, 16);
    // QuickSaveEntries[i]->MainThreadDialoguePageId =
    //    Io::ReadLE<uint32_t>(stream);
    stream->Seek(204, SEEK_CUR);
  }

  stream->Seek(0x3028, SEEK_CUR);

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
    t.tm_mon = saveMonth - 1;
    t.tm_year = saveYear + 100;
    FullSaveEntries[i]->SaveDate = t;
    Io::ReadLE<uint16_t>(stream);
    FullSaveEntries[i]->PlayTime = Io::ReadLE<uint32_t>(stream);
    FullSaveEntries[i]->SwTitle = Io::ReadLE<uint16_t>(stream);
    Io::ReadLE<uint8_t>(stream);
    stream->Seek(31, SEEK_CUR);
    Io::ReadArrayLE<uint8_t>(
        ((SaveFileEntry*)FullSaveEntries[i])->FlagWorkScript1, stream, 50);
    Io::ReadArrayLE<uint8_t>(
        ((SaveFileEntry*)FullSaveEntries[i])->FlagWorkScript2, stream, 100);
    Io::ReadArrayLE<int>(((SaveFileEntry*)FullSaveEntries[i])->ScrWorkScript1,
                         stream, 300);
    Io::ReadArrayLE<int>(((SaveFileEntry*)FullSaveEntries[i])->ScrWorkScript2,
                         stream, 1100);
    FullSaveEntries[i]->MainThreadExecPriority = Io::ReadLE<uint32_t>(stream);
    FullSaveEntries[i]->MainThreadGroupId = Io::ReadLE<uint32_t>(stream);
    FullSaveEntries[i]->MainThreadWaitCounter = Io::ReadLE<uint32_t>(stream);
    FullSaveEntries[i]->MainThreadScriptParam = Io::ReadLE<uint32_t>(stream);
    FullSaveEntries[i]->MainThreadIp = Io::ReadLE<uint32_t>(stream);
    FullSaveEntries[i]->MainThreadLoopCounter = Io::ReadLE<uint32_t>(stream);
    FullSaveEntries[i]->MainThreadLoopAdr = Io::ReadLE<uint32_t>(stream);
    FullSaveEntries[i]->MainThreadCallStackDepth = Io::ReadLE<uint32_t>(stream);
    for (int j = 0; j < 8; j++) {
      FullSaveEntries[i]->MainThreadReturnIds[j] = Io::ReadLE<uint32_t>(stream);
      FullSaveEntries[i]->MainThreadReturnBufIds[j] =
          Io::ReadLE<uint32_t>(stream);
    }
    Io::ReadArrayLE<int>(FullSaveEntries[i]->MainThreadVariables, stream, 16);
    // FullSaveEntries[i]->MainThreadDialoguePageId =
    // Io::ReadLE<uint32_t>(stream);
    stream->Seek(204, SEEK_CUR);
  }

  stream->~PhysicalFileStream();

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

      std::time_t t = std::time(0);
      entry->SaveDate = *std::localtime(&t);
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
        entry->MainThreadReturnIds[j] =
            WorkingSaveEntry->MainThreadReturnIds[j];
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
  Io::PhysicalFileStream* stream;
  Io::Stream* instream;
  IoError err = Io::PhysicalFileStream::CreateWrite(SaveFilePath, &instream);
  auto err1 = SDL_GetError();
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_IO, "Failed to create save file, SDL error: %s\n",
           err1);
    return;
  }
  stream = (Io::PhysicalFileStream*)instream;

  stream->Seek(0xbc2, SEEK_SET);
  stream->Write(&BGMFlags, sizeof(uint8_t), 100);

  stream->Seek(0xc26, SEEK_SET);
  stream->Write(&MessageFlags, sizeof(uint8_t), 10000);

  stream->Seek(0x3336, SEEK_SET);
  stream->Write(&GameExtraData, sizeof(uint8_t), 1024);

  stream->Seek(0x3b06, SEEK_SET);  // TODO: Actually save system data

  for (int i = 0; i < MaxSaveEntries; i++) {
    if (QuickSaveEntries[i]->Status == 0) {
      stream->Seek(0x1814, SEEK_CUR);
    } else {
      // TODO: We don't have writing to file...

      auto err =
          stream->Write(&QuickSaveEntries[i]->Status, sizeof(uint8_t), 1);
      auto err1 = SDL_GetError();
      if (err != IoError_OK) {
        ImpLog(LL_Error, LC_IO,
               "Failed to write save entry to file, SDL error: %s\n", err1);
        return;
      }
      // TODO: Add error checking
      stream->Write(&QuickSaveEntries[i]->Checksum, sizeof(uint16_t), 1);
      stream->Seek(1, SEEK_CUR);
      uint8_t mon = QuickSaveEntries[i]->SaveDate.tm_mon + 1;
      stream->Write(&mon, sizeof(uint8_t), 1);
      stream->Write(&QuickSaveEntries[i]->SaveDate.tm_mday, sizeof(uint8_t), 1);
      stream->Write(&QuickSaveEntries[i]->SaveDate.tm_hour, sizeof(uint8_t), 1);
      stream->Write(&QuickSaveEntries[i]->SaveDate.tm_min, sizeof(uint8_t), 1);
      uint8_t year = QuickSaveEntries[i]->SaveDate.tm_year - 100;
      stream->Write(&year, sizeof(uint8_t), 1);
      stream->Write(&QuickSaveEntries[i]->SaveDate.tm_sec, sizeof(uint8_t), 1);
      stream->Seek(2, SEEK_CUR);
      stream->Write(&QuickSaveEntries[i]->PlayTime, sizeof(uint32_t), 1);
      stream->Write(&QuickSaveEntries[i]->SwTitle, sizeof(uint16_t), 1);
      stream->Seek(32, SEEK_CUR);
      stream->Write(((SaveFileEntry*)QuickSaveEntries[i])->FlagWorkScript1,
                    sizeof(uint8_t), 50);
      stream->Write(((SaveFileEntry*)QuickSaveEntries[i])->FlagWorkScript2,
                    sizeof(uint8_t), 100);
      stream->Write(((SaveFileEntry*)QuickSaveEntries[i])->ScrWorkScript1,
                    sizeof(int), 300);
      stream->Write(((SaveFileEntry*)QuickSaveEntries[i])->ScrWorkScript2,
                    sizeof(int), 1100);
      stream->Write(&QuickSaveEntries[i]->MainThreadExecPriority,
                    sizeof(uint32_t), 1);
      stream->Write(&QuickSaveEntries[i]->MainThreadGroupId, sizeof(uint32_t),
                    1);
      stream->Write(&QuickSaveEntries[i]->MainThreadWaitCounter,
                    sizeof(uint32_t), 1);
      stream->Write(&QuickSaveEntries[i]->MainThreadScriptParam,
                    sizeof(uint32_t), 1);
      stream->Write(&QuickSaveEntries[i]->MainThreadIp, sizeof(uint32_t), 1);
      stream->Write(&QuickSaveEntries[i]->MainThreadLoopCounter,
                    sizeof(uint32_t), 1);
      stream->Write(&QuickSaveEntries[i]->MainThreadLoopAdr, sizeof(uint32_t),
                    1);
      stream->Write(&QuickSaveEntries[i]->MainThreadCallStackDepth,
                    sizeof(uint32_t), 1);
      for (int j = 0; j < 8; j++) {
        stream->Write(&QuickSaveEntries[i]->MainThreadReturnIds[j],
                      sizeof(uint32_t), 1);
        stream->Write(&QuickSaveEntries[i]->MainThreadReturnBufIds[j],
                      sizeof(uint32_t), 1);
      }
      stream->Write(&QuickSaveEntries[i]->MainThreadVariables, sizeof(int), 16);
      stream->Seek(204, SEEK_CUR);
    }
  }

  stream->Seek(0x3028, SEEK_CUR);

  for (int i = 0; i < MaxSaveEntries; i++) {
    if (FullSaveEntries[i]->Status == 0) {
      stream->Seek(0x1814, SEEK_CUR);
    } else {
      // TODO: We don't have writing to file...

      stream->Write(&FullSaveEntries[i]->Status, sizeof(uint8_t), 1);
      stream->Write(&FullSaveEntries[i]->Checksum, sizeof(uint16_t), 1);
      stream->Seek(1, SEEK_CUR);
      uint8_t mon = FullSaveEntries[i]->SaveDate.tm_mon + 1;
      stream->Write(&mon, sizeof(uint8_t), 1);
      stream->Write(&FullSaveEntries[i]->SaveDate.tm_mday, sizeof(uint8_t), 1);
      stream->Write(&FullSaveEntries[i]->SaveDate.tm_hour, sizeof(uint8_t), 1);
      stream->Write(&FullSaveEntries[i]->SaveDate.tm_min, sizeof(uint8_t), 1);
      uint8_t year = FullSaveEntries[i]->SaveDate.tm_year - 100;
      stream->Write(&year, sizeof(uint8_t), 1);
      stream->Write(&FullSaveEntries[i]->SaveDate.tm_sec, sizeof(uint8_t), 1);
      stream->Seek(2, SEEK_CUR);
      stream->Write(&FullSaveEntries[i]->PlayTime, sizeof(uint32_t), 1);
      stream->Write(&FullSaveEntries[i]->SwTitle, sizeof(uint16_t), 1);
      stream->Seek(32, SEEK_CUR);
      stream->Write(((SaveFileEntry*)FullSaveEntries[i])->FlagWorkScript1,
                    sizeof(uint8_t), 50);
      stream->Write(((SaveFileEntry*)FullSaveEntries[i])->FlagWorkScript2,
                    sizeof(uint8_t), 100);
      stream->Write(((SaveFileEntry*)FullSaveEntries[i])->ScrWorkScript1,
                    sizeof(int), 300);
      stream->Write(((SaveFileEntry*)FullSaveEntries[i])->ScrWorkScript2,
                    sizeof(int), 1100);
      stream->Write(&FullSaveEntries[i]->MainThreadExecPriority,
                    sizeof(uint32_t), 1);
      stream->Write(&FullSaveEntries[i]->MainThreadGroupId, sizeof(uint32_t),
                    1);
      stream->Write(&FullSaveEntries[i]->MainThreadWaitCounter,
                    sizeof(uint32_t), 1);
      stream->Write(&FullSaveEntries[i]->MainThreadScriptParam,
                    sizeof(uint32_t), 1);
      stream->Write(&FullSaveEntries[i]->MainThreadIp, sizeof(uint32_t), 1);
      stream->Write(&FullSaveEntries[i]->MainThreadLoopCounter,
                    sizeof(uint32_t), 1);
      stream->Write(&FullSaveEntries[i]->MainThreadLoopAdr, sizeof(uint32_t),
                    1);
      stream->Write(&FullSaveEntries[i]->MainThreadCallStackDepth,
                    sizeof(uint32_t), 1);
      for (int j = 0; j < 8; j++) {
        stream->Write(&FullSaveEntries[i]->MainThreadReturnIds[j],
                      sizeof(uint32_t), 1);
        stream->Write(&FullSaveEntries[i]->MainThreadReturnBufIds[j],
                      sizeof(uint32_t), 1);
      }
      stream->Write(&FullSaveEntries[i]->MainThreadVariables, sizeof(int), 16);
      // FullSaveEntries[i]->MainThreadDialoguePageId =
      // Io::ReadLE<uint32_t>(stream);
      stream->Seek(204, SEEK_CUR);
    }
  }

  stream->~PhysicalFileStream();
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
          (uint32_t)(thd->Ip - ScriptBuffers[thd->ScriptBufferId]);
      WorkingSaveEntry->MainThreadCallStackDepth = thd->CallStackDepth;

      for (size_t j = 0; j < thd->CallStackDepth; j++) {
        WorkingSaveEntry->MainThreadReturnIds[j] =
            (uint32_t)(thd->ReturnAddresses[j] -
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
            entry->MainThreadReturnIds[0];

        memcpy(thd->Variables, entry->MainThreadVariables, 64);
        thd->DialoguePageId = entry->MainThreadDialoguePageId;

        // Tell the script side of save loading that we already loaded the
        // needed scripts ourselves
        // TODO: Figure out a better way of loading in general
        ScrWork[SW_SVSCRNO1] = 65535;
        ScrWork[SW_SVSCRNO2] = 65535;
        ScrWork[SW_SVSCRNO3] = 65535;
        ScrWork[SW_SVSCRNO4] = 65535;
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
             "Failed to read play time: Unknown save type, returning 0\n");
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
             "Failed to read save flags: Unknown save type, returning 0\n");
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
             "Failed to read save date: Unknown save type, returning empty "
             "time\n");
      return std::tm{};
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
             "Failed to read save status: Unknown save type, returning 0\n");
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
             "Failed to read save title: Unknown save type, returning 0\n");
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

void SaveSystem::GetReadMessagesCount(int* totalMessageCount,
                                      int* readMessageCount) {
  for (int i = 0; i < StoryScriptCount; i++) {
    auto record = ScriptMessageData[StoryScriptIDs[i]];
    *totalMessageCount += record.LineCount;
    for (size_t j = 0; j < record.LineCount; j++) {
      *readMessageCount +=
          ((*(uint8_t*)(MessageFlags + ((record.SaveDataOffset + i) >> 3)) &
            Flbit[(record.SaveDataOffset + i) & 7]) != 0);
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

}  // namespace MO6TW
}  // namespace Impacto