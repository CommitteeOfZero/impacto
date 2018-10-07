#pragma once

#include <vector>
#include <stack>
#include <memory>

#include "vm.h"

namespace Impacto {

namespace Vm {

void ExpressionInit();

int calMain(Sc3VmThread* thread, uint32_t* result);
int CalculateAccel(int a1, int a2, int a3);

enum VmExprTokenType {
  ET_EndOfExpression = 0x00,
  ET_Value = 0xFF,

  ET_Multiplication = 0x01,
  ET_Division = 0x02,
  ET_Addition = 0x03,
  ET_Subtraction = 0x04,
  ET_Modulo = 0x05,
  ET_ShiftLeft = 0x06,
  ET_ShiftRight = 0x07,
  ET_BinaryAnd = 0x08,
  ET_BinaryXor = 0x09,
  ET_BinaryOr = 0x0A,
  ET_LogicEquals = 0x0C,
  ET_LogicNotEquals = 0x0D,
  ET_LogicLessOrEqual = 0x0E,
  ET_LogicMoreOrEqual = 0x0F,
  ET_LogicLess = 0x10,
  ET_LogicMore = 0x11,

  ET_Negation = 0x0B,
  ET_Increment = 0x20,
  ET_Decrement = 0x21,

  ET_Assign = 0x14,
  ET_MulAssign = 0x15,
  ET_DivAssgin = 0x16,
  ET_AddAssign = 0x17,
  ET_SubAssign = 0x18,
  ET_ModAssign = 0x19,
  ET_LShAssign = 0x1A,
  ET_RShAssign = 0x1B,
  ET_AndAssign = 0x1C,
  ET_OrAssign = 0x1D,
  ET_XorAssign = 0x1E,

  ET_ScrWorkAcc = 0x28,
  ET_FlagWorkAcc = 0x29,
  ET_ScrBufAcc = 0x2A,
  ET_LabelTableAcc = 0x2B,
  ET_FarLabelTableAcc = 0x2C,
  ET_ThdWorkAcc = 0x2D,
  ET_MemoryAcc = 0x2E,
  ET_Random = 0x33
};

struct VmExprToken {
  VmExprTokenType type;
  int value;
  int precedence;
};

class VmExpressionNode {
 public:
  VmExprTokenType exprType;

  std::unique_ptr<VmExpressionNode> leftExpr;
  std::unique_ptr<VmExpressionNode> rightExpr;

  int value;
};

}  // namespace Vm

}  // namespace Impacto