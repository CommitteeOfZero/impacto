#pragma once

#include "../../data/tipssystem.h"

namespace Impacto {
namespace CCLCC {

using namespace Impacto::TipsSystem;

class TipsSystem : public TipsSystemBase {
 public:
  TipsSystem(size_t maxTipsCount) : TipsSystemBase(maxTipsCount) {};

  void DataInit(uint32_t scriptBufferId, uint32_t tipsData,
                uint32_t tipsDataSize) override;
  void UpdateTipRecords() override;
  void SetTipLockedState(size_t id, bool state) override;
  void SetTipUnreadState(size_t id, bool state) override;
  void SetTipNewState(size_t id, bool state) override;

  bool GetTipLockedState(size_t id) override;
};

}  // namespace CCLCC
}  // namespace Impacto