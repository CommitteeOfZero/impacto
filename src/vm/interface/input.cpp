#include "input.h"

#include "../../impacto.h"
#include "../../mem.h"
#include "../../inputsystem.h"
#include "../../animation.h"
#include "../../profile/vm.h"
#include "../../profile/scriptinput.h"
#include "../../profile/scriptvars.h"
#include "../../profile/profile_internal.h"

namespace Impacto {
namespace Vm {
namespace Interface {

using namespace Impacto::Profile::ScriptInput;
using namespace Impacto::Profile::ScriptVars;

void UpdatePADcustomType(int type) {
  if (type == 0) {
    PADcustom = PADcustomA;
  } else {
    PADcustom = PADcustomB;
  }
}

void UpdatePADInput() {
  PADinputButtonWentDown = 0;
  PADinputMouseWentDown = 0;
  PADinputButtonIsDown = 0;
  PADinputMouseIsDown = 0;
  for (int i = 0; i < 20; i++) {
    int PADcode = (int)std::pow(2, i);
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
  if (Input::TouchWentDown[0] && Input::TouchWentDown[1])
    PADinputMouseWentDown |= PAD1B;
  else if (Input::TouchWentDown[0])
    PADinputMouseWentDown |= PAD1A;
  if (Input::TouchIsDown[0]) PADinputMouseIsDown |= PAD1A;
}

// TODO: Make this configurable per game
// I have no idea why they have a million things for controls...
bool GetControlState(int controlId) {
  switch (controlId) {
    case CT_OK: {
      return (PADinputButtonWentDown & PADcustom[5]) ||
             (PADinputMouseWentDown & PADcustom[5]);
    }
    case CT_Back: {
      return (PADinputButtonWentDown & PADcustom[6]) ||
             (PADinputMouseWentDown & PADcustom[6]);
    }
    case CT_NextMessage: {
      return (PADinputButtonWentDown & PADcustom[23]) ||
             (PADinputMouseWentDown & PADcustom[23]);
    }
    case CT_QuickSave: {
      return (PADinputButtonWentDown & PADcustom[13]) ||
             (PADinputMouseWentDown & PADcustom[13]);
    }
    case CT_MainMenu: {
      return (PADinputButtonWentDown & PADcustom[10]) ||
             (PADinputMouseWentDown & PADcustom[10]);
    }
    case CT_Backlog: {
      return (PADinputButtonWentDown & PADcustom[12]) ||
             (PADinputMouseWentDown & PADcustom[12]);
    }
    case CT_Tips: {
      return false;
    }
    case CT_LogoSkip: {
      return (PADinputButtonWentDown & PADcustom[14]) ||
             (PADinputMouseWentDown & PADcustom[14]);
    }
    case CT_ResetOptions: {
      return (PADinputButtonWentDown & PAD1Y) ||
             (PADinputMouseWentDown & PAD1Y);
    }
    default:
      return false;
  }
}

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto
