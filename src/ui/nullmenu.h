#pragma once

#include "menu.h"

namespace Impacto {
namespace UI {

class NullMenu : public Menu {
 public:
  void Show();
  void Hide();
  void Update(float dt);
  void Render();
};

}  // namespace UI
}  // namespace Impacto