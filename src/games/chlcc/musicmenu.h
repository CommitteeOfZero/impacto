#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
// #include "../../ui/widgets/chlcc/imagethumbnailbutton.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

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

  Animation MenuTransition;
  Animation TitleFade;

  bool InputEnabled = false;

  int CurrentlyPlayingTrackId = -1;
  int CurrentLowerBound = 0;
  int CurrentUpperBound = 15;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto