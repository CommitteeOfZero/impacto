#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/cgviewer.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

class AlbumMenu : public Menu {
 public:
  AlbumMenu();

  void Show();
  void Hide();
  void UpdateInput();
  void Update(float dt);
  void Render();

  void CharacterButtonOnClick(Widgets::Button* target);
  void ArrowUpOnClick(Widgets::Button* target);
  void ArrowDownOnClick(Widgets::Button* target);
  void CgOnClick(Widgets::Button* target);
  void OnCgVariationEnd(Widgets::CgViewer* target);

 private:
  Widgets::Group* MainItems;
  Widgets::Group* ImageGrid;
  Widgets::Group* SecondaryItems;
  Widgets::Group* Arrows;
  Widgets::Button* ArrowUpButton;
  Widgets::Button* ArrowDownButton;

  Widgets::Group* CgViewerGroup;
  Widgets::CgViewer* CgViewerWidget;

  float MinimumImageGridY;
  float MaximumImageGridY;

  int SelectedCharacterId = -1;
  bool ShowCgViewer = false;

  Animation FadeAnimation;
  Animation ArrowsAnimation;

  void SwitchToCharacter(int id);
  void LoadCharacter(int id);
  void MoveImageGrid();
};

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto