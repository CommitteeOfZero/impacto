#pragma once

#include "titlemenu.h"

namespace Impacto {
namespace UI {
namespace CHLCC_Switch {

class TitleMenu final : public CHLCC::TitleMenu {
 public:
  TitleMenu();

  void ContinueButtonOnClick(Widgets::Button* target);
  void ExtraButtonOnClick(Widgets::Button* target);
  void MenuButtonOnClick(Widgets::Button* target) override;

  // void Show() override;
  // void Hide() override;
  void Update(float dt) override;
  void Render() override;
};

}  // namespace CHLCC_Switch
}  // namespace UI
}  // namespace Impacto