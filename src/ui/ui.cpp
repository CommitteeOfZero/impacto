#include "ui.h"

namespace Impacto {
namespace UI {

Menu* FocusedMenu = 0;

Menu* SystemMenuPtr = new NullMenu();
Menu* TitleMenuPtr = new NullMenu();
Menu* SaveMenuPtr = new NullMenu();
Menu* OptionsMenuPtr = new NullMenu();
Menu* TipsMenuPtr = new NullMenu();

SelectionMenu* SelectionMenuPtr = 0;
SysMesBox* SysMesBoxPtr = 0;
BacklogMenu* BacklogMenuPtr = 0;

}  // namespace UI
}  // namespace Impacto