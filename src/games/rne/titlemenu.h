#pragma once

#include "../../ui/menu.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace RNE {

class TitleMenu : public Menu {
 public:
  TitleMenu();

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void Render() override;
};

}  // namespace RNE
}  // namespace UI
}  // namespace Impacto