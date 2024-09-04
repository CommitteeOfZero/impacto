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
  TipsEntriesGroup.WrapFocus = true;

  TipsScrollStartPos = {
      TipsScrollEntriesX,
      TipsScrollYStart + TipsScrollThumbSprite.ScaledHeight() / 2.0f};

  TipsScrollTrackBounds = {
      TipsScrollThumbSprite.Bounds.Width,
      TipsScrollYEnd - TipsScrollYStart - TipsScrollThumbSprite.ScaledHeight()};
}

// Todo: Next page with left right keys
void TipsTabGroup::UpdateInput() {
  using namespace Vm::Interface;
  if (IsFocused) {
    auto prevEntry = CurrentlyFocusedElement;
    TipsEntriesScrollbar->UpdateInput();
    if (PADinputButtonWentDown & PAD1DOWN) {
      AdvanceFocus(FDIR_DOWN);
      if (CurrentlyFocusedElement != prevEntry) {
        if (CurrentlyFocusedElement->Bounds.Y >
                TipsTabBounds.Y + TipsTabBounds.Height ||
            CurrentlyFocusedElement->Bounds.Y < TipsTabBounds.Y) {
          if (CurrentlyFocusedElement != TipsEntryButtons.front())
            ScrollPosY += TipsEntryBounds.Height;
          else
            ScrollPosY = 0;
        }
      }
    } else if (PADinputButtonWentDown & PAD1UP) {
      AdvanceFocus(FDIR_UP);
      if (CurrentlyFocusedElement != prevEntry) {
        if (CurrentlyFocusedElement->Bounds.Y >
                TipsTabBounds.Y + TipsTabBounds.Height ||
            CurrentlyFocusedElement->Bounds.Y < TipsTabBounds.Y) {
          if (CurrentlyFocusedElement != TipsEntryButtons.back())
            ScrollPosY -= TipsEntryBounds.Height;
          else
            ScrollPosY = TipsEntriesScrollbar->MaxValue;
        }
      }
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
    // Inverting since we want buttons to be clickable when the tab is not
    // shown
    TabName.HasFocus = true;
  } else {
    TipsEntriesGroup.Enabled = true;
    TabName.HasFocus = false;
    TipsEntriesGroup.Update(dt);
  }
  float oldScrollPosY = ScrollPosY;
  UpdateInput();
  if (TipsEntriesScrollbar) {
    TipsEntriesScrollbar->Update(dt);

    TipsEntriesScrollbar->UpdateInput();
    if (oldScrollPosY != ScrollPosY) {
      TipsEntriesGroup.Move({0, oldScrollPosY - ScrollPosY});
    }
  }
}

void TipsTabGroup::Render() {
  if (State == Shown) {
    TabName.Render();
    TipsEntriesGroup.Render();
    TipsEntriesScrollbar->Render();
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
  TipsEntriesGroup.Clear();
  TipsEntryButtons.clear();
  delete TipsEntriesScrollbar;
  for (auto& tipId : SortedTipIds) {
    auto& record = *TipsSystem::GetTipRecord(tipId);
    if (!tipsFilterPredicate(record)) {
      sortIndex++;
      continue;
    }
    bool dispNew = record.IsNew && !record.IsLocked;
    RectF buttonBounds = TipsEntryBounds;
    buttonBounds.Y += TipsEntryButtons.size() * buttonBounds.Height;
    TipsEntryButton* button = new TipsEntryButton(
        tipId, sortIndex++, buttonBounds, TipsHighlightedSprite, dispNew);
    button->OnClickHandler = TipClickHandler;
    TipsEntriesGroup.Add(button, FDIR_DOWN);
    TipsEntryButtons.push_back(button);
    if (Type == TipsTabType::NewTips) {
      TipsSystem::SetTipNewState(tipId, false);
    }
  }

  auto roundUpMultiple = [](float numToRound, float multiple) {
    return std::ceil(numToRound / multiple) * multiple;
  };
  int scrollDistance =
      TipsEntryBounds.Height * TipsEntryButtons.size() -
      roundUpMultiple(TipsTabBounds.Height, TipsEntryBounds.Height);

  TipsEntriesScrollbar = new Scrollbar(
      0, TipsScrollStartPos, 0, std::max(0, scrollDistance), &ScrollPosY,
      SBDIR_VERTICAL, TipsScrollThumbSprite, TipsScrollTrackBounds);
}

void TipsTabGroup::Show() {
  if (State != Shown) {
    State = Shown;
    IsFocused = true;
    TipsEntriesGroup.Show();
    TabName.Show();
    CurrentlyFocusedElement = TipsEntriesGroup.GetFocus(FDIR_DOWN);

    if (CurrentlyFocusedElement) {
      static_cast<TipsEntryButton*>(CurrentlyFocusedElement)->PrevFocusState =
          true;
      CurrentlyFocusedElement->HasFocus = true;
    }
  }
}
void TipsTabGroup::Hide() {
  if (State != Hidden) {
    State = Hidden;
    IsFocused = false;
    ScrollPosY = 0.0f;
    TipsEntriesGroup.Hide();
    TabName.Hide();
  }
}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto