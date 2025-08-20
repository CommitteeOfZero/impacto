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

  void Show() override;
  void Hide() override;
  void UpdateInput(float dt) override;
  void Update(float dt) override;
  void Render() override;

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
  Animation FromSystemMenuTransition;

  glm::vec2 RedTitleLabelPos;
  glm::vec2 RightTitlePos;
  glm::vec2 LeftTitlePos;

  bool IsChoiceMadeOnce = false;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto