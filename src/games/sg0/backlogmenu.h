#pragma once

#include "../../ui/backlogmenu.h"

namespace Impacto {
namespace UI {
namespace SG0 {

class BacklogMenu : public UI::BacklogMenu<Widgets::BacklogEntry> {
 public:
  void Hide() override;
  void Render() override;

 protected:
  void UpdateVisibility() override;

 private:
  int LastAlpha = 0;
};

}  // namespace SG0
}  // namespace UI
}  // namespace Impacto
