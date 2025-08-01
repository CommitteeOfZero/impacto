#pragma once

#include "../../ui/menu.h"
#include "../../ui/turboonholdhandler.h"
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
  void UpdateInput(float dt);
  void Update(float dt);
  void Render();

  // void MovieButtonOnClick(Widgets::Button* target);

 private:
  Widgets::Group* MainItems;

  void DrawCircles();
  void DrawErin();
  void DrawRedBar();

  void UpdateEntries();
  void UpdateTitles();
  void MusicButtonOnClick(Widgets::Button* target);
  void SwitchToTrack(int id);
  int GetNextTrackId(int id);

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

  glm::vec2 RedTitleLabelPos;
  glm::vec2 RightTitlePos;
  glm::vec2 LeftTitlePos;
  
  std::optional<FocusDirection> QueuedMove;
  TurboOnHoldHandler DirectionButtonHoldHandler;
  bool TurboMoved = false;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto