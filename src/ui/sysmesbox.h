#pragma once

#include "menu.h"
#include "../vm/vm.h"
#include "../text.h"
#include "../ui/widgets/group.h"

namespace Impacto {
namespace UI {

class SysMesBox : public Menu {
 public:
  virtual void Show();
  virtual void Hide();
  virtual void Update(float dt);
  virtual void Render();

  virtual void Init();
  virtual void AddMessage(Vm::BufferOffsetContext str);
  virtual void AddChoice(Vm::BufferOffsetContext str);

  int MessageCount;
  int ChoiceCount;

 protected:
  Widgets::Group* MessageItems;
  Widgets::Group* ChoiceItems;

  float BoxOpacity;
  std::array<std::vector<ProcessedTextGlyph>, 8> Messages;
  std::array<float, 8> MessageWidths;
  std::array<std::vector<ProcessedTextGlyph>, 8> Choices;
  std::array<float, 8> ChoiceWidths;
  float ChoiceX = 0.0f;

  Animation FadeAnimation;
};

}  // namespace UI
}  // namespace Impacto