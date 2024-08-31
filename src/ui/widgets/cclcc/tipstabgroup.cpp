#include "tipstabgroup.h"

#include "../../../renderer/renderer.h"
#include "../../../profile/dialogue.h"
#include "../../../profile/games/cclcc/tipsmenu.h"
#include "../../../text.h"
#include "../../../vm/vm.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

using namespace Impacto::TipsSystem;
using namespace Impacto::Profile::CCLCC::TipsMenu;
using namespace Impacto::UI::CCLCC;

TipsTabGroup::TipsTabGroup(Menu* ctx, TipsTabType type, RectF const dest,
                           glm::vec2 tabPos, Sprite const& highlight)
    : Group(ctx),
      Type(type),
      TabName(
          Button(to_underlying(type), Sprite(), highlight, Sprite(), tabPos)),
      TipsEntriesGroup(ctx, {dest.X, dest.Y + 100}) {
  Bounds = dest;
  TipsEntriesGroup.RenderingBounds =
      RectF(dest.X, dest.Y + 100, dest.Width, dest.Height);
  Add(&TipsEntriesGroup);
  Add(&TabName);
  TabName.HighlightSprite.Bounds.X += type * TipsHighlightedTabAdder;
}

void TipsTabGroup::Update(float dt) {
  TabName.Update(dt);
  TipsEntriesGroup.Update(dt);
  if (IsShown && !TabName.HasFocus) {
    TabName.HasFocus = true;
  }
}

void TipsTabGroup::Render() {
  if (IsShown) {
    TabName.Render();
    TipsEntriesGroup.Render();
  }
}

void TipsTabGroup::UpdateTipsEntries(
    std::set<int, UI::CCLCC::SortByTipName> const& SortedTipIds) {
  auto tipsFilterPredicate = [&](TipsSystem::TipsDataRecord const& record) {
    switch (Type) {
      case TipsTabType::AllTips:
        return true;
      case TipsTabType::UnlockedTips:
        return !record.IsLocked;
      case TipsTabType::UnreadTips:
        return record.IsUnread && !record.IsLocked;
      case TipsTabType::NewTips:
        return record.IsNew && !record.IsLocked;
    }
  };

  int sortIndex = 1;
  EntriesCount = 0;
  TipsEntriesGroup.Clear();
  for (auto& tipId : SortedTipIds) {
    if (!tipsFilterPredicate(*TipsSystem::GetTipRecord(tipId))) {
      sortIndex++;
      continue;
    }
    TipsEntryButton* button = new TipsEntryButton(
        tipId, sortIndex++,
        RectF(TipsEntryNameInitDisplay.x,
              TipsEntryNameInitDisplay.y + EntriesCount * TipsEntryNameOffset,
              TipsHighlightedSprite.Bounds.Width, TipsEntryNameOffset),
        TipsHighlightedSprite);
    EntriesCount++;
    TipsEntriesGroup.Add(button, FDIR_DOWN);
  }
}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto