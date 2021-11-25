#pragma once

#include "../../ui/tipsmenu.h"
#include "../../ui/widgets/group.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

int const MaxTipStrings = 6;
uint32_t const MaxTipDataSize = 0x500;

struct TipsDataRecord {
  uint16_t unk01;
  uint16_t thumbnailIndex;
  uint16_t numberOfContentStrings;
  uint8_t* stringPtrs[MaxTipStrings];
};

class TipsMenu : public UI::TipsMenu {
 public:
  TipsMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  void DataInit(int scriptBufferId, uint8_t* tipsData);

 protected:
  void SwitchToTipId(int id);
  void SwitchToTipPage(int page);

 private:
  std::vector<TipsDataRecord> Records;
  Sprite* ThumbnailSprite;
};

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto