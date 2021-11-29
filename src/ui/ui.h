#pragma once

#include "backlogmenu.h"
#include "selectionmenu.h"
#include "nullmenu.h"
#include "sysmesbox.h"
#include "tipsmenu.h"

namespace Impacto {
namespace UI {

BETTER_ENUM(SystemMenuType, int, None, RNE, MO6TW)
BETTER_ENUM(SaveMenuType, int, None, MO6TW)
BETTER_ENUM(SysMesBoxType, int, None, RNE, Dash, CHLCC, MO6TW, Darling, CC)
BETTER_ENUM(TitleMenuType, int, None, RNE, Dash, CHLCC, MO6TW, CC)
BETTER_ENUM(OptionsMenuType, int, None, MO6TW)
BETTER_ENUM(TipsMenuType, int, None, MO6TW)
BETTER_ENUM(ClearListMenuType, int, None, MO6TW)
BETTER_ENUM(AlbumMenuType, int, None, MO6TW)
BETTER_ENUM(MusicMenuType, int, None, MO6TW)
BETTER_ENUM(MovieMenuType, int, None, MO6TW)

int const MaxExtraMenus = 10;

// Current focused menu
extern Menu* FocusedMenu;

// Common menus
extern Menu* SystemMenuPtr;
extern Menu* TitleMenuPtr;
extern Menu* SaveMenuPtr;
extern Menu* OptionsMenuPtr;

// Extra menus
extern int ExtraMenuCount;
extern Menu* ExtraMenus[MaxExtraMenus];

//
extern SelectionMenu* SelectionMenuPtr;
extern SysMesBox* SysMesBoxPtr;
extern BacklogMenu* BacklogMenuPtr;
extern TipsMenu* TipsMenuPtr;

}  // namespace UI
}  // namespace Impacto