#pragma once

#include "../../ui/tipsmenu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/carousel.h"
#include "../../ui/widgets/label.h"
#include "../../data/tipssystem.h"
#include "../../ui/widgets/cclcc/tipsentrybutton.h"
#include <set>

namespace Impacto::UI::Widgets::CCLCC {
class TipsTabGroup;
}
namespace Impacto {
namespace UI {
namespace CCLCC {

enum TipsTabType {
  AllTips,
  UnlockedTips,
  NewTips,
  UnreadTips,
};

struct SortByTipName {
  SortByTipName();
  bool operator()(int a, int b) const;
  uint8_t* SortString;
  ska::flat_hash_map<uint8_t, int> Sc3SortMap;
};

class TipsMenu : public UI::TipsMenu {
 public:
  TipsMenu();

  void Init();

  void Show();
  void Hide();
  void UpdateInput();
  void Update(float dt);
  void Render();

  void TipOnClick(Widgets::Button* target);

 protected:
  void SwitchToTipId(int id);
  void NextTipPage();

 private:
  constexpr int static TabCount = 4;
  int CurrentTipPage = 1;
  int CurrentTabIdx = 0;

  TipsTabType CurrentTabType;
  Widgets::Carousel TipViewItems;
  std::set<int, SortByTipName>* SortedTipIds;
  std::array<Widgets::CCLCC::TipsTabGroup*, TabCount> TipsTabs;
  Widgets::Label* PageSeparator;
  Widgets::Label* CurrentPage;
  Widgets::Label* TotalPages;
  bool HasInitialized = false;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto