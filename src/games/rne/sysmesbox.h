#pragma once

#include "../../ui/sysmesbox.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace RNE {

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

  float BoxAnimCount = 0.0f;
  float BoxTopY = 0.0f;
  float BoxBottomY = 0.0f;
  float LineLength = 0.0f;
  float BoxHeight = 0.0f;
  float BoxProgressCount = 0.0f;
  float ButtonYesX = 0.0f;
  float ButtonRightX = 0.0f;
  int TextStartCount = 0;
};

}  // namespace RNE
}  // namespace UI
}  // namespace Impacto