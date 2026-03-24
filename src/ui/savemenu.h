#pragma once

#include <optional>
#include "menu.h"
#include "../text/text.h"
#include "../data/savesystem.h"

namespace Impacto {
namespace UI {
enum class SaveMenuPageType : int {
  QuickLoad,
  Save,
  Load,
};
class SaveMenu : public Menu {
 public:
  SaveMenu() { InputConfig = InputRate::RepeatFast; }
  virtual void Update(float dt) override {};
  virtual void Render() override {};

  virtual void RefreshCurrentEntryInfo() {};

  uint8_t ActivePage = 0;
  std::optional<SaveMenuPageType> ActiveMenuType;
};

}  // namespace UI
}  // namespace Impacto