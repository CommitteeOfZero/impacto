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
  UnreadTips,
  NewTips,
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
  std::vector<int> SortedTipIds;
  std::array<Widgets::CCLCC::TipsTabGroup*, TabCount> TipsTabs;
  Widgets::Label* PageSeparator;
  Widgets::Label* CurrentPage;
  Widgets::Label* TotalPages;
  bool HasInitialized = false;

  void SetActiveTab(TipsTabType type);
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto