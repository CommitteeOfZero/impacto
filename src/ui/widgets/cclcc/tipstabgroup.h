#pragma once

#include "../button.h"
#include "../group.h"
#include "../../../data/tipssystem.h"
#include "../../../games/cclcc/tipsmenu.h"
#include "tipsentrybutton.h"
#include <vector>

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class TipsTabGroup : public Menu {
 public:
  TipsTabGroup(Impacto::UI::CCLCC::TipsTabType type,
               std::function<void(Widgets::Button*)> onClickHandler);

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void Render() override;
  void UpdateTipsEntries(std::vector<int> const& SortedTipIds);
  int GetTipEntriesCount() { return EntriesCount; }

 private:
  int EntriesCount = 0;
  Widgets::Group TipsEntriesGroup;
  Widgets::Button TabName;
  Impacto::UI::CCLCC::TipsTabType Type;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto