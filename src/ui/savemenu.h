#pragma once

#include <optional>
#include "menu.h"
#include "../text.h"
#include "../data/savesystem.h"

namespace Impacto {
namespace UI {
BETTER_ENUM(SaveMenuPageType, int, QuickLoad, Save, Load)

class SaveMenu : public Menu {
 public:
  virtual void Update(float dt) override {};
  virtual void Render() override {};

  uint8_t ActivePage = 0;
  better_enums::optional<SaveMenuPageType> ActiveMenuType;
};

}  // namespace UI
}  // namespace Impacto