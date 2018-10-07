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
  ET_ImmediateValue = 0xFF,

  ET_Multiply = 0x01,
  ET_Divide = 0x02,
  ET_Add = 0x03,
  ET_Subtract = 0x04,
  ET_Modulo = 0x05,
  ET_LeftShift = 0x06,
  ET_RightShift = 0x07,
  ET_BitwiseAnd = 0x08,
  ET_BitwiseXor = 0x09,
  ET_BitwiseOr = 0x0A,
  ET_Equal = 0x0C,
  ET_NotEqual = 0x0D,
  ET_LessThanEqual = 0x0E,
  ET_MoreThanEqual = 0x0F,
  ET_LessThan = 0x10,
  ET_GreaterThan = 0x11,

  ET_Negation = 0x0B,
  ET_Increment = 0x20,
  ET_Decrement = 0x21,

  ET_Assign = 0x14,
  ET_MultiplyAssign = 0x15,
  ET_DivideAssign = 0x16,
  ET_AddAssign = 0x17,
  ET_SubtractAssign = 0x18,
  ET_ModuloAssign = 0x19,
  ET_LeftShiftAssign = 0x1A,
  ET_RightShiftAssign = 0x1B,
  ET_BitwiseAndAssign = 0x1C,
  ET_BitwiseOrAssign = 0x1D,
  ET_BitwiseXorAssign = 0x1E,

  ET_FuncGlobalVars = 0x28,
  ET_FuncFlags = 0x29,
  ET_FuncDataAccess = 0x2A,
  ET_FuncLabelTable = 0x2B,
  ET_FuncFarLabelTable = 0x2C,
  ET_FuncThreadVars = 0x2D,
  ET_FuncDMA = 0x2E,
  ET_FuncUnk2F = 0x2F,
  ET_FuncUnk30 = 0x30,
  ET_FuncNop31 = 0x31,
  ET_FuncNop32 = 0x32,
  ET_FuncRandom = 0x33
};

struct VmExprToken {
  VmExprTokenType Type;
  int Value;
  int Precedence;
};

class VmExpressionNode {
 public:
  VmExprTokenType ExprType;

  std::unique_ptr<VmExpressionNode> LeftExpr;
  std::unique_ptr<VmExpressionNode> RightExpr;

  int Value;
};

class ExpressionParser {
 public:
  ExpressionParser(Sc3VmThread* thd);
  VmExpressionNode* ParseSubExpression(int minPrecidence);

 private:
  int CurrentToken;
  std::vector<VmExprToken> Tokens;
  void GetTokens(Sc3VmThread* thd);

  VmExpressionNode* ParseTerm();
};

}  // namespace Vm

}  // namespace Impacto