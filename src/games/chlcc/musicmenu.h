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

  void Show() override;
  void Hide() override;
  void UpdateInput(float dt) override;
  void Update(float dt) override;
  void Render() override;

  // void MovieButtonOnClick(Widgets::Button* target);

 private:
  Widgets::Group* MainItems;

  void DrawCircles();
  void DrawErin();
  void DrawRedBar();
  void DrawButtonPrompt();

  void UpdateEntries();
  void UpdateTitles();
  void MusicButtonOnClick(Widgets::Button* target);
  void SwitchToTrack(int id);
  int GetNextTrackId(int id);

  Animation MenuTransition;
  Animation TitleFade;
  Animation NowPlayingAnimation;
  Animation FromSystemMenuTransition;

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
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto