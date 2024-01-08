#include "ui.h"

namespace Impacto {
namespace UI {

Menu* FocusedMenu = 0;

ska::flat_hash_map<uint8_t, std::vector<Menu*>> Menus;

Menu* SystemMenuPtr = new NullMenu();
Menu* TitleMenuPtr = new NullMenu();
Menu* SaveMenuPtr = new NullMenu();
Menu* OptionsMenuPtr = new NullMenu();
Menu* TrophyMenuPtr = new NullMenu();

SelectionMenu* SelectionMenuPtr = 0;
SysMesBox* SysMesBoxPtr = 0;
BacklogMenu* BacklogMenuPtr = 0;
TipsMenu* TipsMenuPtr = 0;

}  // namespace UI
}  // namespace Impacto