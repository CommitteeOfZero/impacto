#include "scriptinput.h"
#include "profile_internal.h"

#include "../vm/interface/input.h"

namespace Impacto {
namespace Profile {
namespace ScriptInput {

using namespace Impacto::Vm::Interface;

void Configure() {
  EnsurePushMemberOfType("Input", LUA_TTABLE);

  auto KB_PAD1A = GetMemberVector<KeyboardCodeType>("KB_PAD1A");
  auto KB_PAD1B = GetMemberVector<KeyboardCodeType>("KB_PAD1B");
  auto KB_PAD1X = GetMemberVector<KeyboardCodeType>("KB_PAD1X");
  auto KB_PAD1Y = GetMemberVector<KeyboardCodeType>("KB_PAD1Y");
  auto KB_PAD1SELECT = GetMemberVector<KeyboardCodeType>("KB_PAD1SELECT");
  auto KB_PAD1START = GetMemberVector<KeyboardCodeType>("KB_PAD1START");
  auto KB_PAD1UP = GetMemberVector<KeyboardCodeType>("KB_PAD1UP");
  auto KB_PAD1DOWN = GetMemberVector<KeyboardCodeType>("KB_PAD1DOWN");
  auto KB_PAD1LEFT = GetMemberVector<KeyboardCodeType>("KB_PAD1LEFT");
  auto KB_PAD1RIGHT = GetMemberVector<KeyboardCodeType>("KB_PAD1RIGHT");
  auto KB_PAD1L1 = GetMemberVector<KeyboardCodeType>("KB_PAD1L1");
  auto KB_PAD1L2 = GetMemberVector<KeyboardCodeType>("KB_PAD1L2");
  auto KB_PAD1L3 = GetMemberVector<KeyboardCodeType>("KB_PAD1L3");
  auto KB_PAD1R1 = GetMemberVector<KeyboardCodeType>("KB_PAD1R1");
  auto KB_PAD1R2 = GetMemberVector<KeyboardCodeType>("KB_PAD1R2");
  auto KB_PAD1R3 = GetMemberVector<KeyboardCodeType>("KB_PAD1R3");
  auto KB_PAD1UP_RS = GetMemberVector<KeyboardCodeType>("KB_PAD1UP_RS");
  auto KB_PAD1DOWN_RS = GetMemberVector<KeyboardCodeType>("KB_PAD1DOWN_RS");

  auto MS_LEFT = EnsureGetMember<int>("MS_LEFT");
  auto MS_RIGHT = EnsureGetMember<int>("MS_RIGHT");

  auto GP_PAD1A = EnsureGetMember<ControllerButtonType>("GP_PAD1A");
  auto GP_PAD1B = EnsureGetMember<ControllerButtonType>("GP_PAD1B");
  auto GP_PAD1X = EnsureGetMember<ControllerButtonType>("GP_PAD1X");
  auto GP_PAD1Y = EnsureGetMember<ControllerButtonType>("GP_PAD1Y");
  auto GP_PAD1SELECT = EnsureGetMember<ControllerButtonType>("GP_PAD1SELECT");
  auto GP_PAD1START = EnsureGetMember<ControllerButtonType>("GP_PAD1START");
  auto GP_PAD1UP = EnsureGetMember<ControllerButtonType>("GP_PAD1UP");
  auto GP_PAD1DOWN = EnsureGetMember<ControllerButtonType>("GP_PAD1DOWN");
  auto GP_PAD1LEFT = EnsureGetMember<ControllerButtonType>("GP_PAD1LEFT");
  auto GP_PAD1RIGHT = EnsureGetMember<ControllerButtonType>("GP_PAD1RIGHT");
  auto GP_PAD1L1 = EnsureGetMember<ControllerButtonType>("GP_PAD1L1");
  auto GP_PAD1L3 = EnsureGetMember<ControllerButtonType>("GP_PAD1L3");
  auto GP_PAD1R1 = EnsureGetMember<ControllerButtonType>("GP_PAD1R1");
  auto GP_PAD1R3 = EnsureGetMember<ControllerButtonType>("GP_PAD1R3");

  auto GPA_PAD1LX = EnsureGetMember<ControllerAxisType>("GPA_PAD1LX");
  auto GPA_PAD1LY = EnsureGetMember<ControllerAxisType>("GPA_PAD1LY");
  auto GPA_PAD1RX = EnsureGetMember<ControllerAxisType>("GPA_PAD1RX");
  auto GPA_PAD1RY = EnsureGetMember<ControllerAxisType>("GPA_PAD1RY");
  auto GPA_PAD1LT = EnsureGetMember<ControllerAxisType>("GPA_PAD1LT");
  auto GPA_PAD1RT = EnsureGetMember<ControllerAxisType>("GPA_PAD1RT");

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

  PADcustomType = EnsureGetMember<uint32_t>("PADcustomType");
  PADcustomSizeA = EnsureGetMember<uint32_t>("PADcustomSizeA");
  PADcustomSizeB = EnsureGetMember<uint32_t>("PADcustomSizeB");
  GetMemberArray<uint32_t>(PADcustomA, PADcustomSizeA, "PADcustomA");
  GetMemberArray<uint32_t>(PADcustomB, PADcustomSizeB, "PADcustomB");
}

}  // namespace ScriptInput
}  // namespace Profile
}  // namespace Impacto