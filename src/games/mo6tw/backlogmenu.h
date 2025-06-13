#pragma once

#include "../../ui/backlogmenu.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

class BacklogMenu : public UI::BacklogMenu {
 public:
  void Render() override;

 private:
  void UpdateVisibility() override;
};

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto
