#include "input.h"

#include "../../impacto.h"
#include "../../mem.h"
#include "../../inputsystem.h"
#include "../../animation.h"
#include "../../profile/vm.h"
#include "../../profile/scriptinput.h"
#include "../../profile/scriptvars.h"

namespace Impacto {
namespace Vm {
namespace Interface {

using namespace Impacto::Profile::ScriptInput;
using namespace Impacto::Profile::ScriptVars;

int PADinputButtonWentDown = 0;
int PADinputMouseWentDown = 0;
int PADinputButtonIsDown = 0;
int PADinputMouseIsDown = 0;

void UpdatePADInput() {
  PADinputButtonWentDown = 0;
  PADinputMouseWentDown = 0;
  PADinputButtonIsDown = 0;
  PADinputMouseIsDown = 0;
  for (int i = 0; i < 16; i++) {
    int PADcode = std::pow(2, i);
    int KBcode = PADToKeyboard[PADcode];
    int GPcode = PADToController[PADcode];
    int MScode = PADToMouse[PADcode];
    if ((GPcode == PAD1L2 || GPcode == PAD1R2) && 
        (Input::ControllerAxisWentDownLight[GPcode] ||
         Input::KeyboardButtonIsDown[KBcode]))
      PADinputButtonWentDown |= PADcode;
    if (Input::KeyboardButtonWentDown[KBcode] ||
        Input::ControllerButtonWentDown[GPcode])
      PADinputButtonWentDown |= PADcode;
    if (Input::MouseButtonWentDown[MScode]) PADinputMouseWentDown |= PADcode;
    if (Input::KeyboardButtonIsDown[KBcode] ||
        Input::ControllerButtonIsDown[GPcode])
      PADinputButtonIsDown |= PADcode;
    if (Input::MouseButtonIsDown[MScode]) PADinputMouseIsDown |= PADcode;
  }

  if (PADinputButtonIsDown & PAD1R1) {
    SetFlag(SF_MESALLSKIP, true);
  } else {
    SetFlag(SF_MESALLSKIP, false);
  }
}

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto