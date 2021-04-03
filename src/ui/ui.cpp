#include "ui.h"

namespace Impacto {
namespace UI {

Menu* FocusedMenu = 0;

Menu* SystemMenuPtr = new NullMenu();
Menu* TitleMenuPtr = new NullMenu();
Menu* SaveMenuPtr = new NullMenu();

SelectionMenu* SelectionMenuPtr = 0;
SysMesBox* SysMesBoxPtr = 0;

}  // namespace UI
}  // namespace Impacto