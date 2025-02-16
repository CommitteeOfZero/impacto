#include "scriptinput.h"
#include "profile_internal.h"

#include "../vm/interface/input.h"

namespace Impacto {
namespace Profile {
namespace ScriptInput {

using namespace Impacto::Vm::Interface;

void Configure() {
  EnsurePushMemberOfType("Input", LUA_TTABLE);

  KB_PAD1A = EnsureGetMemberUint("KB_PAD1A");
  KB_PAD1B = EnsureGetMemberUint("KB_PAD1B");
  KB_PAD1X = EnsureGetMemberUint("KB_PAD1X");
  KB_PAD1Y = EnsureGetMemberUint("KB_PAD1Y");
  KB_PAD1SELECT = EnsureGetMemberUint("KB_PAD1SELECT");
  KB_PAD1START = EnsureGetMemberUint("KB_PAD1START");
  KB_PAD1UP = EnsureGetMemberUint("KB_PAD1UP");
  KB_PAD1DOWN = EnsureGetMemberUint("KB_PAD1DOWN");
  KB_PAD1LEFT = EnsureGetMemberUint("KB_PAD1LEFT");
  KB_PAD1RIGHT = EnsureGetMemberUint("KB_PAD1RIGHT");
  KB_PAD1L1 = EnsureGetMemberUint("KB_PAD1L1");
  KB_PAD1L2 = EnsureGetMemberUint("KB_PAD1L2");
  KB_PAD1L3 = EnsureGetMemberUint("KB_PAD1L3");
  KB_PAD1R1 = EnsureGetMemberUint("KB_PAD1R1");
  KB_PAD1R2 = EnsureGetMemberUint("KB_PAD1R2");
  KB_PAD1R3 = EnsureGetMemberUint("KB_PAD1R3");
  KB_PAD1L1_ALT = EnsureGetMemberUint("KB_PAD1L1_ALT");
  KB_PAD1UP_RS = EnsureGetMemberUint("KB_PAD1UP_RS");
  KB_PAD1DOWN_RS = EnsureGetMemberUint("KB_PAD1DOWN_RS");

  MS_LEFT = EnsureGetMemberUint("MS_LEFT");
  MS_RIGHT = EnsureGetMemberUint("MS_RIGHT");

  GP_PAD1A = EnsureGetMemberUint("GP_PAD1A");
  GP_PAD1B = EnsureGetMemberUint("GP_PAD1B");
  GP_PAD1X = EnsureGetMemberUint("GP_PAD1X");
  GP_PAD1Y = EnsureGetMemberUint("GP_PAD1Y");
  GP_PAD1SELECT = EnsureGetMemberUint("GP_PAD1SELECT");
  GP_PAD1START = EnsureGetMemberUint("GP_PAD1START");
  GP_PAD1UP = EnsureGetMemberUint("GP_PAD1UP");
  GP_PAD1DOWN = EnsureGetMemberUint("GP_PAD1DOWN");
  GP_PAD1LEFT = EnsureGetMemberUint("GP_PAD1LEFT");
  GP_PAD1RIGHT = EnsureGetMemberUint("GP_PAD1RIGHT");
  GP_PAD1L1 = EnsureGetMemberUint("GP_PAD1L1");
  GP_PAD1L3 = EnsureGetMemberUint("GP_PAD1L3");
  GP_PAD1R1 = EnsureGetMemberUint("GP_PAD1R1");
  GP_PAD1R3 = EnsureGetMemberUint("GP_PAD1R3");

  GPA_PAD1LX = EnsureGetMemberUint("GPA_PAD1LX");
  GPA_PAD1LY = EnsureGetMemberUint("GPA_PAD1LY");
  GPA_PAD1RX = EnsureGetMemberUint("GPA_PAD1RX");
  GPA_PAD1RY = EnsureGetMemberUint("GPA_PAD1RY");
  GPA_PAD1LT = EnsureGetMemberUint("GPA_PAD1LT");
  GPA_PAD1RT = EnsureGetMemberUint("GPA_PAD1RT");

  PADToKeyboard[PAD1A] = KB_PAD1A;
  PADToKeyboard[PAD1B] = KB_PAD1B;
  PADToKeyboard[PAD1X] = KB_PAD1X;
  PADToKeyboard[PAD1Y] = KB_PAD1Y;
  PADToKeyboard[PAD1SELECT] = KB_PAD1SELECT;
  PADToKeyboard[PAD1START] = KB_PAD1START;
  PADToKeyboard[PAD1UP_DIRECT] = KB_PAD1UP;
  PADToKeyboard[PAD1DOWN_DIRECT] = KB_PAD1DOWN;
  PADToKeyboard[PAD1LEFT_DIRECT] = KB_PAD1LEFT;
  PADToKeyboard[PAD1RIGHT_DIRECT] = KB_PAD1RIGHT;
  PADToKeyboard[PAD1L1] = KB_PAD1L1;
  PADToKeyboard[PAD1L2] = KB_PAD1L2;
  PADToKeyboard[PAD1L3] = KB_PAD1L3;
  PADToKeyboard[PAD1R1] = KB_PAD1R1;
  PADToKeyboard[PAD1R2] = KB_PAD1R2;
  PADToKeyboard[PAD1R3] = KB_PAD1R3;

  PADToKeyboard[PAD1UP_RS] = KB_PAD1UP_RS;
  PADToKeyboard[PAD1DOWN_RS] = KB_PAD1DOWN_RS;

  PADToMouse[PAD1A] = MS_LEFT;
  PADToMouse[PAD1B] = MS_RIGHT;
  PADToMouse[PAD1START] = MS_RIGHT;

  PADToController[PAD1A] = GP_PAD1A;
  PADToController[PAD1B] = GP_PAD1B;
  PADToController[PAD1X] = GP_PAD1X;
  PADToController[PAD1Y] = GP_PAD1Y;
  PADToController[PAD1SELECT] = GP_PAD1SELECT;
  PADToController[PAD1START] = GP_PAD1START;
  PADToController[PAD1L1] = GP_PAD1L1;
  PADToController[PAD1L3] = GP_PAD1L3;
  PADToController[PAD1R1] = GP_PAD1R1;
  PADToController[PAD1R3] = GP_PAD1R3;
  PADToController[PAD1UP_DIRECT] = GP_PAD1UP;
  PADToController[PAD1DOWN_DIRECT] = GP_PAD1DOWN;
  PADToController[PAD1LEFT_DIRECT] = GP_PAD1LEFT;
  PADToController[PAD1RIGHT_DIRECT] = GP_PAD1RIGHT;

  PADToControllerAxis[PAD1UP_LS] = {GPA_PAD1LY, AxisNeg};
  PADToControllerAxis[PAD1DOWN_LS] = {GPA_PAD1LY, AxisPos};
  PADToControllerAxis[PAD1LEFT_LS] = {GPA_PAD1LX, AxisNeg};
  PADToControllerAxis[PAD1RIGHT_LS] = {GPA_PAD1LX, AxisPos};
  PADToControllerAxis[PAD1UP_RS] = {GPA_PAD1RY, AxisNeg};
  PADToControllerAxis[PAD1DOWN_RS] = {GPA_PAD1RY, AxisPos};
  PADToControllerAxis[PAD1LEFT_RS] = {GPA_PAD1RX, AxisNeg};
  PADToControllerAxis[PAD1RIGHT_RS] = {GPA_PAD1RX, AxisPos};
  PADToControllerAxis[PAD1L2] = {GPA_PAD1LT, AxisPos};
  PADToControllerAxis[PAD1R2] = {GPA_PAD1RT, AxisPos};

  Pop();

  PADcustomType = EnsureGetMemberUint("PADcustomType");
  PADcustomSizeA = EnsureGetMemberUint("PADcustomSizeA");
  PADcustomSizeB = EnsureGetMemberUint("PADcustomSizeB");
  GetMemberUintArray(PADcustomA, PADcustomSizeA, "PADcustomA");
  GetMemberUintArray(PADcustomB, PADcustomSizeB, "PADcustomB");
}

}  // namespace ScriptInput
}  // namespace Profile
}  // namespace Impacto