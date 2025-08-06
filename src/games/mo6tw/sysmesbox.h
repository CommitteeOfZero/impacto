#pragma once

#include "../../ui/sysmesbox.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

class SysMesBox : public UI::SysMesBox {
 public:
  virtual void Show() override;
  virtual void Hide() override;
  virtual void Update(float dt) override;
  virtual void Render() override;

  virtual void Init() override;
  virtual void AddMessage(Vm::BufferOffsetContext ctx) override;
  virtual void AddChoice(Vm::BufferOffsetContext ctx) override;

 private:
  void ChoiceItemOnClick(UI::Widgets::Button* target);
};

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto