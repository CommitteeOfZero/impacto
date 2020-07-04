#include "input.h"

#include "../../impacto.h"
#include "../../inputsystem.h"
#include "../../profile/vm.h"
#include "../../profile/scriptinput.h"

namespace Impacto {
namespace Vm {
namespace Interface {

using namespace Impacto::Profile::ScriptInput;

int PADinputButtonWentDown = 0;
int PADinputMouseWentDown = 0;

void UpdatePADInput() {
  PADinputButtonWentDown = 0;
  PADinputMouseWentDown = 0;
  for (int i = 0; i < 16; i++) {
    int PADcode = std::pow(2, i);
    int KBcode = PADToKeyboard[PADcode];
    int GPcode = PADToController[PADcode];
    int MScode = PADToMouse[PADcode];
    if (Input::KeyboardButtonWentDown[KBcode] ||
        Input::ControllerButtonWentDown[GPcode])
      PADinputButtonWentDown |= PADcode;
    if (Input::MouseButtonWentDown[MScode]) PADinputMouseWentDown |= PADcode;
  }
}

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto