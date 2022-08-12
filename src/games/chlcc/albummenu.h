#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../vm/interface/input.h"
//#include "../../ui/widgets/chlcc/imagethumbnailbutton.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class AlbumMenu : public Menu {
 public:
  AlbumMenu();

  void Show();
  void Hide();
  void UpdateInput();
  void Update(float dt);
  void Render();

  // void MovieButtonOnClick(Widgets::Button* target);

 private:
  // Widgets::Group* MainItems;

  inline void DrawCircles();
  inline void DrawErin();
  inline void DrawRedBar();
  // inline void DrawTitles();

  // void UpdateMovieEntries();

  // Animation FadeAnimation;

  Animation MenuTransition;
  Animation TitleFade;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto