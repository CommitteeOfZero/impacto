#pragma once

#include <array>

#include "../../ui/sysmesbox.h"

namespace Impacto {
namespace UI {
namespace SG0 {

class SysMesBox : public UI::SysMesBox {
 public:
  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void UpdateInput(float dt) override;
  void Render() override;

  void Init() override;
  void AddMessage(Vm::BufferOffsetContext ctx) override;
  void AddChoice(Vm::BufferOffsetContext ctx) override;

 private:
  void SetCursor(int choice);
  void Choose(int choice);

  int Cursor = 0;
  std::array<RectF, 8> ChoiceBounds;
};

}  // namespace SG0
}  // namespace UI
}  // namespace Impacto
