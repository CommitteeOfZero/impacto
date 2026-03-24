#pragma once

#include "backlogmenu.h"
#include "selectionmenu.h"
#include "nullmenu.h"
#include "sysmesbox.h"
#include "tipsmenu.h"
#include "optionsmenu.h"
#include "savemenu.h"
#include "../game.h"

#include <ankerl/unordered_dense.h>
#include <vector>

namespace Impacto {
namespace UI {

enum class CommonMenuType : int {
  None,
  CHLCC,
};
enum class SystemMenuType : int {
  None,
  RNE,
  MO6TW,
  CHLCC,
  MO8,
  CCLCC,
};
enum class SaveMenuType : int {
  None,
  MO6TW,
  MO8,
  CHLCC,
  CCLCC,
};
enum class SysMesBoxType : int {
  None,
  RNE,
  Dash,
  CHLCC,
  MO6TW,
  Darling,
  CC,
};
enum class TitleMenuType : int {
  None,
  RNE,
  Dash,
  CHLCC,
  MO6TW,
  MO8,
  CC,
  CCLCC,
};
enum class OptionsMenuType : int {
  None,
  MO6TW,
  MO8,
  CHLCC,
  CCLCC,
};
enum class TipsMenuType : int {
  None,
  MO6TW,
  CHLCC,
  CCLCC,
};
enum class ClearListMenuType : int {
  None,
  MO6TW,
  CHLCC,
  CCLCC,
};
enum class LibraryMenuType : int {
  None,
  CCLCC,
};
enum class AlbumMenuType : int {
  None,
  MO6TW,
  CHLCC,
};
enum class MusicMenuType : int {
  None,
  MO6TW,
  CHLCC,
};
enum class MovieMenuType : int {
  None,
  MO6TW,
  CHLCC,
};
enum class ActorsVoiceMenuType : int {
  None,
  MO6TW,
};
enum class TrophyMenuType : int {
  None,
  CHLCC,
};
enum class HelpMenuType : int {
  None,
  CCLCC,
};
enum class GameSpecificType : int {
  None,
  CHLCC,
  CC,
  CCLCC,
  RNE,
  Dash,
};
int constexpr MaxExtraMenus = 10;

inline ankerl::unordered_dense::map<Game::DrawComponentType, std::vector<Menu*>>
    Menus;

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