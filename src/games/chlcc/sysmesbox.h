#pragma once

#include "../../hud/sysmesbox.h"

namespace Impacto {
namespace CHLCC {

using namespace Impacto::SysMesBox;

class SysMesBox : public SysMesBoxBase {
 public:
  void Show();
  void Hide();
  void Update(float dt);
  void Render();

 private:
  float ChoiceX = 0.0f;
  float HighlightX = 0.0f;
  float SelectedWidth = 0.0f;
  float SelectedWidthAdjusted = 0.0f;
};

}  // namespace CHLCC
}  // namespace Impacto