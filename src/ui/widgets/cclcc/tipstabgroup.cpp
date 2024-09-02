#include "tipstabgroup.h"

#include "../../../renderer/renderer.h"
#include "../../../profile/dialogue.h"
#include "../../../profile/games/cclcc/tipsmenu.h"
#include "../../../inputsystem.h"
#include "../../../vm/interface/input.h"
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

TipsTabButton::TipsTabButton(
    TipsTabType type, std::function<void(Widgets::Button*)> onClickHandler)

    : Button(
          type, TipsHighlightedTabSprite, Sprite(), Sprite(),
          TipsTabNameDisplay + glm::vec2(type * TipsHighlightedTabAdder, 0)) {
  OnClickHandler = std::move(onClickHandler);
  NormalSprite.Bounds.X += type * TipsHighlightedTabAdder;
}

void TipsTabButton::UpdateInput() {
  if (Enabled) {
    if (Input::PrevMousePos != Input::CurMousePos) {
      Hovered = Bounds.ContainsPoint(Input::CurMousePos);
    }
    if (OnClickHandler && HasFocus &&
        ((Hovered &&
          Vm::Interface::PADinputMouseWentDown & Vm::Interface::PAD1A))) {
      OnClickHandler(this);
    }
  }
}

TipsTabGroup::TipsTabGroup(
    TipsTabType type, std::function<void(Widgets::Button*)> tabClickHandler,
    std::function<void(Widgets::Button*)> tipClickHandler)
    : Type(type),
      TabName(type, tabClickHandler),
      TipsEntriesGroup(this),
      TipClickHandler(tipClickHandler) {
  TipsEntriesGroup.RenderingBounds = TipsTabBounds;
  TipsEntriesGroup.WrapFocus = false;
}

void TipsTabGroup::UpdateInput() {
  using namespace Vm::Interface;
  if (IsFocused) {
    if (PADinputButtonWentDown & PAD1DOWN) {
      AdvanceFocus(FDIR_DOWN);
      Audio::Channels[Audio::AC_SSE]->Play("sysse", 1, false, 0);
    } else if (PADinputButtonWentDown & PAD1UP) {
      AdvanceFocus(FDIR_UP);
      Audio::Channels[Audio::AC_SSE]->Play("sysse", 1, false, 0);
    }
  }
}

void TipsTabGroup::Update(float dt) {
  if (!Impacto::UI::TipsMenuPtr->IsFocused ||
      Impacto::UI::TipsMenuPtr->State == Hidden) {
    TabName.Enabled = false;
    TipsEntriesGroup.Enabled = false;
    return;
  }
  TabName.Enabled = true;
  TabName.Update(dt);
  TabName.UpdateInput();
  if (State != Shown) {
    TipsEntriesGroup.Enabled = false;
    // Inverting since we want buttons to be clickable when the tab is not shown
    TabName.HasFocus = true;
  } else {
    TipsEntriesGroup.Enabled = true;
    TabName.HasFocus = false;
    TipsEntriesGroup.Update(dt);
  }
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
    auto& record = *TipsSystem::GetTipRecord(tipId);
    if (!tipsFilterPredicate(record)) {
      sortIndex++;
      continue;
    }
    bool dispNew = record.IsNew && !record.IsLocked;
    RectF buttonBounds = TipsEntryBounds;
    buttonBounds.Y += EntriesCount * buttonBounds.Height;
    TipsEntryButton* button = new TipsEntryButton(
        tipId, sortIndex++, buttonBounds, TipsHighlightedSprite, dispNew);
    button->OnClickHandler = TipClickHandler;
    EntriesCount++;
    TipsEntriesGroup.Add(button, FDIR_DOWN);
    if (Type == TipsTabType::NewTips) {
      TipsSystem::SetTipNewState(tipId, false);
    }
  }
}

void TipsTabGroup::Show() {
  if (State != Shown) {
    State = Shown;
    IsFocused = true;
    TipsEntriesGroup.Show();
    TabName.Show();
    CurrentlyFocusedElement = TipsEntriesGroup.GetFocus(FDIR_DOWN);
    if (CurrentlyFocusedElement) CurrentlyFocusedElement->HasFocus = true;
  }
}
void TipsTabGroup::Hide() {
  if (State != Hidden) {
    State = Hidden;
    IsFocused = false;
    TipsEntriesGroup.MoveTo({0, 0});
    TipsEntriesGroup.Hide();
    TabName.Hide();
  }
}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto