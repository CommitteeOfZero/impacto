#pragma once

#include "../nametagdisplay.h"

namespace Impacto {
namespace CC {

class NametagDisplay : public Impacto::NametagDisplay {
 public:
  NametagDisplay();

  void Render(const NameInfo& nameInfo, glm::vec4 tint) override;
  void Update(float dt) override;
  void Reset() override;

  void Show() override;
  void Hide() override;

 private:
  Animation ShowAnimation;
};

}  // namespace CC
}  // namespace Impacto
