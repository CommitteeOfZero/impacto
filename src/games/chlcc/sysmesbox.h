#pragma once

#include "../../ui/sysmesbox.h"
#include "../../ui/widgets/button.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

struct SysMesBoxStar {
  float Angle;
  float RotationSpeed;
};

class SysMesBox : public UI::SysMesBox {
 public:
  SysMesBox();

  virtual void Show() override;
  virtual void Hide() override;
  virtual void Update(float dt) override;
  virtual void Render() override;

  virtual void Init() override;
  virtual void AddMessage(uint8_t* str) override;
  virtual void AddChoice(uint8_t* str) override;

 private:
  std::array<SysMesBoxStar, 14> LoadingStars;
  Animation LoadingStarsFadeAnimation;

  void ChoiceItemOnClick(UI::Widgets::Button* target);
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto