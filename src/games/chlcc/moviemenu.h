#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/chlcc/moviemenuentrybutton.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class MovieMenu : public Menu {
 public:
  MovieMenu();

  void Show();
  void Hide();
  void UpdateInput();
  void Update(float dt);
  void Render();

  void MovieButtonOnClick(Widgets::Button* target);

 private:
  Widgets::Group* MovieItems;

  void DrawCircles();
  void DrawErin();
  void DrawRedBar();
  void DrawButtonPrompt();
  void DrawSelectMovie(float yOffset);
  void UpdateTitles();

  void UpdateMovieEntries();

  Animation MenuTransition;
  Animation TitleFade;
  Animation SelectMovieTextFade;

  glm::vec2 RedTitleLabelPos;
  glm::vec2 RightTitlePos;
  glm::vec2 LeftTitlePos;

  bool IsChoiceMadeOnce = false;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto