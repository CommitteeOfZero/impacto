#pragma once

#include <enum.h>
#include <flat_hash_map.hpp>
#include "../impacto.h"
#include "widgetgroup.h"

namespace Impacto {
namespace UI {

BETTER_ENUM(MenuType, int, None, RNE, Dash, CHLCC, MO6TW)

enum MenuState { Hidden, Hiding, Showing, Shown };

class Menu {
 public:
  virtual void Show() = 0;
  virtual void Hide() = 0;
  virtual void Update(float dt) = 0;
  virtual void Render() = 0;

  MenuState State;

  bool IsFocused = false;
  bool ChoiceMade = false;

  Menu* LastFocusedMenu = 0;
};

}  // namespace UI
}  // namespace Impacto