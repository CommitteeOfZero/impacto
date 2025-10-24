#pragma once

#include "../../ui/menu.h"
#include "../../ui/savemenu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class SystemMenu : public Menu {
 public:
  SystemMenu();

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void Render() override;

  void MenuButtonOnClick(Widgets::Button* target);

 private:
  void UpdateMenuLoop();
  void UpdateTitles();
  void DrawRedBar();
  void DrawButtonPrompt();
  void DrawSelectMenu(float yOffset);
  void DrawRunningSelectedLabel(float offsetY);
  void UpdateRunningSelectedLabel(float dt);
  void UpdateSmoothSelection(float dt);
  void DrawCircles();
  void DrawErin();

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

  float CurrentRunningPosition = 0.0f;
  float SelectionOffsetY = 0.0f;
  int IndexOfActiveButton = 0;
  glm::vec2 RedTitleLabelPos;
  glm::vec2 RightTitlePos;
  glm::vec2 LeftTitlePos;

  Widgets::Group* MainItems;
  Animation MenuTransition;
  Animation TitleFade;
  Animation MenuLoop;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto