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
  SaveError CreateSaveFile() override { return SaveError::OK; }  // Todo
  SaveError CheckSaveFile() override { return SaveError::OK; }  // Todo
  SaveError MountSaveFile(std::vector<QueuedTexture>& textures) override;

  SaveError LoadSystemData() override;
  void SaveSystemData() override;

  void SaveMemory() override;
  void LoadEntry(SaveType type, int id) override;
  void FlushWorkingSaveEntry(SaveType type, int id, int autoSaveType) override;
  SaveError WriteSaveFile() override;
  uint32_t GetSavePlayTime(SaveType type, int id) override;
  uint8_t GetSaveFlags(SaveType type, int id) override;
  tm const& GetSaveDate(SaveType type, int id) override;
  uint8_t GetSaveStatus(SaveType type, int id) override;
  int GetSaveTitle(SaveType type, int id) override;
  uint32_t GetTipStatus(int tipId) override;
  void SetTipStatus(int tipId, bool isLocked, bool isUnread,
                    bool isNew) override;
  void SetLineRead(int scriptId, int lineId) override;
  bool IsLineRead(int scriptId, int lineId) override;
  void GetReadMessagesCount(int* totalMessageCount,
                            int* readMessageCount) override;
  void GetViewedEVsCount(int* totalEVCount, int* viewedEVCount) override;
  void GetEVStatus(int evId, int* totalVariations,
                   int* viewedVariations) override;
  bool GetEVVariationIsUnlocked(int evId, int variationIdx) override;
  bool GetBgmFlag(int id) override;
  void SetCheckpointId(int id) override {}
  Sprite& GetSaveThumbnail(SaveType type, int id) override;

 private:
  uint8_t GameExtraData[1024];
  uint8_t MessageFlags[10000];
  std::array<uint8_t, 0x3b06> SystemData;
  bool EVFlags[1200];
  uint8_t BGMFlags[100];
};

}  // namespace MO6TW
}  // namespace Impacto