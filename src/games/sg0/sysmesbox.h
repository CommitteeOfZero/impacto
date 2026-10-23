#pragma once

#include <array>

#include "../../ui/sysmesbox.h"

namespace Impacto {
namespace UI {
namespace SG0 {

// STEINS;GATE 0 system message: a full-screen backdrop (gsysmes.dds) with the
// message lines centred on it and the script's choice strings in one row
// underneath. SystemMes(2, kind) chooses 0 = OK (no choices, any confirm
// closes), 1 = two choices, 255 = no input (progress messages); the chosen
// index goes to SW_SYSSEL.
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
