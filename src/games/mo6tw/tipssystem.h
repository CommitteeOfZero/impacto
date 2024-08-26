#pragma once

#include "../../data/tipssystem.h"

namespace Impacto {
namespace MO6TW {

using namespace Impacto::TipsSystem;

class TipsSystem : public TipsSystemBase {
 public:
  TipsSystem(int maxTipsCount) : TipsSystemBase(maxTipsCount) {};

  void DataInit(int scriptBufferId, uint8_t* tipsData);
  void UpdateTipRecords();
  void SetTipLockedState(int id, bool state);
  void SetTipUnreadState(int id, bool state);
  void SetTipNewState(int id, bool state);

  bool GetTipLockedState(int id);
};

}  // namespace MO6TW
}  // namespace Impacto