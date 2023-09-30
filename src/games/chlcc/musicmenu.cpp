#include "musicmenu.h"

#include "../../profile/games/chlcc/musicmenu.h"
#include "../../profile/scriptvars.h"
#include "../../profile/profile_internal.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../data/savesystem.h"
#include "../../profile/dialogue.h"

#include "../../ui/widgets/chlcc/trackselectbutton.h"

namespace Impacto {
namespace UI {
namespace CHLCC {
using namespace Impacto::Profile::CHLCC::MusicMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets;

void MusicMenu::MusicButtonOnClick(Button* target) {
  if (target->IsLocked) return;

  SwitchToTrack(target->Id);
}

MusicMenu::MusicMenu() {
  MenuTransition.Direction = 1.0f;
  MenuTransition.LoopMode = ALM_Stop;
  MenuTransition.DurationIn = MenuTransitionDuration;
  MenuTransition.DurationOut = MenuTransitionDuration;

  TitleFade.Direction = 1.0f;
  TitleFade.LoopMode = ALM_Stop;
  TitleFade.DurationIn = TitleFadeInDuration;
  TitleFade.DurationOut = TitleFadeOutDuration;

  RedBarSprite = InitialRedBarSprite;
  RedBarPosition = InitialRedBarPosition;

  MainItems = new Group(this);
  Sprite empty = Sprite();

  auto onClick =
      std::bind(&MusicMenu::MusicButtonOnClick, this, std::placeholders::_1);

  for (int idx = 0; idx < MusicTrackCount; idx++) {
    auto button = new Widgets::CHLCC::TrackSelectButton(
        idx, TrackHighlight, TrackButtonPosTemplate + (float)idx * TrackOffset,
        TrackNameOffset, ArtistOffset);
    button->OnClickHandler = onClick;
    MainItems->Add(button, FDIR_DOWN);
  }

  MainItems->Children.front()->SetFocus(MainItems->Children.back(), FDIR_UP);
  MainItems->Children.back()->SetFocus(MainItems->Children.front(), FDIR_DOWN);
}

void MusicMenu::Show() {
  if (State != Shown) {
    if (State != Showing) MenuTransition.StartIn();
    State = Showing;
    //    MainItems->Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);
    UpdateEntries();
    MainItems->Show();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
    MainItems->Children.front()->HasFocus = true;
    CurrentlyFocusedElement = MainItems->Children.front();
  }
}

void MusicMenu::Hide() {
  if (State != Hidden) {
    if (State != Hiding) {
      MenuTransition.StartOut();
    }
    State = Hiding;
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
    InputEnabled = false;
  }
}

void MusicMenu::Render() {
  if (State != Hidden) {
    if (MenuTransition.IsIn()) {
      Renderer->DrawRect(RectF(0.0f, 0.0f, 1280.0f, 720.0f),
                         RgbIntToFloat(BackgroundColor));
    } else {
      DrawCircles();
    }
    DrawErin();
    //    auto test = Vm::ScriptGetTextTableStrAddress(4, 1);
    //    auto l = Label(test, glm::vec2(100, 100), 24, false);
    //    l.Render();
    glm::vec3 tint = {1.0f, 1.0f, 1.0f};
    // Alpha goes from 0 to 1 in half the time
    float alpha =
        MenuTransition.Progress < 0.5f ? MenuTransition.Progress * 2.0f : 1.0f;
    Renderer->DrawSprite(BackgroundFilter, RectF(0.0f, 0.0f, 1280.0f, 720.0f),
                         glm::vec4(tint, alpha));
    DrawRedBar();
    // DrawTitles();
    glm::vec2 offset(0.0f, 0.0f);
    if (MenuTransition.Progress > 0.22f) {
      if (MenuTransition.Progress < 0.73f) {
        // Approximated function from the original, another mess
        offset = glm::vec2(
            0.0f,
            glm::mix(-720.0f, 0.0f,
                     1.00397f * std::sin(3.97161f -
                                         3.26438f * MenuTransition.Progress) -
                         0.00295643f));

        MainItems->RenderingBounds =
            RectF(0.0f, TrackButtonPosTemplate.y + offset.y, 1280.0f,
                  16 * TrackOffset.y + 1);
        MainItems->MoveTo(offset);
        for (auto button : MainItems->Children)
          static_cast<Widgets::CHLCC::TrackSelectButton*>(button)->MoveTracks(
              offset);
      }
      MainItems->Render();
      Renderer->DrawSprite(TrackTree, TrackTreePos + offset);
    }
  }
}

void MusicMenu::Update(float dt) {
  UpdateInput();
  if (ScrWork[SW_SYSMENUCT] < 10000 && State == Shown) {
    Hide();
  } else if (GetFlag(SF_SOUNDMENU) && ScrWork[SW_SYSMENUCT] > 0 &&
             State == Hidden) {
    Show();
  }

  if (MenuTransition.IsOut() && State == Hiding) {
    MainItems->Hide();
    State = Hidden;
  } else if (MenuTransition.IsIn() && State == Showing) {
    State = Shown;
    MainItems->RenderingBounds =
        RectF(0.0f, TrackButtonPosTemplate.y, 1280.0f, 16 * TrackOffset.y + 1);
    MainItems->MoveTo({0.0f, 0.0f});
    for (auto el : MainItems->Children)
      static_cast<Widgets::CHLCC::TrackSelectButton*>(el)->MoveTracks(
          {0.0f, 0.0f});
    InputEnabled = true;
  }

  if (State != Hidden) {
    MenuTransition.Update(dt);
    if (MenuTransition.Direction == -1.0f && MenuTransition.Progress <= 0.72f) {
      TitleFade.StartOut();
    } else if (MenuTransition.IsIn() &&
               (TitleFade.Direction == 1.0f || TitleFade.IsOut())) {
      TitleFade.StartIn();
    }
    TitleFade.Update(dt);
    if (InputEnabled) MainItems->Update(dt);
  }
}

void MusicMenu::UpdateInput() {
  Menu::UpdateInput();
  if (State == Shown) {
    MainItems->UpdateInput();
    auto button = static_cast<Widgets::CHLCC::TrackSelectButton*>(
        CurrentlyFocusedElement);
    if (button == nullptr) return;

    // Opposite of scroll direction
    if (button->Id - CurrentLowerBound >= 16) {
      CurrentLowerBound = button->Id - 15;
      CurrentUpperBound = button->Id;
    } else if (CurrentUpperBound - button->Id >= 16) {
      CurrentLowerBound = button->Id;
      CurrentUpperBound = button->Id + 15;
    } else
      return;

    glm::vec2 offset(0.0f, -(float)CurrentLowerBound * TrackOffset.y);
    MainItems->MoveTo(offset);
    for (auto el : MainItems->Children) {
      auto b = static_cast<Widgets::CHLCC::TrackSelectButton*>(el);
      b->MoveTracks(offset);
    }
  }
}

inline void MusicMenu::DrawCircles() {
  float y = CircleStartPosition.y;
  int resetCounter = 0;
  // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
  // duration is totalframes/60
  float progress = MenuTransition.Progress * MenuTransitionDuration * 60.0f;
  for (int line = 0; line < 4; line++) {
    int counter = resetCounter;
    float x = CircleStartPosition.x;
    for (int col = 0; col < 7; col++) {
      if (counter + 1 <= (progress)) {
        float scale = ((progress) - (counter + 1.0f)) * 16.0f;
        scale = scale <= 320.0f ? scale : 320.0f;
        scale *= CircleSprite.Bounds.Height / 106.0f;
        Renderer->DrawSprite(
            CircleSprite, RectF(x + (CircleSprite.Bounds.Width - scale) / 2.0f,
                                y + (CircleSprite.Bounds.Height - scale) / 2.0f,
                                scale, scale));
        x += CircleOffset;
      }
      counter += 2;
    }
    y += CircleOffset;
    resetCounter += 2;
  }
}

inline void MusicMenu::DrawErin() {
  float y = ErinPosition.y;
  if (MenuTransition.Progress < 0.78f) {
    y = 801.0f;
    if (MenuTransition.Progress > 0.22f) {
      // Approximation from the original function, which was a bigger
      // mess
      y = glm::mix(
          -19.0f, 721.0f,
          0.998938f -
              0.998267f * sin(3.97835f - 3.27549f * MenuTransition.Progress));
    }
  }
  Renderer->DrawSprite(ErinSprite, glm::vec2(ErinPosition.x, y));
}

inline void MusicMenu::DrawRedBar() {
  if (MenuTransition.IsIn()) {
    Renderer->DrawSprite(InitialRedBarSprite, InitialRedBarPosition);
  } else if (MenuTransition.Progress > 0.70f) {
    // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
    // duration is totalframes/60
    float progress = MenuTransition.Progress * MenuTransitionDuration * 60.0f;
    float pixelPerAdvanceLeft = RedBarBaseX * (progress - 47.0f) / 17.0f;
    RedBarSprite.Bounds.X = RedBarDivision - pixelPerAdvanceLeft;
    RedBarSprite.Bounds.Width = pixelPerAdvanceLeft;
    RedBarPosition.x = RedBarBaseX - pixelPerAdvanceLeft;
    Renderer->DrawSprite(RedBarSprite, RedBarPosition);
    float pixelPerAdvanceRight = 13.0f * (progress - 47.0f);
    RedBarSprite.Bounds.X = RedBarDivision;
    RedBarSprite.Bounds.Width = pixelPerAdvanceRight;
    RedBarPosition = RightRedBarPosition;
    Renderer->DrawSprite(RedBarSprite, RedBarPosition);
  }
}

void MusicMenu::UpdateEntries() {
  for (int idx = 0; idx < MainItems->Children.size(); idx++) {
    auto button = static_cast<Widgets::CHLCC::TrackSelectButton*>(
        MainItems->Children[idx]);
    button->SetTrackText(Vm::ScriptGetTextTableStrAddress(4, idx * 3));
    button->SetArtistText(Vm::ScriptGetTextTableStrAddress(4, idx * 3 + 1));
  }
}

void MusicMenu::SwitchToTrack(int id) {
  CurrentlyPlayingTrackId = id;
  if (id == -1) {
    Audio::Channels[Audio::AC_BGM0].Stop(0.5f);
    return;
  }
  Io::InputStream* stream;
  Io::VfsOpen("bgm", Playlist[id], &stream);
  Audio::Channels[Audio::AC_BGM0].Play(Audio::AudioStream::Create(stream), 0,
                                       0.5f);
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto