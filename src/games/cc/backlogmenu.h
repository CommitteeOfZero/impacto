#pragma once

#include "../../ui/backlogmenu.h"

namespace Impacto {
namespace UI {
namespace CC {

class BacklogMenu : public UI::BacklogMenu {
 public:
  BacklogMenu();

  void Init();

  void Show();
  void Hide();
  void UpdateInput();
  void Update(float dt);
  void Render();
};
  


} // namespace CC
} // namespace UI
} // namespace Impacto