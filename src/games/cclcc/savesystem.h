#pragma once

#include "../../data/savesystem.h"

namespace Impacto {
namespace CCLCC {

using namespace Impacto::SaveSystem;

class SaveFileEntry : public SaveFileEntryBase {
 public:
  uint8_t FlagWorkScript1[50];   // 50 bytes from &FlagWork[50]
  uint8_t FlagWorkScript2[100];  // 100 bytes from &FlagWork[300]
  int ScrWorkScript1[600];       // 2400 bytes from &ScrWork[1000]
  int ScrWorkScript2[3000];      // 12000 bytes from &ScrWork[4300]
  uint8_t MapLoadData[0x6ac8];
  uint8_t YesNoData[0x54];
};

class SaveSystem : public SaveSystemBase {
 public:
  SaveError MountSaveFile();
  void SaveMemory();
  void LoadMemory(SaveType type, int id);
  void FlushWorkingSaveEntry(SaveType type, int id);
  void WriteSaveFile();
  uint32_t GetSavePlayTime(SaveType type, int id);
  uint8_t GetSaveFlags(SaveType type, int id);
  tm GetSaveDate(SaveType type, int id);
  uint8_t GetSaveSatus(SaveType type, int id);
  int GetSaveTitle(SaveType type, int id);
  uint32_t GetTipStatus(int tipId);
  void SetTipStatus(int tipId, bool isLocked, bool isUnread, bool isNew);
  void GetReadMessagesCount(int* totalMessageCount, int* readMessageCount);
  void GetViewedEVsCount(int* totalEVCount, int* viewedEVCount);
  void GetEVStatus(int evId, int* totalVariations, int* viewedVariations);
  bool GetEVVariationIsUnlocked(int evId, int variationIdx);
  bool GetBgmFlag(int id);
  void SetCheckpointId(int id);

 private:
  uint8_t GameExtraData[1024];
  uint8_t MessageFlags[10000];
  bool EVFlags[1200];
  uint8_t BGMFlags[100];
};

}  // namespace CCLCC
}  // namespace Impacto