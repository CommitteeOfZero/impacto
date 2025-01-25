#pragma once

#include "../../data/tipssystem.h"

namespace Impacto {
namespace CHLCC {

using namespace Impacto::TipsSystem;

class TipsSystem : public TipsSystemBase {
 public:
  TipsSystem(int maxTipsCount) : TipsSystemBase(maxTipsCount) {};

  void DataInit(int scriptBufferId, uint8_t* tipsData, uint32_t tipsDataSize);
  void UpdateTipRecords();
  void SetTipLockedState(int id, bool state);
  void SetTipUnreadState(int id, bool state);
  void SetTipNewState(int id, bool state);

  bool GetTipLockedState(int id);
};

}  // namespace CHLCC
}  // namespace Impacto