#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/scrollbar.h"
#include "../../ui/widgets/toggle.h"
#include "../../ui/widgets/optiongroup.h"

namespace Impacto {
namespace UI {
namespace MO8 {

class OptionsMenu : public Menu {
 public:
  OptionsMenu();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  void NextPageOnClick(Widgets::Button* target);
  void PreviousPageOnClick(Widgets::Button* target);
  void MessageSpeedToggleOnClick(Widgets::Toggle* target);
  void AutoModeWaitTimeOnClick(Widgets::Toggle* target);
  void SkipModeOnClick(Widgets::Toggle* target);

 private:
  void GoToNextPage();
  void GoToPreviousPage();

  Animation FadeAnimation;
  Animation PageFadeAnimation;

  Widgets::Group* PageControls;
  Widgets::Group* TextPage;
  Widgets::Group* SoundPage1;
  Widgets::Group* SoundPage2;
  Widgets::Group* OtherPage;

  std::vector<Widgets::Group*> Pages;
  std::vector<Widgets::Group*>::iterator CurrentPage;
  std::vector<Widgets::Group*>::iterator PreviousPage;

  bool MessageSpeedValues[4] = {false};
  bool AutoModeWaitTimeValues[3] = {false};
  bool SkipModeValues[2] = {false};
};

}  // namespace MO8
}  // namespace UI
}  // namespace Impacto