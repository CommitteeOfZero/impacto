#pragma once

#include "menu.h"

namespace Impacto {
namespace UI {

class NullMenu : public Menu {
 public:
  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void Render() override;
};

}  // namespace UI
}  // namespace Impacto