#pragma once

#include "../../ui/optionsmenu.h"
#include "../../spriteanimation.h"
#include "../../ui/widgets/clickarea.h"
#include "../../ui/widgets/cclcc/optionsentry.h"
#include "../../ui/widgets/group.h"

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
  std::unique_ptr<Widgets::Group> CreateBasicPage(
      const std::function<void(OptionsEntry*)>& select,
      const std::function<void(Widget*)>& highlight);
  std::unique_ptr<Widgets::Group> CreateTextPage(
      const std::function<void(OptionsEntry*)>& select,
      const std::function<void(Widget*)>& highlight);
  std::unique_ptr<Widgets::Group> CreateSoundPage(
      const std::function<void(OptionsEntry*)>& select,
      const std::function<void(Widget*)>& highlight);
  std::unique_ptr<Widgets::Group> CreateVoicePage(
      const std::function<void(OptionsEntry*)>& select,
      const std::function<void(Widget*)>& highlight);

  void PageButtonOnHover(int pageNumber);

  void Select(OptionsEntry* entry);
  void Highlight(Widget* entry) override;
  bool AnyEntrySelected() {
    return CurrentlyFocusedElement &&
           static_cast<OptionsEntry*>(CurrentlyFocusedElement)->Selected;
  }

  void UpdatePageInput(float dt) override;
  void UpdateEntryMovementInput(float dt) override;
  void UpdateVisibility() override;

  SpriteAnimation PoleAnimation;

  std::vector<Widgets::ClickArea> PageButtons;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto