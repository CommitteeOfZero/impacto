#pragma once

#include "menu.h"
#include "widgets/button.h"
#include "../text.h"

namespace Impacto {
namespace UI {

class SelectionMenu : public Menu {
 public:
  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  void Init(bool isPlain);
  void AddChoice(uint8_t* str);

  void ChoiceItemOnClick(Widgets::Button* target);

  int SelectedChoiceId = 0;

 private:
  WidgetGroup* ChoiceItems;
  Animation FadeAnimation;
  bool IsPlain;
  ProcessedTextGlyph Choices[15][255];
  int ChoiceCount = 0;
  int ChoiceLengths[15];
  float ChoiceWidths[15];

  float CurrentSelBackgroundY = 0.0f;
  float ChoiceHeight = 0.0f;
  float ChoiceWidthMax = FLT_MIN;
  float ChoiceXMin = FLT_MAX;
};

}  // namespace UI
}  // namespace Impacto