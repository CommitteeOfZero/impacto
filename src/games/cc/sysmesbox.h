#pragma once

#include "../../ui/sysmesbox.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace CC {

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
  UI::Widgets::Button* WidgetOK;
  UI::Widgets::Button* WidgetYes;
  UI::Widgets::Button* WidgetNo;
};

}  // namespace CC
}  // namespace UI
}  // namespace Impacto