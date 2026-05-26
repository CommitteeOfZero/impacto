#pragma once

#include <optional>
#include <glm/fwd.hpp>
#include "commonmenu.h"
#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../profile/games/cclcc/systemmenu.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

class SystemMenu : public Menu, public CommonMenu {
 public:
  SystemMenu();

  void Init() override;
  void Show() override;
  void Hide() override;
  void UpdateInput(float dt) override;
  void Update(float dt) override;
  void Render() override;

  void MenuButtonOnClick(Widgets::Button* target);
  Sprite ScreenCap;

 private:
  Widgets::Group* MainItems;
  Animation MenuTransition;
  Animation FadeAnimation;
  Animation ItemsFade;
  bool ItemsFadeComplete = false;
  std::optional<int> LastFocusedButtonId;

  enum class MenuItems : size_t {
    Backlog,
    QuickSave,
    QuickLoad,
    Save,
    Load,
    TipsList,
    Config,
    Help,
    ReturnTitle
  };
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto