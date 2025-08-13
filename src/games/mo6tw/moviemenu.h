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

  void Show() override;
  void Hide() override;
  void UpdateInput(float dt) override;
  void Update(float dt) override;
  void Render() override;

  void MovieButtonOnClick(Widgets::Button* target);

 private:
  Widgets::Group* MainItems;

  void UpdateMovieEntries();

  Animation FadeAnimation;
};

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto