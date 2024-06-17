#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../vm/interface/input.h"
#include "../../ui/widgets/cgviewer.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class AlbumMenu : public Menu {
 public:
  AlbumMenu();

  void Show();
  void Hide();
  void UpdateInput();
  void Update(float dt);
  void Render();

 private:
  std::vector<Widgets::Group *> Pages;

  void DrawCircles();
  void DrawErin();
  void DrawRedBar();
  void DrawButtonGuide();

  void DrawPage(const glm::vec2 &offset);

  void UpdatePages();
  void UpdateTitles();

  void CgOnClick(Widgets::Button *target);
  void OnCgVariationEnd(Widgets::CgViewer *target);

  Animation MenuTransition;
  Animation TitleFade;

  int CurrentPage;
  int MaxReachablePage;

  Widgets::Group *CgViewerGroup;
  Widgets::CgViewer *CgViewerWidget;
  bool ShowCgViewer = false;

  glm::vec2 RedTitleLabelPos;
  glm::vec2 RightTitlePos;
  glm::vec2 LeftTitlePos;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto