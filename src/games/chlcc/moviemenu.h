#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/chlcc/moviemenuentrybutton.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

struct MovieButtonEntry {
  int PhysicalId;
  int ExtraId;
};

static const std::map<int, MovieButtonEntry> movieButtonMap = {
    {0, {10, 18}},  // PS3 OP / PSP OP
    {1, {0, 0}},    // X360 OP
    {2, {1, 11}},   // Rimi ED
    {3, {2, 12}},   // Nanami ED
    {4, {4, 14}},   // Yua ED
    {5, {5, 15}},   // Mia ED
    {6, {6, 16}},   // Ayase ED
    {7, {3, 13}},   // Sena ED
    {8, {7, 17}},   // Kozue ED
    {9, {8, 8}}     // Seira ED
};

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
  bool IsExtraMovieModeOn = false;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto