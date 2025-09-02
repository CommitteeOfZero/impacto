#pragma once

#include "../../data/tipssystem.h"

namespace Impacto {
namespace CHLCC {

using namespace Impacto::TipsSystem;

class TipsSystem : public TipsSystemBase {
 public:
  TipsSystem(size_t maxTipsCount) : TipsSystemBase(maxTipsCount) {};

  void DataInit(uint32_t scriptBufferId, uint32_t tipsDataAdr,
                uint32_t tipsDataSize);
  void UpdateTipRecords();
  void SetTipLockedState(size_t id, bool state);
  void SetTipUnreadState(size_t id, bool state);
  void SetTipNewState(size_t id, bool state);

  bool GetTipLockedState(size_t id);
};

}  // namespace CHLCC
}  // namespace Impacto