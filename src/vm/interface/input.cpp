#include "input.h"

#include "../../impacto.h"
#include "../../mem.h"
#include "../../inputsystem.h"
#include "../../profile/configsystem.h"
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

uint32_t GetPadInputButtonDown(InputDownType downType) {
  switch (downType) {
    case InputDownType::IsDown:
      return PADinputButtonIsDown;
    case InputDownType::WentDown:
      return PADinputButtonWentDown;
    case InputDownType::RepeatDown:
      return PADinputButtonRepeatDown;
    case InputDownType::RepeatAccelDown:
      return PADinputButtonRepeatAccelDown;

    default: {
      ImpLog(LogLevel::Error, LogChannel::General,
             "Unexpected InputDownType {:d}", static_cast<int>(downType));
      return 0;
    }
  }
}

void UpdatePADcustomType(int type) {
  if (type == 0) {
    PADcustom = PADcustomA;
  } else {
    PADcustom = PADcustomB;
  }
}

static std::array<float, 32> PADIsDownTime{};
static float PADRepeatClock = 0.0f;
static float PADAccel1Clock = 0.0f;
static float PADAccel2Clock = 0.0f;

enum class PADInputType { WentDown, IsDown };

static void UpdateFromPADCode(uint32_t PADcode, PADInputType type) {
  const auto KBcodes = PADToKeyboard.find(PADcode);
  const auto GPcode = PADToController.find(PADcode);
  const auto GPAcode = PADToControllerAxis.find(PADcode);
  const auto MScode = PADToMouse.find(PADcode);

  const auto& axisDownLightArr = type == PADInputType::WentDown
                                     ? Input::ControllerAxisWentDownLight
                                     : Input::ControllerAxisIsDownLight;
  [[maybe_unused]] const auto& axisDownHeavyArr =
      type == PADInputType::WentDown ? Input::ControllerAxisWentDownHeavy
                                     : Input::ControllerAxisIsDownHeavy;
  const auto& mouseDownArr = type == PADInputType::WentDown
                                 ? Input::MouseButtonWentDown
                                 : Input::MouseButtonIsDown;
  const auto& kbDownArr = type == PADInputType::WentDown
                              ? Input::KeyboardButtonWentDown
                              : Input::KeyboardButtonIsDown;
  const auto& gpDownArr = type == PADInputType::WentDown
                              ? Input::ControllerButtonWentDown
                              : Input::ControllerButtonIsDown;

  auto& padInputButton = type == PADInputType::WentDown ? PADinputButtonWentDown
                                                        : PADinputButtonIsDown;
  auto& padInputMouse = type == PADInputType::WentDown ? PADinputMouseWentDown
                                                       : PADinputMouseIsDown;

  const bool isKbDown =
      KBcodes != PADToKeyboard.end() &&
      std::any_of(KBcodes->second.begin(), KBcodes->second.end(),
                  [&kbDownArr](auto KBcode) { return kbDownArr[KBcode]; });
  const bool isGpDown =
      GPcode != PADToController.end() && gpDownArr[GPcode->second];
  const auto checkAxis = [&](auto axisDownArr) {
    if (GPAcode != PADToControllerAxis.end()) {
      if (axisDownArr[GPAcode->second.first] &&
          Input::ControllerAxis[GPAcode->second.first] *
                  (float)GPAcode->second.second >
              0.0f)
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
          (float)GPAcode->second.second * Input::ControllerAxisLightThreshold)
    padInputButton |= PADcode;
  if (isKbDown || isGpDown || isGPAxisDown) {
    padInputButton |= checkPadDirectional(PADcode);
  }
  if (isMsDown) padInputMouse |= PADcode;
}

void UpdatePADHoldInput(float dt) {
  constexpr float frameTime = 1 / 60.0f;

  PADinputButtonRepeatDown = 0;
  PADinputButtonRepeatAccelDown = 0;

  PADRepeatClock += dt;
  PADAccel1Clock += dt;
  PADAccel2Clock += dt;

  bool repeatCycle = false;
  bool accel1Cycle = false;
  bool accel2Cycle = false;

  // Reset clocks at intervals so all buttons sync
  if (PADRepeatClock >= 4 * frameTime) {
    repeatCycle = true;
    PADRepeatClock = 0.0f;
  }
  if (PADAccel1Clock >= 2 * frameTime) {
    accel1Cycle = true;
    PADAccel1Clock = 0.0f;
  }
  if (PADAccel2Clock >= 1 * frameTime) {
    accel2Cycle = true;
    PADAccel2Clock = 0.0f;
  }

  for (int i = 0; i < 32; i++) {
    uint32_t PADcode = 1 << (uint8_t)i;
    if ((PADinputButtonIsDown & PADcode) == 0) {
      PADIsDownTime[i] = 0.0f;
      continue;
    }

    if (PADinputButtonWentDown & PADcode) {
      PADinputButtonRepeatDown |= PADcode;
      PADinputButtonRepeatAccelDown |= PADcode;
    }

    PADIsDownTime[i] += dt;
    if (PADIsDownTime[i] >= 90 * frameTime) {
      if (PADIsDownTime[i] < 180) {
        if (accel1Cycle) PADinputButtonRepeatAccelDown |= PADcode;

      } else {
        if (accel2Cycle) PADinputButtonRepeatAccelDown |= PADcode;
      }
    }

    if (PADIsDownTime[i] >= 30 * frameTime) {
      if (repeatCycle) {
        PADinputButtonRepeatDown |= PADcode;
        if (PADIsDownTime[i] < 90 * frameTime) {
          PADinputButtonRepeatAccelDown |= PADcode;
        }
      }
    }
  }
}

void ResetPADHoldTimer(uint32_t PADcode) {
  for (uint8_t i = 0; i < 32; ++i) {
    if (PADcode & (1 << i)) PADIsDownTime[i] = 0.0f;
  }
}

void UpdatePADInput() {
  PADinputButtonWentDown = 0;
  PADinputMouseWentDown = 0;
  PADinputButtonIsDown = 0;
  PADinputMouseIsDown = 0;
  for (int i = 0; i < 32; i++) {
    uint32_t PADcode = 1 << (uint8_t)i;
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
bool GetControlState(int controlId, InputDownType downType) {
  using namespace Impacto::Profile::ConfigSystem;

  uint32_t padInputDown = GetPadInputButtonDown(downType);
  if (downType == InputDownType::IsDown) {
    padInputDown |= Interface::PADinputMouseIsDown;
  } else if (downType == InputDownType::WentDown) {
    padInputDown |= Interface::PADinputMouseWentDown;
  }

  switch (controlId) {
    case CT_OK:
      return padInputDown & PADcustom[5];
    case CT_Back:
      return padInputDown & PADcustom[6];
    case CT_HIDE:
      return PADcustom[11] ? (PADcustom[11] & PADinputButtonWentDown)
                           : (PADcustom[6] & PADinputButtonWentDown);
    case CT_NextMessage:
      return padInputDown & PADcustom[23];
    case CT_QuickSave:
      return padInputDown & PADcustom[13];
    case CT_MainMenu:
      return padInputDown & PADcustom[10];
    case CT_Backlog:
      return padInputDown & PADcustom[12];
    case CT_Tips:
      return false;
    case CT_LogoSkip:
      return padInputDown & PADcustom[14];
    case CT_ResetOptions:
      return padInputDown & PAD1Y;
    case CT_DelusionTriggerL:
      return padInputDown & PADcustom[36 + 2 * DirectionalInputForTrigger];
    case CT_DelusionTriggerR:
      return padInputDown & PADcustom[37 + 2 * DirectionalInputForTrigger];
    default:
      return false;
  }
}

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto
