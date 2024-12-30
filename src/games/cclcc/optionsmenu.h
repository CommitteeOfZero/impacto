#pragma once

#include "../../ui/menu.h"
#include "../../spriteanimation.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/clickbutton.h"
#include "../../ui/widgets/cclcc/optionsentry.h"

using namespace Impacto::UI::Widgets::CCLCC;

namespace Impacto {
namespace UI {
namespace CCLCC {

class OptionsMenu : public Menu {
 public:
  OptionsMenu();

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void UpdateInput(float dt);
  void Render() override;

 private:
  void GoToPage(int pageNumber);
  void Select(OptionsEntry* entry);
  void PageButtonOnHover(int pageNumber);

  void UpdatePageInput(float dt);
  void UpdateEntryMovementInput(float dt);

  Animation FadeAnimation;
  SpriteAnimation PoleAnimation;

  Widgets::Group* BasicPage;
  Widgets::Group* TextPage;
  Widgets::Group* SoundPage;
  Widgets::Group* VoicePage;

  int CurrentPage;
  std::vector<Widgets::Group*> Pages;
  std::vector<Widgets::ClickButton> PageButtons;

  float DirectionButtonHeldTime = 0.0f;
  float DirectionButtonWaitTime = 0.0f;

  float PageDirectionButtonHeldTime = 0.0f;
  float PageDirectionButtonWaitTime = 0.0f;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto