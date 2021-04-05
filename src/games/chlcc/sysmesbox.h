#pragma once

#include "../../ui/sysmesbox.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/label.h"
#include "../../text.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class SysMesBox : public UI::SysMesBox {
 public:
  virtual void Show() override;
  virtual void Hide() override;
  virtual void Update(float dt) override;
  virtual void Render() override;

  virtual void Init();
  virtual void AddMessage(uint8_t* str) override;
  virtual void AddChoice(uint8_t* str) override;

 private:
  void ChoiceItemOnClick(UI::Widgets::Button* target);
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto