#pragma once

#include "../../ui/optionsmenu.h"
#include "../../spriteanimation.h"
#include "../../ui/widgets/clickarea.h"
#include "../../ui/widgets/cclcc/optionsentry.h"

using namespace Impacto::UI::Widgets::CCLCC;

namespace Impacto {
namespace UI {
namespace CCLCC {

class OptionsMenu : public UI::OptionsMenu {
 public:
  OptionsMenu();

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void UpdateInput(float dt) override;
  void Render() override;

 private:
  void PageButtonOnHover(int pageNumber);

  void Select(OptionsEntry* entry);
  void Highlight(Widget* entry) override;
  bool AnyEntrySelected() {
    return CurrentlyFocusedElement &&
           static_cast<OptionsEntry*>(CurrentlyFocusedElement)->Selected;
  }

  void UpdatePageInput(float dt) override;
  void UpdateEntryMovementInput(float dt) override;

  SpriteAnimation PoleAnimation;

  std::vector<Widgets::ClickArea> PageButtons;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto