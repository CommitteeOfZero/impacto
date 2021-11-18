#pragma once

#include "backlogmenu.h"
#include "selectionmenu.h"
#include "nullmenu.h"
#include "sysmesbox.h"

namespace Impacto {
namespace UI {

BETTER_ENUM(SystemMenuType, int, None, RNE, MO6TW)
BETTER_ENUM(SaveMenuType, int, None, MO6TW)
BETTER_ENUM(SysMesBoxType, int, None, RNE, Dash, CHLCC, MO6TW, Darling, CC)
BETTER_ENUM(TitleMenuType, int, None, RNE, Dash, CHLCC, MO6TW, CC)
BETTER_ENUM(OptionsMenuType, int, None, MO6TW)

// Current focused menu
extern Menu* FocusedMenu;

// Common menus
extern Menu* SystemMenuPtr;
extern Menu* TitleMenuPtr;
extern Menu* SaveMenuPtr;
extern Menu* OptionsMenuPtr;

//
extern SelectionMenu* SelectionMenuPtr;
extern SysMesBox* SysMesBoxPtr;
extern BacklogMenu* BacklogMenuPtr;

}  // namespace UI
}  // namespace Impacto