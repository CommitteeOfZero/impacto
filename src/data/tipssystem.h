#pragma once
#include "../impacto.h"

#include <enum.h>
#include "../log.h"

namespace Impacto {
namespace TipsSystem {

BETTER_ENUM(TipsSystemType, int, None, MO6TW)

int const MaxTipStrings = 10;

struct TipsDataRecord {
  uint16_t Id;
  uint16_t SortLetterIndex;
  uint16_t ThumbnailIndex;
  uint16_t NumberOfContentStrings;
  uint8_t* StringPtrs[MaxTipStrings];
  bool IsLocked;
  bool IsUnread;
  bool IsNew;
};

class TipsSystemBase {
 public:
  virtual void DataInit(int scriptBufferId, uint8_t* tipsData) = 0;
  virtual void UpdateTipRecords() = 0;
  virtual void SetTipLockedState(int id, bool state) = 0;
  virtual void SetTipUnreadState(int id, bool state) = 0;
  virtual void SetTipNewState(int id, bool state) = 0;

  TipsDataRecord* Records;
  int TipEntryCount = 0;
};

extern TipsSystemBase* Implementation;

void Init();
void DataInit(int scriptBufferId, uint8_t* tipsData);
void UpdateTipRecords();
void SetTipLockedState(int id, bool state);
void SetTipUnreadState(int id, bool state);
void SetTipNewState(int id, bool state);

TipsDataRecord* GetTipRecords();
TipsDataRecord* GetTipRecord(int id);
int GetTipCount();

}  // namespace TipsSystem
}  // namespace Impacto