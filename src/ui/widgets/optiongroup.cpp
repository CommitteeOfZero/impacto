#include "optiongroup.h"

#include "../../inputsystem.h"
#include "../../vm/interface/input.h"

namespace Impacto {
namespace UI {
namespace Widgets {

using namespace Impacto::Vm::Interface;

OptionGroup::OptionGroup(Menu* menuContext, Sprite const& background,
                         Sprite const& highlight, Sprite const& itemHighlight,
                         glm::vec2 pos, glm::vec2 itemsOffset) {
  MenuContext = menuContext;
  BackgroundSprite = background;
  BackgroundHSprite = highlight;
  ItemHighlightSprite = itemHighlight;
  Position = pos;
  CurrentItemPosition = pos + itemsOffset;
  Bounds = RectF(pos.x, pos.y, BackgroundSprite.ScaledWidth(),
                 BackgroundSprite.ScaledHeight());
}

void OptionGroup::Update(float dt) {
  for (const auto& item : Items) {
    item->Update(dt);
  }
}

void OptionGroup::UpdateInput() {
  if (Enabled) {
    bool mouseInput =
        Input::CurrentInputDevice == Input::InputDevice::IDEV_Mouse ||
        Input::CurrentInputDevice == Input::InputDevice::IDEV_Touch;

    if (GroupEntered || mouseInput) {
      for (const auto& item : Items) {
        if (item->Enabled && item->Hovered && mouseInput) {
          if (MenuContext->CurrentlyFocusedElement)
            MenuContext->CurrentlyFocusedElement->HasFocus = false;
          item->HasFocus = true;
          MenuContext->CurrentlyFocusedElement = item;
        }
        item->UpdateInput();
      }

      if (!mouseInput && (GetControlState(CT_OK) || GetControlState(CT_Back))) {
        GroupEntered = false;
        MenuContext->CurrentlyFocusedElement->HasFocus = false;
        MenuContext->CurrentlyFocusedElement = this;
        MenuContext->CurrentlyFocusedElement->HasFocus = true;
        PADinputButtonWentDown &= ~PADcustom[6];
        PADinputButtonWentDown &= ~PADcustom[5];
      }
    }

    if (HasFocus) {
      GroupEntered = false;

      if (!mouseInput && GetControlState(CT_OK)) {
        GroupEntered = true;
        if (Items.size() > 0) {
          MenuContext->CurrentlyFocusedElement->HasFocus = false;
          MenuContext->CurrentlyFocusedElement = Items.at(0);
          MenuContext->CurrentlyFocusedElement->HasFocus = true;
        }
      }
    }

    if (Input::PrevMousePos != Input::CurMousePos) {
      Hovered = Bounds.ContainsPoint(Input::CurMousePos);
    }
  }
}

void OptionGroup::Render() {
  if (HasFocus || GroupEntered)
    Renderer->DrawSprite(BackgroundHSprite, Position, Tint);
  else
    Renderer->DrawSprite(BackgroundSprite, Position, Tint);

  for (const auto& item : Items) {
    item->Tint = Tint;
    item->Render();

    if (GroupEntered && item->HasFocus) {
      Renderer->DrawSprite(ItemHighlightSprite,
                           glm::vec2(item->Bounds.X, item->Bounds.Y), Tint);
    }
  }
}

void OptionGroup::AddOption(Toggle* item) {
  item->MoveTo(CurrentItemPosition);
  CurrentItemPosition.x += (item->Bounds.Width + ItemPadding);
  Items.push_back(item);

  if (LastFocusableElementId != -1) {
    auto el = Items.at(LastFocusableElementId);
    el->SetFocus(item, FDIR_RIGHT);
    item->SetFocus(el, FDIR_LEFT);
  }
  LastFocusableElementId = (int)Items.size() - 1;
  auto firstEl = Items.at(0);
  item->SetFocus(firstEl, FDIR_RIGHT);
  firstEl->SetFocus(item, FDIR_LEFT);
}

void OptionGroup::AddOption(Scrollbar* item) {
  item->MoveTo(CurrentItemPosition);
  CurrentItemPosition.x += (item->Bounds.Width + ItemPadding);
  Items.push_back(item);
}

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto