#pragma once

#include "../nametagdisplay.h"

namespace Impacto {
namespace CHLCC {

class NametagDisplay : public Impacto::NametagDisplay {
 public:
  void Render(const NameInfo& nameInfo, glm::vec4 tint) override;
  void Update(float dt) override {}
  void Reset() override {}
};

}  // namespace CHLCC
}  // namespace Impacto
