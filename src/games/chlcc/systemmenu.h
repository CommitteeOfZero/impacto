#pragma once

#include <optional>
#include "commonmenu.h"
#include "animations/menutransition.h"
#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class SystemMenu : public Menu, public CommonMenu {
 public:
  SystemMenu();

  void Show() override;
  void Hide() override;
  void SubItemShow();
  void SubItemsHide(bool instantHide);
  void Update(float dt) override;
  void Render() override;

  void MenuButtonOnClick(Widgets::Button* target);

 private:
  glm::vec4 GetCurrentBgColor();

  using CommonMenu::UpdateTitles;
  void UpdateTitles();
  void UpdateRightTitle();
  void DrawRunningSelectedLabel(float offsetY);
  void UpdateRunningSelectedLabel(float dt);
  void UpdateSmoothSelection(float dt);

  enum class MenuItems : size_t {
    Backlog,
    Save,
    Load,
    QuickSave,
    QuickLoad,
    Config,
    TipsList,
    Trophy,
    ReturnTitle
  };

  MenuTransitionAnimation SubItemsTransition;
  Widgets::Group* MainItems;

  float CurrentRunningPosition = 0.0f;
  float SelectionOffsetY = 0.0f;
  int IndexOfActiveButton = 0;
  std::optional<int> LastFocusedButtonId;
  uint32_t CurrentColor;
  MenuState SubItemsState = Hidden;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto