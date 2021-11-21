#pragma once

#include "menu.h"
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
  virtual void AddMessage(uint8_t* str);
  virtual void AddChoice(uint8_t* str);

  int MessageCount;
  int ChoiceCount;

 protected:
  Widgets::Group* MessageItems;
  Widgets::Group* ChoiceItems;

  float BoxOpacity;
  ProcessedTextGlyph Messages[8][255];
  int MessageLengths[8];
  float MessageWidths[8];
  ProcessedTextGlyph Choices[8][255];
  int ChoiceLengths[8];
  float ChoiceWidths[8];
  float ChoiceX = 0.0f;

  Animation FadeAnimation;
};

}  // namespace UI
}  // namespace Impacto