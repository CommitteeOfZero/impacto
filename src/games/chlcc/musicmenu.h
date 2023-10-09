#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/label.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

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

  // void MovieButtonOnClick(Widgets::Button* target);

 private:
  Widgets::Group* MainItems;

  inline void DrawCircles();
  inline void DrawErin();
  inline void DrawRedBar();

  void UpdateEntries();

  void MusicButtonOnClick(Widgets::Button* target);
  void SwitchToTrack(int id);
  inline int GetNextTrackId(int id);

  Animation MenuTransition;
  Animation TitleFade;
  Animation NowPlayingAnimation;

  bool InputEnabled = false;

  int CurrentlyPlayingTrackId = -1;
  int CurrentLowerBound = 0;
  int CurrentUpperBound = 15;
  float PreviousPosition = 0.0f;
  MusicPlaybackMode PlaybackMode = MPM_One;
  Sprite PlaymodeRepeatSprite;
  Sprite PlaymodeAllSprite;
  Widgets::Label CurrentlyPlayingTrackName;
  Widgets::Label CurrentlyPlayingTrackArtist;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto