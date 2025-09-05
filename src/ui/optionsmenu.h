#pragma once

#include "menu.h"
#include "turboonholdhandler.h"
#include "widgets/group.h"
#include "../profile/configsystem.h"

namespace Impacto {
namespace UI {

class OptionsMenu : public Menu {
 public:
  virtual void Show() override;
  virtual void Hide() override;
  virtual void Update(float dt) override;
  virtual void UpdateInput(float dt) override;
  virtual void ResetToDefault() { Profile::ConfigSystem::ResetToDefault(); };

 protected:
  OptionsMenu();

  virtual void GoToPage(size_t pageNumber);
  virtual void UpdatePageInput(float dt);
  virtual void UpdateEntryMovementInput(float dt);
  virtual void UpdateVisibility() = 0;

  virtual void Highlight(Widget* toHighlight);

  Animation FadeAnimation;

  size_t CurrentPage = 0;
  std::vector<std::unique_ptr<Widgets::Group>> Pages;

  TurboOnHoldHandler DirectionButtonHeldHandler;
  TurboOnHoldHandler PageButtonHeldHandler;
};

}  // namespace UI
}  // namespace Impacto