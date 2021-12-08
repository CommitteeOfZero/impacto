#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/label.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

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

  Widgets::Group* MainItems;
  Widgets::Group* BackgroundItems;
  Widgets::Label* Thumbnail;
  Widgets::Label* CurrentlyPlaying;
  Widgets::Label* PlaybackModeLabel;
  Animation FadeAnimation;

  float MusicListY;
  int PlaybackMode = 0;
};

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto