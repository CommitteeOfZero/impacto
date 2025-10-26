#include "backlogmenu.h"

#include "ui.h"
#include "../profile/game.h"
#include "../profile/vm.h"
#include "../renderer/renderer.h"
#include "../mem.h"
#include "../vm/interface/input.h"
#include "../profile/scriptvars.h"
#include "../profile/scriptinput.h"
#include "../profile/dialogue.h"
#include "../profile/configsystem.h"
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
    const float volume =
        Profile::ConfigSystem::VoiceMuted[target->CharacterId]
            ? 0.0f
            : Profile::ConfigSystem::VoiceVolume[target->CharacterId];
    Audio::Channels[Audio::AC_REV]->SetVolume(volume);
    Audio::Channels[Audio::AC_REV]->Play("voice", target->AudioId, false, 0.0f);
  }
}

BacklogMenu::BacklogMenu() {
  InputConfig = InputRate::RepeatFast;
  MainItems = new Widgets::Group(this, EntriesStart);
  MainItems->RenderingBounds = RenderingBounds;
  MainItems->HoverBounds = HoverBounds;
  MainItems->WrapFocus = false;

  MainScrollbar =
      new Scrollbar(0, ScrollbarPosition, 0.0f, 1.0f, &PageY, SBDIR_VERTICAL,
                    ScrollbarTrack, ScrollbarThumb, glm::vec2(0),
                    ScrollbarThumbLength, RenderingBounds, -5000);
  MainScrollbar->Enabled = false;
  CurrentEntryPos = EntriesStart;

  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
}

void BacklogMenu::Show() {
  if (State == Hidden) {
    State = Showing;
    FadeAnimation.StartIn();
    MainItems->Show();

    if (!MainItems->Children.empty()) {
      auto el = MainItems->Children.back();
      FocusStart[FDIR_UP] = el;
      FocusStart[FDIR_DOWN] = el;
    }

    if (UI::FocusedMenu != nullptr) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    UI::FocusedMenu = this;

    ResetPADHoldTimer(PAD1UP | PAD1DOWN | PAD1RIGHT | PAD1LEFT);

    // Set scrollbar back to default position
    if (ItemsHeight > MainItems->RenderingBounds.Height) {
      PageY = MainScrollbar->EndValue;
      MainScrollbar->Update(0);
      MainItems->MoveTo(glm::vec2(EntriesStart.x, PageY));
    }
  }
}
void BacklogMenu::Hide() {
  if (State == Shown) {
    State = Hiding;
    FadeAnimation.StartOut();

    if (LastFocusedMenu != nullptr) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = nullptr;
    }
  }
}

static bool IsBeyondShiftedHoverBounds(const Widget* el, float delta, bool up) {
  if (up) return el->Bounds.Y < HoverBounds.Y + delta;

  return el->Bounds.Y + el->Bounds.Height >
         HoverBounds.Y + HoverBounds.Height + delta;
}

void BacklogMenu::UpdatePageUpDownInput(float dt) {
  const uint32_t shouldFire =
      Vm::Interface::PADinputButtonRepeatDown & (PAD1LEFT | PAD1RIGHT);

  const bool move =
      (bool)(shouldFire & PAD1LEFT) ^ (bool)(shouldFire & PAD1RIGHT);
  if (!move) {
    if (shouldFire) ResetPADHoldTimer(PAD1LEFT | PAD1RIGHT);
    return;
  }

  const FocusDirection dir = (shouldFire & PAD1LEFT) ? FDIR_UP : FDIR_DOWN;

  if (MainScrollbar->Enabled) {
    const float delta = (dir == FDIR_UP) ? PageUpDownHeight : -PageUpDownHeight;
    PageY += delta;
    MainScrollbar->ClampValue();

    if (!CurrentlyFocusedElement) {
      CurrentlyFocusedElement = (dir == FDIR_UP) ? MainItems->Children.front()
                                                 : MainItems->Children.back();
    }

    CurrentlyFocusedElement->HasFocus = false;

    Widget* nextEl = CurrentlyFocusedElement->GetFocus(dir);
    while (nextEl &&
           IsBeyondShiftedHoverBounds(nextEl, delta, dir == FDIR_UP)) {
      CurrentlyFocusedElement = nextEl;
      nextEl = CurrentlyFocusedElement->GetFocus(dir);
    }

  } else {
    if (CurrentlyFocusedElement) CurrentlyFocusedElement->HasFocus = false;

    CurrentlyFocusedElement = (dir == FDIR_UP) ? MainItems->Children.front()
                                               : MainItems->Children.back();
  }

  CurrentlyFocusedElement->HasFocus = true;
}

static bool InVerticalHoverBounds(const Widget* entry) {
  if (entry == nullptr) return false;

  return (HoverBounds.Y <= entry->Bounds.Y &&
          entry->Bounds.Y + entry->Bounds.Height <=
              HoverBounds.Y + HoverBounds.Height);
}

void BacklogMenu::UpdateScrollingInput(float dt) {
  const uint32_t held = PADinputButtonIsDown & (PAD1DOWN | PAD1UP);
  const bool padScrolling = (bool)(held & PAD1DOWN) ^ (bool)(held & PAD1UP);
  if (!padScrolling) return;

  FocusDirection dir = (held & PAD1DOWN) ? FDIR_DOWN : FDIR_UP;

  bool focusOnEdge = false;
  const Widget* nextEl = nullptr;
  if (CurrentlyFocusedElement != nullptr) {
    nextEl = CurrentlyFocusedElement->GetFocus(dir);
    focusOnEdge = !InVerticalHoverBounds(nextEl);
  }

  // Gradual scrolling
  if (MainScrollbar->Enabled && focusOnEdge) {
    PageY += (dir == FDIR_UP) ? ScrollingSpeed * dt : -ScrollingSpeed * dt;
    MainScrollbar->ClampValue();
    MainItems->Update(dt);
  }

  const uint32_t shouldFire = PADinputButtonRepeatDown & (PAD1DOWN | PAD1UP);

  if (focusOnEdge) {
    if (nextEl) {
      const float excess = (dir == FDIR_UP)
                               ? HoverBounds.Y - nextEl->Bounds.Y
                               : nextEl->Bounds.Y + nextEl->Bounds.Height -
                                     HoverBounds.Y - HoverBounds.Height;
      if (excess < ScrollingSpeed * dt) AdvanceFocus(dir);
    }
  } else {
    ImpLog(LogLevel::Trace, LogChannel::General, "Should Fire: {}", shouldFire);
    if ((shouldFire & PAD1UP) ^ (shouldFire & PAD1DOWN)) {
      dir = (shouldFire & PAD1DOWN) ? FDIR_DOWN : FDIR_UP;
      AdvanceFocus(dir);
    }
  }
  if (((shouldFire & PAD1UP) ^ (shouldFire & PAD1DOWN)) == 0 && shouldFire) {
    ResetPADHoldTimer(PAD1DOWN | PAD1UP);
  }
}

void BacklogMenu::UpdateInput(float dt) {
  MainScrollbar->UpdateInput(dt);
  UpdatePageUpDownInput(dt);
  UpdateScrollingInput(dt);
}

void BacklogMenu::Update(float dt) {
  if (State != Hidden && State != Shown) FadeAnimation.Update(dt);
  UpdateVisibility();
  SetFlag(SF_BACKLOG_NOLOG, MainItems->Children.empty());

  if (State == Shown && IsFocused) {
    UpdateInput(dt);

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
    MainScrollbar->Update(dt);

    // Handle entry moving out of hover bounds
    if (CurrentlyFocusedElement &&
        !InVerticalHoverBounds(CurrentlyFocusedElement)) {
      FocusDirection dir = (CurrentlyFocusedElement->Bounds.Y < HoverBounds.Y)
                               ? FDIR_DOWN
                               : FDIR_UP;
      Widget* newFocusedElement = CurrentlyFocusedElement->GetFocus(dir);
      while (newFocusedElement && !InVerticalHoverBounds(newFocusedElement))
        newFocusedElement = newFocusedElement->GetFocus(dir);

      CurrentlyFocusedElement->Hovered = false;
      CurrentlyFocusedElement->HasFocus = false;
      CurrentlyFocusedElement = newFocusedElement;
      if (newFocusedElement != nullptr) newFocusedElement->HasFocus = true;
    }
  }
}

void BacklogMenu::RenderHighlight(const glm::vec2 offset) const {
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
      pos = RectF(0.0f, el.Bounds.Y - EntryHighlightPadding,
                  EntryHighlight.ScaledWidth(),
                  EntryHighlight.ScaledHeight() + EntryHighlightPadding * 2);
      break;
    case EntryHighlightLocationType::AllLinesLeftOfScreen:
      pos = RectF(0.0f, el.Bounds.Y - EntryHighlightPadding,
                  EntryHighlight.ScaledWidth(),
                  el.Bounds.Height + EntryHighlightPadding * 2);
      break;
  }

  pos.X += EntryHighlightOffset.x;
  pos.Y += EntryHighlightOffset.y;

  float opacity = glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress);
  Renderer->DrawSprite(EntryHighlight, pos + offset,
                       glm::vec4(1.0f, 1.0f, 1.0f, opacity));
}

void BacklogMenu::Render() {}

void BacklogMenu::AddMessage(Vm::BufferOffsetContext scrCtx, int audioId,
                             int characterId) {
  if (!GetFlag(SF_REVADDDISABLE) || ScrWork[SW_MESWIN0TYPE] == 0) {
    auto onClick = [this](auto* btn) { return MenuButtonOnClick(btn); };

    Widgets::BacklogEntry* backlogEntry = CreateBacklogEntry(
        CurrentId, scrCtx, audioId, characterId, CurrentEntryPos, HoverBounds);
    backlogEntry->OnClickHandler = onClick;
    MainItems->Add(backlogEntry, FDIR_DOWN);
    CurrentId++;

    CurrentEntryPos.y += backlogEntry->TextHeight + EntryYPadding;
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
  MainItems->MoveTo(EntriesStart);
  PageY = 0.0f;
  CurrentId = 0;
  ItemsHeight = 0.0f;
  MainScrollbar->StartValue = 0.0f;
  MainScrollbar->EndValue = 1.0f;
  MainScrollbar->Enabled = false;
  CurrentEntryPos = EntriesStart;
}

}  // namespace UI
}  // namespace Impacto