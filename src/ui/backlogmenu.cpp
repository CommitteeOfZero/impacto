#include "backlogmenu.h"

#include "ui.h"
#include "../renderer2d.h"
#include "../mem.h"
#include "../vm/interface/input.h"
#include "../profile/scriptvars.h"
#include "../profile/scriptinput.h"
#include "../profile/dialogue.h"
#include "../profile/ui/backlogmenu.h"
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
    Io::InputStream* stream;
    Io::VfsOpen("voice", target->AudioId, &stream);
    Audio::Channels[Audio::AC_REV].Play(Audio::AudioStream::Create(stream),
                                        false, 0.0f);
  }
}

BacklogMenu::BacklogMenu() {
  MainItems = new Widgets::Group(this, EntriesStart);
  MainItems->RenderingBounds = RenderingBounds;

  MainScrollbar = new Scrollbar(0, ScrollbarPosition, 0.0f, 1.0f, &PageY,
                                SBDIR_VERTICAL, ScrollbarTrack, ScrollbarThumb);
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
      MainScrollbar->MinValue = MainItems->RenderingBounds.Y + EntryYPadding;
      MainScrollbar->MaxValue = -ItemsHeight +
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
      auto focusedEl = CurrentlyFocusedElement;
      if (focusedEl->Bounds.Y < MainItems->RenderingBounds.Y) {
        PageY += focusedEl->Bounds.Height + EntryYPadding;
      } else if (focusedEl->Bounds.Y + focusedEl->Bounds.Height >
                 MainItems->RenderingBounds.Y +
                     MainItems->RenderingBounds.Height) {
        PageY -= focusedEl->Bounds.Height + EntryYPadding;
      }
    }

    MainScrollbar->UpdateInput();
    MainScrollbar->Update(dt);
  }
}

void BacklogMenu::Render() {
  if (State != Hidden) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    MainItems->Opacity = col.a;
    Renderer2D::DrawSprite(BacklogBackground, glm::vec2(0.0f), col);
    MainItems->Render();
    MainScrollbar->Render();
  }
}

void BacklogMenu::AddMessage(uint8_t* str, int audioId) {
  if (!GetFlag(SF_REVADDDISABLE) || ScrWork[SW_MESWIN0TYPE] == 0) {
    auto onClick =
        std::bind(&BacklogMenu::MenuButtonOnClick, this, std::placeholders::_1);

    auto backlogEntry =
        new BacklogEntry(CurrentId, str, audioId,
                         glm::vec2(CurrentEntryPos.x, CurrentEntryPos.y));
    CurrentId += 1;
    CurrentEntryPos.y += backlogEntry->TextHeight + EntryYPadding;
    backlogEntry->OnClickHandler = onClick;
    MainItems->Add(backlogEntry, FDIR_DOWN);
    ItemsHeight += backlogEntry->TextHeight + EntryYPadding;
    if (ItemsHeight > MainItems->RenderingBounds.Height) {
      MainScrollbar->MaxValue = -ItemsHeight +
                                MainItems->RenderingBounds.Height +
                                MainItems->RenderingBounds.Y;
      PageY = MainScrollbar->MaxValue;
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
  MainScrollbar->MinValue = 0.0f;
  MainScrollbar->MaxValue = 0.0f;
  MainScrollbar->Enabled = false;
  CurrentEntryPos = EntriesStart;
}

}  // namespace UI
}  // namespace Impacto