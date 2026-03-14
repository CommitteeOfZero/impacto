#pragma once

#include "commonmenu.h"
#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

struct MovieButtonEntry {
  int PhysicalId;
  int ExtraId;
};
class MovieMenu : public Menu, public CommonMenu {
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

  using CommonMenu::DrawButtonPrompt;
  void DrawButtonPrompt();
  void UpdateMovieEntries();

  bool IsChoiceMadeOnce = false;
  bool IsExtraMovieModeOn = false;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto