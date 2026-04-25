#pragma once

#include "commonmenu.h"
#include "../../ui/menu.h"
#include "../../ui/widgets/group.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/cgviewer.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class AlbumMenu : public Menu, public CommonMenu {
 public:
  AlbumMenu();

  void Init() override;
  void Show() override;
  void Hide() override;
  void UpdateInput(float dt) override;
  void Update(float dt) override;
  void Render() override;

 private:
  std::vector<Widgets::Group*> Pages;

  void DrawPage();

  void UpdatePages();

  void CgOnClick(Widgets::Button* target);
  void OnCgVariationEnd(Widgets::CgViewer* target);

  Widgets::Group* CgViewerGroup;
  Widgets::CgViewer* CgViewerWidget;
  Animation ButtonGuideFade = Animation();

  int PrevPage = 0;
  int CurrentPage;
  int MaxReachablePage;

  bool ShowCgViewer = false;
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto