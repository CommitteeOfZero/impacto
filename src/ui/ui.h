#pragma once

#include "backlogmenu.h"
#include "selectionmenu.h"
#include "nullmenu.h"
#include "sysmesbox.h"
#include "tipsmenu.h"
#include "optionsmenu.h"
#include "savemenu.h"

#include <ankerl/unordered_dense.h>
#include <vector>

namespace Impacto {
namespace UI {

BETTER_ENUM(SystemMenuType, int, None, RNE, MO6TW, CHLCC, MO8, CCLCC)
BETTER_ENUM(SaveMenuType, int, None, MO6TW, MO8, CHLCC, CCLCC)
BETTER_ENUM(SysMesBoxType, int, None, RNE, Dash, CHLCC, MO6TW, Darling, CC)
BETTER_ENUM(TitleMenuType, int, None, RNE, Dash, CHLCC, MO6TW, MO8, CC, CCLCC)
BETTER_ENUM(OptionsMenuType, int, None, MO6TW, MO8, CHLCC, CCLCC)
BETTER_ENUM(TipsMenuType, int, None, MO6TW, CHLCC, CCLCC)
BETTER_ENUM(ClearListMenuType, int, None, MO6TW, CHLCC, CCLCC)
BETTER_ENUM(LibraryMenuType, int, None, CCLCC)
BETTER_ENUM(AlbumMenuType, int, None, MO6TW, CHLCC)
BETTER_ENUM(MusicMenuType, int, None, MO6TW, CHLCC)
BETTER_ENUM(MovieMenuType, int, None, MO6TW, CHLCC)
BETTER_ENUM(ActorsVoiceMenuType, int, None, MO6TW)
BETTER_ENUM(TrophyMenuType, int, None, CHLCC)
BETTER_ENUM(HelpMenuType, int, None, CCLCC)

int constexpr MaxExtraMenus = 10;

inline ankerl::unordered_dense::map<uint8_t, std::vector<Menu*>> Menus;

// Current focused menu
inline Menu* FocusedMenu = nullptr;

// Common menus
inline Menu* SystemMenuPtr = new NullMenu();
inline Menu* TitleMenuPtr = new NullMenu();
inline Menu* TrophyMenuPtr = new NullMenu();
inline Menu* HelpMenuPtr = new NullMenu();
inline SaveMenu* SaveMenuPtr = new SaveMenu();

//
inline SelectionMenu* SelectionMenuPtr = nullptr;
inline SysMesBox* SysMesBoxPtr = nullptr;
inline BacklogMenu* BacklogMenuPtr = nullptr;
inline TipsMenu* TipsMenuPtr = nullptr;
inline OptionsMenu* OptionsMenuPtr = nullptr;

// Extras menus
inline Menu* LibraryMenuPtr = nullptr;
inline Menu* ClearListMenuPtr = nullptr;
inline Menu* AlbumMenuPtr = nullptr;
inline Menu* MusicMenuPtr = nullptr;
inline Menu* MovieMenuPtr = nullptr;
inline Menu* ActorsVoiceMenuPtr = nullptr;

}  // namespace UI
}  // namespace Impacto