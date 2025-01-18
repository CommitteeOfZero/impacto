#pragma once

#include "../../ui/optionsmenu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/scrollbar.h"
#include "../../ui/widgets/toggle.h"
#include "../../ui/widgets/optiongroup.h"

namespace Impacto {
namespace UI {
namespace MO8 {

class OptionsMenu : public UI::OptionsMenu {
 public:
  OptionsMenu();

  void Hide() override;
  void Update(float dt) override;
  void UpdateInput(float dt) override;
  void Render() override;

  void NextPageOnClick(Widgets::Button* target);
  void PreviousPageOnClick(Widgets::Button* target);
  void MessageSpeedToggleOnClick(Widgets::Toggle* target);
  void AutoModeWaitTimeOnClick(Widgets::Toggle* target);
  void SkipModeOnClick(Widgets::Toggle* target);

 private:
  void UpdatePageInput(float dt) override;
  inline void UpdateVisibility() override;

  void GoToPage(int pageNumber) override;
  Animation PageFadeAnimation;
  int PreviousPage = -1;

  Widgets::Group* PageControls;

  bool MessageSpeedValues[4] = {false};
  bool AutoModeWaitTimeValues[3] = {false};
  bool SkipModeValues[2] = {false};
};

}  // namespace MO8
}  // namespace UI
}  // namespace Impacto