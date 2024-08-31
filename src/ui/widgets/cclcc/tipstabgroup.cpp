#include "tipstabgroup.h"

#include "../../../renderer/renderer.h"
#include "../../../profile/dialogue.h"
#include "../../../profile/games/cclcc/tipsmenu.h"
#include "../../../ui/ui.h"
#include "../../../text.h"
#include "../../../vm/vm.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

using namespace Impacto::TipsSystem;
using namespace Impacto::Profile::CCLCC::TipsMenu;
using namespace Impacto::UI::CCLCC;

TipsTabGroup::TipsTabGroup(TipsTabType type,
                           std::function<void(Widgets::Button*)> onClickHandler)
    : Type(type),
      TabName(Button(
          type, TipsHighlightedTabSprite, Sprite(), Sprite(),
          TipsTabNameDisplay + glm::vec2(type * TipsHighlightedTabAdder, 0))),
      TipsEntriesGroup(this) {
  TipsEntriesGroup.RenderingBounds = TipsTabBounds;
  TipsEntriesGroup.Bounds = TipsTabBounds;
  TipsEntriesGroup.WrapFocus = false;
  TabName.NormalSprite.Bounds.X += type * TipsHighlightedTabAdder;
  TabName.OnClickHandler = std::move(onClickHandler);
}

void TipsTabGroup::Update(float dt) {
  TabName.Update(dt);
  TabName.UpdateInput();
  TipsEntriesGroup.Update(dt);
  TipsEntriesGroup.UpdateInput();
  UpdateInput();
  if (CurrentlyFocusedElement) {
    if (CurrentlyFocusedElement->Bounds.Y <
        TipsEntriesGroup.RenderingBounds.Y) {
      TipsEntriesGroup.Move({0, CurrentlyFocusedElement->Bounds.Height});
    } else if (CurrentlyFocusedElement->Bounds.Y >=
               TipsEntriesGroup.RenderingBounds.Y +
                   TipsEntriesGroup.RenderingBounds.Height) {
      TipsEntriesGroup.Move({0, -CurrentlyFocusedElement->Bounds.Height});
    }
  }

  // Inverting since we want buttons to be clickable when the tab is not shown
  TabName.HasFocus = State != Shown;
  TabName.Enabled = Impacto::UI::TipsMenuPtr->IsFocused;
}

void TipsTabGroup::Render() {
  if (State == Shown) {
    TabName.Render();
    TipsEntriesGroup.Render();
  }
}

void TipsTabGroup::UpdateTipsEntries(std::vector<int> const& SortedTipIds) {
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
    RectF buttonBounds = TipsEntryBounds;
    buttonBounds.Y += EntriesCount * buttonBounds.Height;
    TipsEntryButton* button = new TipsEntryButton(
        tipId, sortIndex++, buttonBounds, TipsHighlightedSprite);
    EntriesCount++;
    TipsEntriesGroup.Add(button, FDIR_DOWN);
  }
}

void TipsTabGroup::Show() {
  if (State != Shown) {
    State = Shown;
    IsFocused = true;
    TipsEntriesGroup.Show();
    TabName.Show();
  }
}
void TipsTabGroup::Hide() {
  if (State != Hidden) {
    State = Hidden;
    IsFocused = false;
    TipsEntriesGroup.Hide();
    TabName.Hide();
  }
}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto