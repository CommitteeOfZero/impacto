#pragma once

#include <optional>
#include <glm/fwd.hpp>
#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../profile/games/cclcc/systemmenu.h"

namespace Impacto {
namespace UI {
namespace CCLCC {

class SystemMenu : public Menu {
 public:
  using GridVertices =
      std::array<VertexBufferSprites,
                 (Profile::CCLCC::SystemMenu::GridRowCount + 1) *
                     (Profile::CCLCC::SystemMenu::GridColCount + 1)>;

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
  Animation MenuFade;
  Animation ItemsFade;
  bool ItemsFadeComplete = false;
  glm::vec2 BGPosition{};
  std::optional<int> LastFocusedButtonId;

  GridVertices Vertices;

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