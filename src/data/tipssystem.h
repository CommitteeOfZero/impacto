#pragma once
#include "../impacto.h"

#include <vector>
#include <array>
#include <enum.h>
#include "../log.h"

namespace Impacto {
namespace TipsSystem {

BETTER_ENUM(TipsSystemType, int, None, MO6TW, CHLCC, CCLCC)

int constexpr MaxTipStrings = 10;

struct TipsDataRecord {
  uint16_t Id = 0;
  uint16_t SortLetterIndex = 0;
  uint16_t ThumbnailIndex = 0;
  uint16_t NumberOfContentStrings = 0;
  std::array<uint32_t, MaxTipStrings> StringAdr = {};
  bool IsLocked = true;
  bool IsUnread = true;
  bool IsNew = true;
};

class TipsSystemBase {
 public:
  TipsSystemBase(int maxTipsCount) : Records(maxTipsCount) {}
  virtual ~TipsSystemBase() {}
  virtual void DataInit(uint32_t scriptBufferId, uint32_t tipsDataAdr,
                        uint32_t tipsDataSize) = 0;
  virtual void UpdateTipRecords() = 0;
  virtual void SetTipLockedState(int id, bool state) = 0;
  virtual void SetTipUnreadState(int id, bool state) = 0;
  virtual void SetTipNewState(int id, bool state) = 0;

  virtual bool GetTipLockedState(int id) = 0;

  std::vector<TipsDataRecord> Records;
  size_t TipEntryCount = 0;
  uint8_t ScriptBufferId = 0;
};

inline std::unique_ptr<TipsSystemBase> Implementation;

void Init();
void DataInit(uint32_t scriptBufferId, uint32_t tipsDataAdr,
              uint32_t tipsDataSize);
void UpdateTipRecords();
void SetTipLockedState(int id, bool state);
void SetTipUnreadState(int id, bool state);
void SetTipNewState(int id, bool state);
uint8_t GetTipsScriptBufferId();
bool GetTipLockedState(int id);

std::vector<TipsDataRecord>* GetTipRecords();
TipsDataRecord* GetTipRecord(int id);
int GetTipCount();

}  // namespace TipsSystem
}  // namespace Impacto