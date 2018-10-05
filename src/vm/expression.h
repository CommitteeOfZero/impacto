#pragma once

#include <vector>
#include <stack>
#include <memory>

#include "vm.h"

namespace Impacto {

namespace Vm {

void CALCinit();

int calMain(Sc3VmThread* thread, uint32_t* result);
int CALCaccel(int a1, int a2, int a3);

enum CALCTokenType {
  EndOfExpression = 0x00,
  Value = 0xFF,

  Multiplication = 0x01,
  Division = 0x02,
  Addition = 0x03,
  Subtraction = 0x04,
  Modulo = 0x05,
  ShiftLeft = 0x06,
  ShiftRight = 0x07,
  BinaryAnd = 0x08,
  BinaryXor = 0x09,
  BinaryOr = 0x0A,
  LogicEquals = 0x0C,
  LogicNotEquals = 0x0D,
  LogicLessOrEqual = 0x0E,
  LogicMoreOrEqual = 0x0F,
  LogicLess = 0x10,
  LogicMore = 0x11,

  Negation = 0x0B,
  Increment = 0x20,
  Decrement = 0x21,

  Assign = 0x14,
  MulAssign = 0x15,
  DivAssgin = 0x16,
  AddAssign = 0x17,
  SubAssign = 0x18,
  ModAssign = 0x19,
  LShAssign = 0x1A,
  RShAssign = 0x1B,
  AndAssign = 0x1C,
  OrAssign = 0x1D,
  XorAssign = 0x1E,

  ScrWorkAcc = 0x28,
  FlagWorkAcc = 0x29,
  ScrBufAcc = 0x2A,
  LabelTableAcc = 0x2B,
  FarLabelTableAcc = 0x2C,
  ThdWorkAcc = 0x2D,
  MemoryAcc = 0x2E,
  Random = 0x33
};

struct CALCToken {
  CALCTokenType type;
  int value;
  int precedence;
};

class CALCExpressionNode {
 public:
  CALCTokenType exprType;

  std::unique_ptr<CALCExpressionNode> leftExpr;
  std::unique_ptr<CALCExpressionNode> rightExpr;

  int value;
};

}  // namespace Vm

}  // namespace Impacto