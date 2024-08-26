#pragma once

#include "../../data/tipssystem.h"

namespace Impacto {
namespace CCLCC {

using namespace Impacto::TipsSystem;

class TipsSystem : public TipsSystemBase {
 public:
  TipsSystem(int maxTipsCount) : TipsSystemBase(maxTipsCount) {};

  void DataInit(int scriptBufferId, uint8_t* tipsData,
                uint32_t tipsDataSize) override;
  void UpdateTipRecords() override;
  void SetTipLockedState(int id, bool state) override;
  void SetTipUnreadState(int id, bool state) override;
  void SetTipNewState(int id, bool state) override;

  bool GetTipLockedState(int id) override;
};

}  // namespace CCLCC
}  // namespace Impacto