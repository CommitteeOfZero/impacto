#pragma once

#include "../../ui/tipsmenu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/carousel.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

int const MaxTipStrings = 6;
uint32_t const MaxTipDataSize = 0x500;
int const MaxTipPages = 10;
int const MaxTipsPerPage = 50;

struct TipsDataRecord {
  uint16_t id;
  uint16_t sortLetterIndex;
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

  void TipOnClick(Widgets::Button* target);

 protected:
  void SwitchToTipId(int id);
  void SwitchToTipPage(int page);

 private:
  int CurrentTipPage = 1;
  int CurrentPageIdx = 0;
  int PageCount = 0;
  uint16_t* sortString = 0;
  std::vector<TipsDataRecord> Records;
  Sprite* ThumbnailSprite;
  Widgets::Carousel* ItemsList;
  Widgets::Label* PageSeparator;
  Widgets::Label* CurrentPage;
  Widgets::Label* TotalPages;
};

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto