#pragma once

#include "../button.h"
#include "../group.h"
#include "../../../data/tipssystem.h"
#include "../../../games/cclcc/tipsmenu.h"
#include "tipsentrybutton.h"
#include <set>

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class TipsTabGroup : public Widgets::Group {
 public:
  TipsTabGroup(Menu* ctx, Impacto::UI::CCLCC::TipsTabType type,
               RectF const dest, glm::vec2 tabPos, Sprite const& highlight);

  void Update(float dt) override;
  void Render() override;
  void UpdateTipsEntries(
      std::set<int, Impacto::UI::CCLCC::SortByTipName> const& SortedTipIds);
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