#pragma once

#include "../../data/savesystem.h"

namespace Impacto {
namespace MO6TW {

using namespace Impacto::SaveSystem;

class SaveFileEntry : public SaveFileEntryBase {
 public:
  uint8_t FlagWorkScript1[50];   // 50 bytes from &FlagWork[50]
  uint8_t FlagWorkScript2[100];  // 100 bytes from &FlagWork[300]
  int ScrWorkScript1[300];       // 1200 bytes from &ScrWork[300]
  int ScrWorkScript2[1300];      // 5200 bytes from &ScrWork[2300]
};

class SaveSystem : public SaveSystemBase {
 public:
  SaveError MountSaveFile();
  void SaveMemory();
  void LoadMemory(SaveType type, int id);
  void FlushWorkingSaveEntry(SaveType type, int id);
  void WriteSaveFile();
  uint8_t GetSaveSatus(SaveType type, int id);
  int GetSaveTitle(SaveType type, int id);
  uint32_t GetTipStatus(int tipId);
  void SetTipStatus(int tipId, bool isLocked, bool isUnread, bool isNew);
  void GetReadMessagesCount(int* totalMessageCount, int* readMessageCount);
  void GetViewedEVsCount(int* totalEVCount, int* viewedEVCount);
  bool GetBgmFlag(int id);

 private:
  uint8_t GameExtraData[1024];
  uint8_t MessageFlags[10000];
  bool EVFlags[1200];
  uint8_t BGMFlags[100];
};

}  // namespace MO6TW
}  // namespace Impacto