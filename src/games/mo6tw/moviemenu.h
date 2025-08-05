#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/mo6tw/imagethumbnailbutton.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

class MovieMenu : public Menu {
 public:
  MovieMenu();

  void Show();
  void Hide();
  void UpdateInput(float dt);
  void Update(float dt);
  void Render();

  void MovieButtonOnClick(Widgets::Button* target);

 private:
  Widgets::Group* MainItems;

  void UpdateMovieEntries();

  Animation FadeAnimation;
};

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto