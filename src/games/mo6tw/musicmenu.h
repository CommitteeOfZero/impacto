#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/label.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

enum MusicPlaybackMode {
  MPM_One,
  MPM_Playlist,
  MPM_RepeatOne,
  MPM_RepeatPlaylist
};

class MusicMenu : public Menu {
 public:
  MusicMenu();

  void Show();
  void Hide();
  void UpdateInput();
  void Update(float dt);
  void Render();

  void MusicButtonOnClick(Widgets::Button* target);

 private:
  void UpdateMusicEntries();
  void UpdateMusicTimer();
  void SwitchToTrack(int id);
  int GetNextTrackId(int id);

  Widgets::Group* MainItems;
  Widgets::Group* BackgroundItems;
  Widgets::Group* Timer;
  Widgets::Label* Thumbnail;
  Widgets::Label* CurrentlyPlaying;
  Widgets::Label* PlaybackModeLabel;
  Animation FadeAnimation;
  Sprite NullSprite;

  float MusicListY;
  MusicPlaybackMode PlaybackMode = MPM_One;
  int CurrentlyPlayingTrackId = -1;
};

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto