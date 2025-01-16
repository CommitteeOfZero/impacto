#pragma once

#include "menu.h"
#include "widgets/group.h"

namespace Impacto {
namespace UI {

class OptionsMenu : public Menu {
 public:
  virtual void Show() override;
  virtual void Hide() override;
  virtual void Update(float dt) override;
  virtual void UpdateInput(float dt);

 protected:
  OptionsMenu();

  virtual void GoToPage(int pageNumber);
  virtual void UpdatePageInput(float dt);
  virtual void UpdateEntryMovementInput(float dt);

  virtual void Highlight(Widget* toHighlight);

  Animation FadeAnimation;

  int CurrentPage = 0;
  std::vector<std::unique_ptr<Widgets::Group>> Pages;

  float DirectionButtonHeldTime = 0.0f;
  float DirectionButtonWaitTime = 0.0f;

  float PageDirectionButtonHeldTime = 0.0f;
  float PageDirectionButtonWaitTime = 0.0f;
};

}  // namespace UI
}  // namespace Impacto