#include "scriptinput.h"
#include "profile_internal.h"

#include "../vm/interface/input.h"

namespace Impacto {
namespace Profile {
namespace ScriptInput {

using namespace Impacto::Vm::Interface;

void Configure() {
  EnsurePushMemberOfType("Input", LUA_TTABLE);

  KB_PAD1A = EnsureGetMemberInt("KB_PAD1A");
  KB_PAD1B = EnsureGetMemberInt("KB_PAD1B");
  KB_PAD1X = EnsureGetMemberInt("KB_PAD1X");
  KB_PAD1Y = EnsureGetMemberInt("KB_PAD1Y");
  KB_PAD1SELECT = EnsureGetMemberInt("KB_PAD1SELECT");
  KB_PAD1START = EnsureGetMemberInt("KB_PAD1START");
  KB_PAD1UP = EnsureGetMemberInt("KB_PAD1UP");
  KB_PAD1DOWN = EnsureGetMemberInt("KB_PAD1DOWN");
  KB_PAD1LEFT = EnsureGetMemberInt("KB_PAD1LEFT");
  KB_PAD1RIGHT = EnsureGetMemberInt("KB_PAD1RIGHT");
  KB_PAD1L1 = EnsureGetMemberInt("KB_PAD1L1");
  KB_PAD1L2 = EnsureGetMemberInt("KB_PAD1L2");
  KB_PAD1L3 = EnsureGetMemberInt("KB_PAD1L3");
  KB_PAD1R1 = EnsureGetMemberInt("KB_PAD1R1");
  KB_PAD1R2 = EnsureGetMemberInt("KB_PAD1R2");
  KB_PAD1R3 = EnsureGetMemberInt("KB_PAD1R3");

  MS_LEFT = EnsureGetMemberInt("MS_LEFT");
  MS_RIGHT = EnsureGetMemberInt("MS_RIGHT");

  GP_PAD1A = EnsureGetMemberInt("GP_PAD1A");
  GP_PAD1B = EnsureGetMemberInt("GP_PAD1B");
  GP_PAD1X = EnsureGetMemberInt("GP_PAD1X");
  GP_PAD1Y = EnsureGetMemberInt("GP_PAD1Y");
  GP_PAD1SELECT = EnsureGetMemberInt("GP_PAD1SELECT");
  GP_PAD1START = EnsureGetMemberInt("GP_PAD1START");
  GP_PAD1UP = EnsureGetMemberInt("GP_PAD1UP");
  GP_PAD1DOWN = EnsureGetMemberInt("GP_PAD1DOWN");
  GP_PAD1LEFT = EnsureGetMemberInt("GP_PAD1LEFT");
  GP_PAD1RIGHT = EnsureGetMemberInt("GP_PAD1RIGHT");
  GP_PAD1L1 = EnsureGetMemberInt("GP_PAD1L1");
  GP_PAD1L2 = EnsureGetMemberInt("GP_PAD1L2");
  GP_PAD1L3 = EnsureGetMemberInt("GP_PAD1L3");
  GP_PAD1R1 = EnsureGetMemberInt("GP_PAD1R1");
  GP_PAD1R2 = EnsureGetMemberInt("GP_PAD1R2");
  GP_PAD1R3 = EnsureGetMemberInt("GP_PAD1R3");

  PADToKeyboard[PAD1A] = KB_PAD1A;
  PADToKeyboard[PAD1B] = KB_PAD1B;
  PADToKeyboard[PAD1X] = KB_PAD1X;
  PADToKeyboard[PAD1Y] = KB_PAD1Y;
  PADToKeyboard[PAD1SELECT] = KB_PAD1SELECT;
  PADToKeyboard[PAD1START] = KB_PAD1START;
  PADToKeyboard[PAD1UP] = KB_PAD1UP;
  PADToKeyboard[PAD1DOWN] = KB_PAD1DOWN;
  PADToKeyboard[PAD1LEFT] = KB_PAD1LEFT;
  PADToKeyboard[PAD1RIGHT] = KB_PAD1RIGHT;
  PADToKeyboard[PAD1L1] = KB_PAD1L1;
  PADToKeyboard[PAD1L2] = KB_PAD1L2;
  PADToKeyboard[PAD1L3] = KB_PAD1L3;
  PADToKeyboard[PAD1R1] = KB_PAD1R1;
  PADToKeyboard[PAD1R2] = KB_PAD1R2;
  PADToKeyboard[PAD1R3] = KB_PAD1R3;

  PADToMouse[PAD1A] = MS_LEFT;
  PADToMouse[PAD1B] = MS_RIGHT;
  PADToMouse[PAD1START] = MS_RIGHT;

  PADToController[PAD1A] = GP_PAD1A;
  PADToController[PAD1B] = GP_PAD1B;
  PADToController[PAD1X] = GP_PAD1X;
  PADToController[PAD1Y] = GP_PAD1Y;
  PADToController[PAD1SELECT] = GP_PAD1SELECT;
  PADToController[PAD1START] = GP_PAD1START;
  PADToController[PAD1UP] = GP_PAD1UP;
  PADToController[PAD1DOWN] = GP_PAD1DOWN;
  PADToController[PAD1LEFT] = GP_PAD1LEFT;
  PADToController[PAD1RIGHT] = GP_PAD1RIGHT;
  PADToController[PAD1L1] = GP_PAD1L1;
  PADToController[PAD1L2] = GP_PAD1L2;
  PADToController[PAD1L3] = GP_PAD1L3;
  PADToController[PAD1R1] = GP_PAD1R1;
  PADToController[PAD1R2] = GP_PAD1R2;
  PADToController[PAD1R3] = GP_PAD1R3;

  Pop();

  PADcustomType = EnsureGetMemberInt("PADcustomType");
  PADcustomSizeA = EnsureGetMemberInt("PADcustomSizeA");
  PADcustomSizeB = EnsureGetMemberInt("PADcustomSizeB");
  GetMemberUintArray(PADcustomA, PADcustomSizeA, "PADcustomA");
  GetMemberUintArray(PADcustomB, PADcustomSizeB, "PADcustomB");
}

}  // namespace ScriptInput
}  // namespace Profile
}  // namespace Impacto