#include "expression.h"

#include "../log.h"
#include "../mem.h"

#include <vector>
#include <memory>

namespace Impacto {

namespace Vm {

enum ExprTokenType {
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

struct ExprToken {
  ExprTokenType Type;
  int Value;
  int Precedence;
};

class ExpressionNode {
 public:
  ExprTokenType ExprType;

  std::unique_ptr<ExpressionNode> LeftExpr;
  std::unique_ptr<ExpressionNode> RightExpr;

  int Value;

  int Evaluate(Sc3VmThread* thd);
  void AssignValue(Sc3VmThread* thd);
};

class ExpressionParser {
 public:
  ExpressionParser(Sc3VmThread* thd);
  ExpressionNode* ParseSubExpression(int minPrecidence);

 private:
  int CurrentToken;
  std::vector<ExprToken> Tokens;
  void GetTokens(Sc3VmThread* thd);

  ExpressionNode* ParseTerm();
};

int ExpressionEval(Sc3VmThread* thd, int* result) {
  ExpressionParser* parser = new ExpressionParser(thd);

  ExpressionNode* root = parser->ParseSubExpression(0);

  std::unique_ptr<ExpressionNode> rootPtr =
      std::unique_ptr<ExpressionNode>(root);

  *result = rootPtr->Evaluate(thd);

  delete parser;

  return 0;
}

int ExpressionNode::Evaluate(Sc3VmThread* thd) {
  int leftVal, rightVal;

  switch (ExprType) {
    case ET_Multiply:
      leftVal = LeftExpr->Evaluate(thd);
      rightVal = RightExpr->Evaluate(thd);
      return leftVal * rightVal;
    case ET_Divide:
      leftVal = LeftExpr->Evaluate(thd);
      rightVal = RightExpr->Evaluate(thd);
      if (!rightVal) return 0x7FFFFFFF;
      return leftVal / rightVal;
    case ET_Add:
      leftVal = LeftExpr->Evaluate(thd);
      rightVal = RightExpr->Evaluate(thd);
      return leftVal + rightVal;
    case ET_Subtract:
      leftVal = LeftExpr->Evaluate(thd);
      rightVal = RightExpr->Evaluate(thd);
      return leftVal - rightVal;
    case ET_Modulo:
      leftVal = LeftExpr->Evaluate(thd);
      rightVal = RightExpr->Evaluate(thd);
      if (!rightVal) return 0x7FFFFFFF;
      return leftVal % rightVal;
    case ET_LeftShift:
      leftVal = LeftExpr->Evaluate(thd);
      rightVal = RightExpr->Evaluate(thd);
      return leftVal << rightVal;
    case ET_RightShift:
      leftVal = LeftExpr->Evaluate(thd);
      rightVal = RightExpr->Evaluate(thd);
      return leftVal >> rightVal;
    case ET_BitwiseAnd:
      leftVal = LeftExpr->Evaluate(thd);
      rightVal = RightExpr->Evaluate(thd);
      return leftVal & rightVal;
    case ET_BitwiseXor:
      leftVal = LeftExpr->Evaluate(thd);
      rightVal = RightExpr->Evaluate(thd);
      return leftVal ^ rightVal;
    case ET_BitwiseOr:
      leftVal = LeftExpr->Evaluate(thd);
      rightVal = RightExpr->Evaluate(thd);
      return leftVal | rightVal;
    case ET_Equal:
      leftVal = LeftExpr->Evaluate(thd);
      rightVal = RightExpr->Evaluate(thd);
      return leftVal == rightVal;
    case ET_NotEqual:
      leftVal = LeftExpr->Evaluate(thd);
      rightVal = RightExpr->Evaluate(thd);
      return leftVal != rightVal;
    case ET_LessThanEqual:
      leftVal = LeftExpr->Evaluate(thd);
      rightVal = RightExpr->Evaluate(thd);
      return leftVal <= rightVal;
    case ET_MoreThanEqual:
      leftVal = LeftExpr->Evaluate(thd);
      rightVal = RightExpr->Evaluate(thd);
      return leftVal >= rightVal;
    case ET_LessThan:
      leftVal = LeftExpr->Evaluate(thd);
      rightVal = RightExpr->Evaluate(thd);
      return leftVal < rightVal;
    case ET_GreaterThan:
      leftVal = LeftExpr->Evaluate(thd);
      rightVal = RightExpr->Evaluate(thd);
      return leftVal > rightVal;
    case ET_Negation:
      leftVal = LeftExpr->Evaluate(thd);
      return ~leftVal;
    case ET_Assign:
    case ET_MultiplyAssign:
    case ET_DivideAssign:
    case ET_AddAssign:
    case ET_SubtractAssign:
    case ET_ModuloAssign:
    case ET_LeftShiftAssign:
    case ET_RightShiftAssign:
    case ET_BitwiseAndAssign:
    case ET_BitwiseOrAssign:
    case ET_BitwiseXorAssign:
    case ET_Increment:
    case ET_Decrement:
      AssignValue(thd);
      return 0;
      break;
    case ET_ImmediateValue:
      return Value;
    case ET_FuncGlobalVars:
      return ScrWork[RightExpr->Evaluate(thd)];
    case ET_FuncFlags:
      return GetFlag(RightExpr->Evaluate(thd));
    case ET_FuncDataAccess:
      leftVal = LeftExpr->Evaluate(thd);
      rightVal = RightExpr->Evaluate(thd);
      if (leftVal >= 0) {
        uint8_t* scrBuf = ScriptBuffers[thd->ScriptBufferId];
        uint8_t* dataArray = (uint8_t*)&scrBuf[leftVal];
        return UnalignedRead<int>(&dataArray[rightVal * sizeof(int)]);
      } else {
        ImpLogSlow(LL_Warning, LC_Expr, "STUB token %02X evaluate\n", ExprType);
        // TODO: Handle this
        return 0;
      }
    case ET_FuncLabelTable:
      return ScriptGetLabelAddressNum(ScriptBuffers[thd->ScriptBufferId],
                                      RightExpr->Evaluate(thd));
    case ET_FuncFarLabelTable:
      return 0;
    case ET_FuncThreadVars:
      return UnalignedRead<uint32_t>(
          thd->GetMemberPointer(RightExpr->Evaluate(thd)));
    case ET_FuncDMA:
    case ET_FuncUnk2F:
    case ET_FuncUnk30:
    case ET_FuncNop31:
    case ET_FuncNop32:
      ImpLogSlow(LL_Warning, LC_Expr, "STUB token %02X evaluate\n", ExprType);
      return 0;
    case ET_FuncRandom:
      // TODO use our own RNG with our own seed
      return RightExpr->Evaluate(thd) * (rand() & 0x7FFF) >> 15;
    default:
      ImpLogSlow(LL_Warning, LC_Expr, "STUB token %02X evaluate\n", ExprType);
      return 0;
  }
}

void ExpressionNode::AssignValue(Sc3VmThread* thd) {
  int leftVal = LeftExpr->Evaluate(thd);
  int rightVal = 0;
  if (ExprType != ET_Increment && ExprType != ET_Decrement)
    rightVal = RightExpr->Evaluate(thd);

  switch (ExprType) {
    case ET_Assign:
      leftVal = rightVal;
      break;
    case ET_MultiplyAssign:
      leftVal *= rightVal;
      break;
    case ET_DivideAssign:
      leftVal /= rightVal;
      break;
    case ET_AddAssign:
      leftVal += rightVal;
      break;
    case ET_SubtractAssign:
      leftVal -= rightVal;
      break;
    case ET_ModuloAssign:
      leftVal %= rightVal;
      break;
    case ET_LeftShiftAssign:
      leftVal <<= rightVal;
      break;
    case ET_RightShiftAssign:
      leftVal >>= rightVal;
      break;
    case ET_BitwiseAndAssign:
      leftVal &= rightVal;
      break;
    case ET_BitwiseOrAssign:
      leftVal |= rightVal;
      break;
    case ET_BitwiseXorAssign:
      leftVal ^= rightVal;
      break;
    case ET_Increment:
      leftVal++;
      break;
    case ET_Decrement:
      leftVal--;
      break;
    default:
      ImpLogSlow(LL_Error, LC_Expr, "Tried to assign with unknown token %02X\n",
                 ExprType);
      break;
  }

  int index = LeftExpr->RightExpr->Evaluate(thd);

  switch (LeftExpr->ExprType) {
    case ET_FuncGlobalVars:
      ScrWork[index] = leftVal;
      break;
    case ET_FuncFlags:
      SetFlag(index, leftVal);
      break;
    case ET_FuncThreadVars: {
      void* thdWork = thd->GetMemberPointer(index);
      UnalignedWrite<int>(thdWork, leftVal);
      break;
    }
    case ET_FuncDMA:
      ImpLogSlow(LL_Warning, LC_Expr, "STUB token %02X assign\n",
                 LeftExpr->ExprType);
      break;
    default:
      ImpLogSlow(LL_Warning, LC_Expr, "STUB token %02X assign\n",
                 LeftExpr->ExprType);
      break;
  }
}

ExpressionParser::ExpressionParser(Sc3VmThread* thd) {
  GetTokens(thd);
  CurrentToken = 0;
}

ExpressionNode* ExpressionParser::ParseSubExpression(int minPrecidence) {
  ExpressionNode* leftExpr = ParseTerm();
  if (leftExpr == nullptr) return leftExpr;

  if (static_cast<size_t>(CurrentToken) < Tokens.size()) {
    ExprToken peek = Tokens[CurrentToken];
    if ((peek.Type == ET_Increment || peek.Type == ET_Decrement) &&
        peek.Precedence >= minPrecidence) {
      CurrentToken++;
      ExpressionNode* result = new ExpressionNode();
      result->ExprType = peek.Type;
      result->LeftExpr = std::unique_ptr<ExpressionNode>(leftExpr);
      leftExpr = result;
      if (static_cast<size_t>(CurrentToken) >= Tokens.size()) return leftExpr;
    }

    peek = Tokens[CurrentToken];

    while (peek.Precedence >= minPrecidence) {
      switch (peek.Type) {
        case ET_Multiply:
        case ET_Divide:
        case ET_Add:
        case ET_Subtract:
        case ET_Modulo:
        case ET_LeftShift:
        case ET_RightShift:
        case ET_BitwiseAnd:
        case ET_BitwiseXor:
        case ET_BitwiseOr:
        case ET_Equal:
        case ET_NotEqual:
        case ET_LessThanEqual:
        case ET_MoreThanEqual:
        case ET_LessThan:
        case ET_GreaterThan:
        case ET_Negation:
        case ET_Assign:
        case ET_MultiplyAssign:
        case ET_DivideAssign:
        case ET_AddAssign:
        case ET_SubtractAssign:
        case ET_ModuloAssign:
        case ET_LeftShiftAssign:
        case ET_RightShiftAssign:
        case ET_BitwiseAndAssign:
        case ET_BitwiseOrAssign:
        case ET_BitwiseXorAssign: {
          CurrentToken++;
          ExpressionNode* rightExpr = ParseSubExpression(peek.Precedence + 1);
          ExpressionNode* result = new ExpressionNode();
          result->ExprType = peek.Type;
          result->LeftExpr = std::unique_ptr<ExpressionNode>(leftExpr);
          result->RightExpr = std::unique_ptr<ExpressionNode>(rightExpr);
          leftExpr = result;
          if (static_cast<size_t>(CurrentToken) < Tokens.size())
            peek = Tokens[CurrentToken];
          else
            return leftExpr;
          break;
        }
        default:
          return leftExpr;
      }
    }
  } else
    return leftExpr;

  return leftExpr;
}

ExpressionNode* ExpressionParser::ParseTerm() {
  ExprToken tok = Tokens[CurrentToken++];
  ExpressionNode* term = nullptr;
  switch (tok.Type) {
    case ET_ImmediateValue:
      term = new ExpressionNode();
      term->ExprType = tok.Type;
      term->Value = tok.Value;
      break;
    case ET_Negation:
    case ET_FuncGlobalVars:
    case ET_FuncFlags:
    case ET_FuncLabelTable:
    case ET_FuncThreadVars:
    case ET_FuncRandom:
      term = new ExpressionNode();
      term->ExprType = tok.Type;
      term->RightExpr = std::unique_ptr<ExpressionNode>(
          ParseSubExpression(tok.Precedence + 1));
      break;
    case ET_FuncDataAccess:
    case ET_FuncFarLabelTable:
      term = new ExpressionNode();
      term->ExprType = tok.Type;
      term->LeftExpr = std::unique_ptr<ExpressionNode>(
          ParseSubExpression(tok.Precedence + 1));
      term->RightExpr = std::unique_ptr<ExpressionNode>(
          ParseSubExpression(tok.Precedence + 1));
      break;
      // ????
      // in RN03_16A.scr there is an instruction parameter that is just "=43"
    case ET_Assign:
      tok = Tokens[CurrentToken++];
      term = new ExpressionNode();
      term->ExprType = tok.Type;
      term->Value = tok.Value;
      break;
    default:
      return nullptr;
  }

  return term;
}

void ExpressionParser::GetTokens(Sc3VmThread* thd) {
  ExprToken curToken;

  if (*thd->Ip) {
    do {
      int8_t tokenType = *thd->Ip;
      if (tokenType >= 0) {
        curToken.Type = (ExprTokenType)tokenType;
        curToken.Precedence = *(++thd->Ip);
        thd->Ip++;
        curToken.Value = 0;
        Tokens.push_back(curToken);
      } else {
        uint8_t* immValue = thd->Ip;
        curToken.Type = ET_ImmediateValue;
        switch (tokenType & 0x60) {
          case 0:
            curToken.Value = tokenType & 0x1F;
            if (tokenType & 0x10) curToken.Value |= 0xFFFFFFE0;
            thd->Ip++;
            break;
          case 0x20:
            curToken.Value = ((immValue[0] & 0x1F) << 8) + immValue[1];
            if (tokenType & 0x10) curToken.Value |= 0xFFFFE000;
            thd->Ip += 2;
            break;
          case 0x40:
            curToken.Value =
                ((immValue[0] & 0x1F) << 16) + (immValue[2] << 8) + immValue[1];
            if (tokenType & 0x10) curToken.Value |= 0xFFE00000;
            thd->Ip += 3;
            break;
          case 0x60:
            curToken.Value = immValue[1] + (immValue[2] << 8) +
                             (immValue[3] << 16) + (immValue[4] << 24);
            thd->Ip += 5;
            break;
          default:
            break;
        }
        curToken.Precedence = *(++thd->Ip);
        Tokens.push_back(curToken);
      }
    } while (*thd->Ip);
  }

  thd->Ip++;
}

}  // namespace Vm

}  // namespace Impacto