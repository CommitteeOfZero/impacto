#pragma once

#include "selectionmenu.h"
#include "sysmesbox.h"

namespace Impacto {
namespace UI {

BETTER_ENUM(SystemMenuType, int, None, RNE, MO6TW)
BETTER_ENUM(SysMesBoxType, int, None, RNE, Dash, CHLCC, MO6TW, Darling)
BETTER_ENUM(TitleMenuType, int, None, RNE, Dash, CHLCC, MO6TW)

// Current focused menu
extern Menu* FocusedMenu;

// Common menus
extern Menu* SystemMenuPtr;
extern Menu* TitleMenuPtr;

//
extern SelectionMenu* SelectionMenuPtr;
extern SysMesBox* SysMesBoxPtr;

}  // namespace UI
}  // namespace Impacto