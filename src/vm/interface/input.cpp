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

enum class PADInputType { WentDown, IsDown };

static void UpdateFromPADCode(int PADcode, PADInputType type) {
  const auto KBcodes = PADToKeyboard.find(PADcode);
  const auto GPcode = PADToController.find(PADcode);
  const auto GPAcode = PADToControllerAxis.find(PADcode);
  const auto MScode = PADToMouse.find(PADcode);

  const auto& axisDownLightArr = type == PADInputType::WentDown
                                     ? Input::ControllerAxisWentDownLight
                                     : Input::ControllerAxisIsDownLight;
  const auto& axisDownHeavyArr = type == PADInputType::WentDown
                                     ? Input::ControllerAxisWentDownHeavy
                                     : Input::ControllerAxisIsDownHeavy;
  const auto& mouseDownArr = type == PADInputType::WentDown
                                 ? Input::MouseButtonWentDown
                                 : Input::MouseButtonIsDown;
  const auto& kbDownArr = type == PADInputType::WentDown
                              ? Input::KeyboardButtonWentDown
                              : Input::KeyboardButtonIsDown;

  auto& padInputButton = type == PADInputType::WentDown ? PADinputButtonWentDown
                                                        : PADinputButtonIsDown;
  auto& padInputMouse = type == PADInputType::WentDown ? PADinputMouseWentDown
                                                       : PADinputMouseIsDown;

  const bool isKbDown =
      KBcodes != PADToKeyboard.end() &&
      std::any_of(KBcodes->second.begin(), KBcodes->second.end(),
                  [&kbDownArr](auto KBcode) { return kbDownArr[KBcode]; });
  const bool isGpDown = GPcode != PADToController.end() &&
                        Input::ControllerButtonWentDown[GPcode->second];
  const auto checkAxis = [&](auto axisDownArr) {
    if (GPAcode != PADToControllerAxis.end()) {
      if (axisDownArr[GPAcode->second.first] &&
          Input::ControllerAxis[GPAcode->second.first] *
                  GPAcode->second.second >
              0)
        return true;
    }
    return false;
  };

  const auto checkPadDirectional = [](auto PADcode) {
    if (PADcode == PAD1UP_DIRECT || PADcode == PAD1UP_RS ||
        PADcode == PAD1UP_LS)
      PADcode |= PAD1UP;
    if (PADcode == PAD1DOWN_DIRECT || PADcode == PAD1DOWN_RS ||
        PADcode == PAD1DOWN_LS)
      PADcode |= PAD1DOWN;
    if (PADcode == PAD1LEFT_DIRECT || PADcode == PAD1LEFT_RS ||
        PADcode == PAD1LEFT_LS)
      PADcode |= PAD1LEFT;
    if (PADcode == PAD1RIGHT_DIRECT || PADcode == PAD1RIGHT_RS ||
        PADcode == PAD1RIGHT_LS)
      PADcode |= PAD1RIGHT;
    return PADcode;
  };

  const bool isGPAxisDown = checkAxis(axisDownLightArr);
  const bool isMsDown =
      MScode != PADToMouse.end() && mouseDownArr[MScode->second];
  if (isGPAxisDown &&
      Input::ControllerAxis[GPAcode->second.first] >
          GPAcode->second.second * Input::ControllerAxisLightThreshold)
    padInputButton |= PADcode;
  if (isKbDown || isGpDown || isGPAxisDown) {
    padInputButton |= checkPadDirectional(PADcode);
  }
  if (isMsDown) padInputMouse |= PADcode;
}

void UpdatePADInput() {
  PADinputButtonWentDown = 0;
  PADinputMouseWentDown = 0;
  PADinputButtonIsDown = 0;
  PADinputMouseIsDown = 0;
  for (int i = 0; i < 30; i++) {
    int PADcode = (int)std::pow(2, i);
    UpdateFromPADCode(PADcode, PADInputType::WentDown);
    UpdateFromPADCode(PADcode, PADInputType::IsDown);
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
