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
  uint16_t CategoryLetterIndex = 0;
  uint16_t ThumbnailIndex = 0;
  uint16_t NumberOfContentStrings = 0;
  std::array<uint32_t, MaxTipStrings> StringAdr = {};
  bool IsLocked = true;
  bool IsUnread = true;
  bool IsNew = true;
};

class TipsSystemBase {
 public:
  TipsSystemBase(size_t maxTipsCount) : Records(maxTipsCount) {}
  virtual ~TipsSystemBase() {}
  virtual void DataInit(uint32_t scriptBufferId, uint32_t tipsDataAdr,
                        uint32_t tipsDataSize) = 0;
  virtual void UpdateTipRecords() = 0;
  virtual void SetTipLockedState(size_t id, bool state) = 0;
  virtual void SetTipUnreadState(size_t id, bool state) = 0;
  virtual void SetTipNewState(size_t id, bool state) = 0;

  virtual bool GetTipLockedState(size_t id) = 0;

  std::vector<TipsDataRecord> Records;
  size_t TipEntryCount = 0;
  uint8_t ScriptBufferId = 0;
};

inline std::unique_ptr<TipsSystemBase> Implementation;

void Init();
void DataInit(uint32_t scriptBufferId, uint32_t tipsDataAdr,
              uint32_t tipsDataSize);
void UpdateTipRecords();
void SetTipLockedState(size_t id, bool state);
void SetTipUnreadState(size_t id, bool state);
void SetTipNewState(size_t id, bool state);
uint8_t GetTipsScriptBufferId();
bool GetTipLockedState(size_t id);

std::vector<TipsDataRecord>* GetTipRecords();
TipsDataRecord* GetTipRecord(size_t id);
size_t GetTipCount();

}  // namespace TipsSystem
}  // namespace Impacto