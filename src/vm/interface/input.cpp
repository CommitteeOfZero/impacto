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
  for (int i = 0; i < 30; i++) {
    int PADcode = (int)std::pow(2, i);
    auto KBcode = PADToKeyboard.find(PADcode);
    auto GPcode = PADToController.find(PADcode);
    auto GPAcode = PADToControllerAxis.find(PADcode);
    auto MScode = PADToMouse.find(PADcode);
    if (GPAcode != PADToControllerAxis.end() &&
        Input::ControllerAxisWentDownLight[GPAcode->second.first] &&
        Input::ControllerAxis[GPAcode->second.first] >
            GPAcode->second.second * Input::ControllerAxisLightThreshold)
      PADinputButtonWentDown |= PADcode;
    auto checkAxis = [&](bool axisDownArr[]) {
      if (GPAcode != PADToControllerAxis.end()) {
        if (axisDownArr[GPAcode->second.first] &&
            Input::ControllerAxis[GPAcode->second.first] *
                    GPAcode->second.second >
                0)
          return true;
      }
      return false;
    };
    bool isKbWentDown = KBcode != PADToKeyboard.end() &&
                        Input::KeyboardButtonWentDown[KBcode->second];
    bool isGpWentDown = GPcode != PADToController.end() &&
                        Input::ControllerButtonWentDown[GPcode->second];

    bool isGPAxisWentDown = checkAxis(Input::ControllerAxisWentDownLight);
    bool isMsWentDown = MScode != PADToMouse.end() &&
                        Input::MouseButtonWentDown[MScode->second];
    if (isKbWentDown || isGpWentDown || isGPAxisWentDown) {
      if (PADcode == PAD1UP_DIRECT || PADcode == PAD1UP_RS ||
          PADcode == PAD1UP_LS)
        PADcode |= PAD1UP;
      else if (PADcode == PAD1DOWN_DIRECT || PADcode == PAD1DOWN_RS ||
               PADcode == PAD1DOWN_LS)
        PADcode |= PAD1DOWN;
      else if (PADcode == PAD1LEFT_DIRECT || PADcode == PAD1LEFT_RS ||
               PADcode == PAD1LEFT_LS)
        PADcode |= PAD1LEFT;
      else if (PADcode == PAD1RIGHT_DIRECT || PADcode == PAD1RIGHT_RS ||
               PADcode == PAD1RIGHT_LS)
        PADcode |= PAD1RIGHT;
      PADinputButtonWentDown |= PADcode;
    }
    if (isMsWentDown) PADinputMouseWentDown |= PADcode;

    bool isKbIsDown = KBcode != PADToKeyboard.end() &&
                      Input::KeyboardButtonIsDown[KBcode->second];
    bool isGpIsDown = GPcode != PADToController.end() &&
                      Input::ControllerButtonIsDown[GPcode->second];
    bool isGPAxisIsDown = checkAxis(Input::ControllerAxisIsDownLight);
    bool isMsIsDown =
        MScode != PADToMouse.end() && Input::MouseButtonIsDown[MScode->second];
    if (isKbIsDown || isGpIsDown || isGPAxisIsDown) {
      if (PADcode == PAD1UP_DIRECT || PADcode == PAD1UP_RS ||
          PADcode == PAD1UP_LS)
        PADcode |= PAD1UP;
      else if (PADcode == PAD1DOWN_DIRECT || PADcode == PAD1DOWN_RS ||
               PADcode == PAD1DOWN_LS)
        PADcode |= PAD1DOWN;
      else if (PADcode == PAD1LEFT_DIRECT || PADcode == PAD1LEFT_RS ||
               PADcode == PAD1LEFT_LS)
        PADcode |= PAD1LEFT;
      else if (PADcode == PAD1RIGHT_DIRECT || PADcode == PAD1RIGHT_RS ||
               PADcode == PAD1RIGHT_LS)
        PADcode |= PAD1RIGHT;
      PADinputButtonIsDown |= PADcode;
    }
    if (isMsIsDown) PADinputMouseIsDown |= PADcode;
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
    case CT_HIDE: {
      return PADcustom[11] ? (PADcustom[11] & PADinputButtonWentDown)
                           : (PADcustom[6] & PADinputButtonWentDown);
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
    case CT_DelusionTriggerL: {
      return (PADinputButtonWentDown & PADcustom[36]) ||
             (PADinputMouseWentDown & PADcustom[36]);
    }
    case CT_DelusionTriggerR: {
      return (PADinputButtonWentDown & PADcustom[37]) ||
             (PADinputMouseWentDown & PADcustom[37]);
    }
    default:
      return false;
  }
}

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto
