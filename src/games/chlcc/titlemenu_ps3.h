#pragma once

#include "titlemenu.h"

namespace Impacto {
namespace UI {
namespace CHLCC_PS3 {

class TitleMenu final : public CHLCC::TitleMenu {
 public:
  TitleMenu();

  void MenuButtonOnClick(Widgets::Button* target) override;
  void SecondaryButtonOnClick(Widgets::Button* target);

  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void Render() override;
};

}  // namespace CHLCC_PS3
}  // namespace UI
}  // namespace Impacto