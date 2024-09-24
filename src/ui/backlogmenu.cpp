#include "backlogmenu.h"

#include "ui.h"
#include "../profile/game.h"
#include "../renderer/renderer.h"
#include "../mem.h"
#include "../vm/interface/input.h"
#include "../profile/scriptvars.h"
#include "../profile/scriptinput.h"
#include "../profile/dialogue.h"
#include "../profile/ui/backlogmenu.h"
#include "../profile/ui/systemmenu.h"
#include "../profile/games/mo6tw/backlogmenu.h"
#include "../profile/games/cc/backlogmenu.h"
#include "../profile/games/cclcc/systemmenu.h"
#include "../inputsystem.h"
#include "../io/vfs.h"

namespace Impacto {
namespace UI {

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::ScriptInput;
using namespace Impacto::Profile::BacklogMenu;
using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;

void BacklogMenu::MenuButtonOnClick(Widgets::BacklogEntry* target) {
  if (target->AudioId != -1) {
    Audio::Channels[Audio::AC_REV]->Play("voice", target->AudioId, false, 0.0f);
  }
}

BacklogMenu::BacklogMenu() {
  MainItems = new Widgets::Group(this, EntriesStart);
  MainItems->RenderingBounds = RenderingBounds;
  MainItems->WrapFocus = false;

  MainScrollbar = new Scrollbar(0, ScrollbarPosition, 0.0f, 1.0f, &PageY,
                                SBDIR_VERTICAL, ScrollbarTrack, ScrollbarThumb,
                                glm::vec2(0), ScrollbarThumbLength);
  MainScrollbar->Enabled = false;
  CurrentEntryPos = EntriesStart;

  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
}

void BacklogMenu::Show() {
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    MainItems->Show();
    if (!MainItems->Children.empty()) {
      auto el = MainItems->Children.back();
      FocusStart[FDIR_UP] = el;
      FocusStart[FDIR_DOWN] = el;
    }
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}
void BacklogMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    FadeAnimation.StartOut();
    MainItems->Hide();
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

inline bool inVerticalHoverBounds(const Widget& entry) {
  return (HoverBounds.Y <= entry.Bounds.Y &&
          entry.Bounds.Y + entry.Bounds.Height <=
              HoverBounds.Y + HoverBounds.Height);
}

void BacklogMenu::Update(float dt) {
  UpdateInput();

  FadeAnimation.Update(dt);

  if (FadeAnimation.IsIn())
    State = Shown;
  else if (FadeAnimation.IsOut())
    State = Hidden;

  if (State == Shown && IsFocused) {
    if (ItemsHeight > MainItems->RenderingBounds.Height) {
      MainScrollbar->Enabled = true;
      MainScrollbar->StartValue = MainItems->RenderingBounds.Y + EntryYPadding;
      MainScrollbar->EndValue = -ItemsHeight +
                                MainItems->RenderingBounds.Height +
                                MainItems->RenderingBounds.Y;
    }
    if (MainScrollbar->Enabled) {
      MainItems->MoveTo(glm::vec2(EntriesStart.x, PageY));
      auto lastEntry = MainItems->Children.back();
      CurrentEntryPos.y =
          lastEntry->Bounds.Y + lastEntry->Bounds.Height + EntryYPadding;
    }
    MainItems->Update(dt);
    if ((PADinputButtonWentDown & PAD1DOWN ||
         PADinputButtonWentDown & PAD1UP) &&
        MainScrollbar->Enabled) {
      const Widget* focusedEl = CurrentlyFocusedElement;
      if (focusedEl->Bounds.Y < HoverBounds.Y) {
        PageY += focusedEl->Bounds.Height + EntryYPadding;
      } else if (focusedEl->Bounds.Y + focusedEl->Bounds.Height >
                 HoverBounds.Y + HoverBounds.Height) {
        PageY -= focusedEl->Bounds.Height + EntryYPadding;
      }
    }

    MainScrollbar->UpdateInput();
    MainScrollbar->Update(dt);

    // Handle entry moving out of hover bounds
    if (CurrentlyFocusedElement &&
        !inVerticalHoverBounds(*CurrentlyFocusedElement)) {
      FocusDirection dir = (CurrentlyFocusedElement->Bounds.Y < HoverBounds.Y)
                               ? FDIR_DOWN
                               : FDIR_UP;
      Widget* newFocusedElement = CurrentlyFocusedElement->GetFocus(dir);
      while (newFocusedElement && !inVerticalHoverBounds(*newFocusedElement))
        newFocusedElement = newFocusedElement->GetFocus(dir);

      CurrentlyFocusedElement->Hovered = false;
      CurrentlyFocusedElement->HasFocus = false;
      CurrentlyFocusedElement = newFocusedElement;
      if (newFocusedElement != nullptr) newFocusedElement->HasFocus = true;
    }
  }
}

void BacklogMenu::RenderHighlight() const {
  if (EntryHighlightLocation == +EntryHighlightLocationType::None ||
      CurrentlyFocusedElement == nullptr ||
      !MainItems->RenderingBounds.Intersects(CurrentlyFocusedElement->Bounds))
    return;

  RectF pos;
  const Widget& el = *CurrentlyFocusedElement;

  switch (EntryHighlightLocation) {
    default:
    case EntryHighlightLocationType::BottomLeftOfEntry:
      pos = RectF(
          el.Bounds.X,
          el.Bounds.Y + el.Bounds.Height - EntryHighlight.ScaledHeight(),
          Profile::Dialogue::REVBounds.Width, EntryHighlight.ScaledHeight());
      break;
    case EntryHighlightLocationType::TopLineLeftOfScreen:
      pos = RectF(0.0f, el.Bounds.Y, EntryHighlight.ScaledWidth(),
                  EntryHighlight.ScaledHeight());
      break;
  }

  pos.X += EntryHighlightOffset.x;
  pos.Y += EntryHighlightOffset.y;

  Renderer->DrawSprite(EntryHighlight, pos);
}

void BacklogMenu::Render() {}

void BacklogMenu::AddMessage(uint8_t* str, int audioId) {
  if (!GetFlag(SF_REVADDDISABLE) || ScrWork[SW_MESWIN0TYPE] == 0) {
    auto onClick =
        std::bind(&BacklogMenu::MenuButtonOnClick, this, std::placeholders::_1);

    auto backlogEntry =
        new BacklogEntry(CurrentId, str, audioId, CurrentEntryPos, HoverBounds);
    CurrentId += 1;
    CurrentEntryPos.y += backlogEntry->TextHeight + EntryYPadding;
    backlogEntry->OnClickHandler = onClick;
    MainItems->Add(backlogEntry, FDIR_DOWN);
    ItemsHeight += backlogEntry->TextHeight + EntryYPadding;
    if (ItemsHeight > MainItems->RenderingBounds.Height) {
      MainScrollbar->EndValue = -ItemsHeight +
                                MainItems->RenderingBounds.Height +
                                MainItems->RenderingBounds.Y;
      PageY = MainScrollbar->EndValue;
      MainItems->MoveTo(glm::vec2(EntriesStart.x, PageY));
      CurrentEntryPos.y =
          backlogEntry->Bounds.Y + backlogEntry->Bounds.Height + EntryYPadding;
    }
  }
}

void BacklogMenu::Clear() {
  MainItems->Clear();
  PageY = 0.0f;
  CurrentId = 0;
  ItemsHeight = 0.0f;
  MainScrollbar->StartValue = 0.0f;
  MainScrollbar->EndValue = 0.0f;
  MainScrollbar->Enabled = false;
  CurrentEntryPos = EntriesStart;
}

}  // namespace UI
}  // namespace Impacto