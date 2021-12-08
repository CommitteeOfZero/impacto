#include "musicmenu.h"

#include "../../profile/ui/extramenus.h"
#include "../../profile/games/mo6tw/musicmenu.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../renderer2d.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../data/savesystem.h"
#include "../../audio/audiochannel.h"
#include "../../vm/interface/input.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

using namespace Impacto::Profile::MO6TW::MusicMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets;

using namespace Impacto::Vm::Interface;

void MusicMenu::MusicButtonOnClick(Button* target) {
  if (target->IsLocked) return;

  Io::InputStream* stream;
  Io::VfsOpen("bgm", Playlist[target->Id], &stream);
  Audio::Channels[Audio::AC_BGM0].Play(Audio::AudioStream::Create(stream),
                                       PlaybackMode == 2, 0.0f);
  Thumbnail->SetSprite(Thumbnails[target->Id]);
  CurrentlyPlaying->SetSprite(ItemNames[target->Id]);
}

MusicMenu::MusicMenu() {
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  MainItems = new Group(this);
  MainItems->WrapFocus = false;
  BackgroundItems = new Group(this);
  BackgroundItems->FocusLock = false;

  BackgroundItems->Add(new Label(ItemsWindow, ItemsWindowPosition));
  BackgroundItems->Add(new Label(PlaybackWindow, PlaybackWindowPosition));
  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);
  Thumbnail = new Label(nullSprite, ThumbnailPosition);
  BackgroundItems->Add(Thumbnail);
  CurrentlyPlaying = new Label(nullSprite, CurrentlyPlayingLabelPosition);
  BackgroundItems->Add(CurrentlyPlaying);
  PlaybackModeLabel =
      new Label(PlaybackModeLabels[PlaybackMode], PlaybackModeLabelPosition);
  BackgroundItems->Add(PlaybackModeLabel);

  auto onClick =
      std::bind(&MusicMenu::MusicButtonOnClick, this, std::placeholders::_1);

  MainItems->RenderingBounds = ItemsWindowRenderingBounds;
  auto pos = MusicListInitialPosition;
  for (int i = 0; i < MusicTrackCount; i++) {
    auto button = new Button(i, ItemNames[i], ItemNames[i],
                             Profile::BacklogMenu::EntryHighlight, pos);
    button->HighlightOffset = ItemNameHighlightOffset;
    button->LockedSprite = LockedItem;
    button->OnClickHandler = onClick;
    MainItems->Add(button, FDIR_DOWN);

    pos += MusicListMargin;
  }

  MusicListY = ScrollbarStart;
  float totalHeight = MusicListMargin.y * (MusicTrackCount + 1);
  MainItems->Bounds =
      RectF(MusicListInitialPosition.x, MusicListInitialPosition.y,
            ItemNames[0].ScaledWidth(), totalHeight);
  auto scrollbar = new Scrollbar(
      0, ScrollbarPosition, ScrollbarStart,
      ScrollbarStart - totalHeight + ItemsWindowRenderingBounds.Height,
      &MusicListY, SBDIR_VERTICAL, ScrollbarTrack, ScrollbarThumb);
  BackgroundItems->Add(scrollbar);
}

void MusicMenu::Show() {
  if (State != Shown) {
    State = Showing;
    UpdateMusicEntries();
    FadeAnimation.StartIn();
    MainItems->Show();
    BackgroundItems->Show();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}
void MusicMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    FadeAnimation.StartOut();
    MainItems->Hide();
    BackgroundItems->Hide();
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void MusicMenu::UpdateInput() {
  Menu::UpdateInput();
  if (State == Shown) {
    MainItems->UpdateInput();
    BackgroundItems->UpdateInput();
    if ((PADinputButtonWentDown & PAD1DOWN ||
         PADinputButtonWentDown & PAD1UP) &&
        MainItems->HasFocus) {
      auto focusedEl = CurrentlyFocusedElement;
      if (focusedEl->Bounds.Y < MainItems->RenderingBounds.Y) {
        MusicListY += MusicListMargin.y;
      } else if (focusedEl->Bounds.Y + focusedEl->Bounds.Height >
                 MainItems->RenderingBounds.Y +
                     MainItems->RenderingBounds.Height) {
        MusicListY -= MusicListMargin.y;
      }
    }
  }
}

void MusicMenu::Update(float dt) {
  UpdateInput();

  FadeAnimation.Update(dt);
  if (ScrWork[SW_MUSICMODE_ALPHA] < 256 && State == Shown) {
    Hide();
  } else if (ScrWork[SW_MUSICMODE_ALPHA] == 256 && State == Hidden) {
    Show();
  }

  if (ScrWork[SW_MUSICMODE_ALPHA] == 256 && FadeAnimation.IsIn())
    State = Shown;
  else if (FadeAnimation.IsOut()) {
    State = Hidden;
  }

  if (State != Hidden) {
    BackgroundItems->Update(dt);
    MainItems->Update(dt);
    MainItems->MoveTo(glm::vec2(MainItems->Bounds.X, MusicListY));
  }
}

void MusicMenu::Render() {
  if (State != Hidden) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    Renderer2D::DrawSprite(BackgroundSprite, glm::vec2(0.0f, 0.0f), col);
    BackgroundItems->Tint = col;
    BackgroundItems->Render();
    MainItems->Tint = col;
    MainItems->Render();
  }
}

void MusicMenu::UpdateMusicEntries() {
  for (auto el : MainItems->Children) {
    auto button = static_cast<Button*>(el);
    button->IsLocked = !SaveSystem::GetBgmFlag(Playlist[button->Id]);
  }
}

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto