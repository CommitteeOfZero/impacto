#include "backlogmenu.h"

#include <limits>

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
#include "../audio/audiosystem.h"

namespace Impacto {
namespace UI {

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::ScriptInput;
using namespace Impacto::Profile::BacklogMenu;
using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;

void BacklogMenu::MenuButtonOnClick(Widgets::BacklogEntry* target) {
  if (target->AudioId.has_value()) {
    const float volume =
        Profile::ConfigSystem::VoiceMuted[target->CharacterId]
            ? 0.0f
            : Profile::ConfigSystem::VoiceVolume[target->CharacterId];
    Audio::Channels[Audio::AC_REV]->SetVolume(volume);
    Audio::Channels[Audio::AC_REV]->Play("voice", *target->AudioId, false,
                                         0.0f);
  }
}

BacklogMenu::BacklogMenu()
    : Entries(MaxEntryCount), ItemsHeight(EntryYPadding) {
  InputConfig = InputRate::RepeatFast;

  MainScrollbar =
      new Scrollbar(0, ScrollbarPosition, 0.0f, 1.0f, &PageY, SBDIR_VERTICAL,
                    ScrollbarTrack, ScrollbarThumb, glm::vec2(0),
                    ScrollbarThumbLength, RenderingBounds, -5000);
  MainScrollbar->Enabled = false;

  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
}

void BacklogMenu::Show() {
  if (State == Hidden) {
    State = Showing;
    FadeAnimation.StartIn();

    for (auto& entry : Entries) {
      entry->Show();
    }

    if (!Entries.empty()) {
      Widget* const el = Entries.front().get();
      FocusStart[FDIR_UP] = el;
      FocusStart[FDIR_DOWN] = el;
    }
    CurrentlyFocusedElement = nullptr;

    if (UI::FocusedMenu != nullptr) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    UI::FocusedMenu = this;

    ResetPADHoldTimer(PAD1UP | PAD1DOWN | PAD1RIGHT | PAD1LEFT);

    // Set scrollbar back to default position
    if (ItemsHeight > RenderingBounds.Height) {
      PageY = MainScrollbar->EndValue;
      MainScrollbar->Update(0);
      MoveEntriesBottomTo(std::min(RenderingBounds.Top() + ItemsHeight,
                                   RenderingBounds.Bottom()));
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

void BacklogMenu::MoveEntriesBottomTo(const float yPosition) {
  if (Entries.empty()) return;

  const BacklogEntry* const newestEntry = Entries.front().get();
  const glm::vec2 offset = {
      0.0f, yPosition - (newestEntry->Bounds.Bottom() + EntryYPadding)};

  for (auto& entry : Entries) {
    entry->Move(offset);
  }
}

static bool IsBeyondShiftedHoverBounds(const Widget* el, float delta, bool up) {
  if (up) return el->Bounds.Y < HoverBounds.Y + delta;

  return el->Bounds.Bottom() > HoverBounds.Bottom() + delta;
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
      CurrentlyFocusedElement =
          (dir == FDIR_UP) ? Entries.back().get() : Entries.front().get();
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

    CurrentlyFocusedElement =
        (dir == FDIR_UP) ? Entries.back().get() : Entries.front().get();
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

  if (!(State == Shown && IsFocused)) {
    AtBottomPrev = false;
    return;
  }

  if (!MainScrollbar->IsScrollHeld()) {
    for (auto& entry : Entries) {
      entry->UpdateInput(dt);

      entry->Hovered &= entry->Bounds.Intersects(HoverBounds);
      if (entry->Hovered &&
          (Input::CurrentInputDevice == Input::Device::Mouse ||
           Input::CurrentInputDevice == Input::Device::Touch)) {
        if (CurrentlyFocusedElement && entry.get() != CurrentlyFocusedElement) {
          CurrentlyFocusedElement->HasFocus = false;
        }
        CurrentlyFocusedElement = entry.get();
        CurrentlyFocusedElement->HasFocus = true;
      }
    }
  }

  if (Profile::CloseBacklogWhenReachedEnd) {
    const float epsilon = std::numeric_limits<float>::epsilon();
    const bool atBottomNow = !MainScrollbar->Enabled ||
                             (PageY <= (MainScrollbar->EndValue + epsilon));

    if (AtBottomPrev && atBottomNow && Input::MouseWheelDeltaY < 0) {
      Vm::Interface::PADinputMouseWentDown |= Vm::Interface::PADcustom[6];
    }
    AtBottomPrev = atBottomNow;
  }
}

void BacklogMenu::Update(float dt) {
  if (State != Hidden && State != Shown) FadeAnimation.Update(dt);
  UpdateVisibility();
  SetFlag(SF_BACKLOG_NOLOG, Entries.empty());

  if (State != Shown || !IsFocused) return;

  UpdateInput(dt);

  if (MainScrollbar->Enabled) {
    MoveEntriesBottomTo(PageY + RenderingBounds.Bottom());
  }

  for (auto& entry : Entries) {
    entry->Update(dt);
  }

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

void BacklogMenu::RenderHighlight(const glm::vec2 offset) const {
  if (EntryHighlightLocation == EntryHighlightLocationType::None ||
      CurrentlyFocusedElement == nullptr ||
      !RenderingBounds.Intersects(CurrentlyFocusedElement->Bounds))
    return;

  RectF pos;
  const BacklogEntry& el = *static_cast<BacklogEntry*>(CurrentlyFocusedElement);

  switch (EntryHighlightLocation) {
    default:
    case EntryHighlightLocationType::BottomLeftOfEntry: {
      const float pageWidth =
          TextModesInfo[Profile::Dialogue::REVMessageModeIdx].MaxLineWidth;
      pos =
          RectF(el.Bounds.X,
                el.Bounds.Y + el.Bounds.Height - EntryHighlight.ScaledHeight(),
                pageWidth, EntryHighlight.ScaledHeight());
    } break;
    case EntryHighlightLocationType::TopLineLeftOfScreen:
      pos = RectF(0.0f, el.Page.Glyphs[0].DestRect.Y - EntryHighlightPadding,
                  EntryHighlight.ScaledWidth(), EntryHighlight.ScaledHeight());
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

void BacklogMenu::AddMessage(Vm::BufferOffsetContext scrCtx,
                             std::optional<int> audioId, int characterId) {
  if (GetFlag(SF_REVADDDISABLE) && ScrWork[SW_MESWIN0TYPE] != 0) return;

  auto onClick = [this](auto* btn) { return MenuButtonOnClick(btn); };

  const glm::vec2 entryPos =
      Entries.empty()
          ? EntriesStart
          : glm::vec2(EntriesStart.x,
                      Entries.front()->Page.Glyphs.back().DestRect.Bottom() +
                          EntryYPadding);

  BacklogEntry* const backlogEntry =
      CreateBacklogEntry(scrCtx, audioId, characterId, entryPos, HoverBounds);
  backlogEntry->OnClickHandler = onClick;

  if (Entries.full()) {
    ItemsHeight -= Entries.back()->Bounds.Height + EntryYPadding;
  }
  ItemsHeight += backlogEntry->Bounds.Height + EntryYPadding;

  if (!Entries.empty()) {
    BacklogEntry* const previousEntry = Entries.front().get();
    backlogEntry->SetFocus(previousEntry, FDIR_UP);
    previousEntry->SetFocus(backlogEntry, FDIR_DOWN);
  }
  Entries.push_front(std::unique_ptr<BacklogEntry>(backlogEntry));
  Entries.back()->SetFocus(nullptr, FDIR_UP);

  if (ItemsHeight > RenderingBounds.Height) {
    MainScrollbar->StartValue = ItemsHeight - RenderingBounds.Height;
    MainScrollbar->EndValue = 0.0f;
    MainScrollbar->Enabled = true;
    PageY = MainScrollbar->EndValue;

    MoveEntriesBottomTo(RenderingBounds.Bottom());
  } else {
    MainScrollbar->StartValue = 0.0f;
    MainScrollbar->EndValue = 1.0f;
    MainScrollbar->Enabled = false;
    PageY = MainScrollbar->StartValue;
  }
}

void BacklogMenu::Clear() {
  Entries.clear();

  PageY = 0.0f;
  ItemsHeight = EntryYPadding;

  MainScrollbar->StartValue = 0.0f;
  MainScrollbar->EndValue = 1.0f;
  MainScrollbar->Enabled = false;
}

}  // namespace UI
}  // namespace Impacto