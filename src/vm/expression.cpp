#include "expression.h"

#include "../game.h"

namespace Impacto {

namespace Vm {

int CurrentToken;
std::vector<VmExprToken> Tokens;
std::vector<VmExprToken> GetTokens(Sc3VmThread* thd);

VmExpressionNode* ParseSubExpression(int minPrecidence);
VmExpressionNode* ParseTerm();
uint32_t Evaluate(Sc3VmThread*, std::unique_ptr<VmExpressionNode>&);
void AssignValue(Sc3VmThread*, std::unique_ptr<VmExpressionNode>&);
void* GetMemberPointer(Sc3VmThread* thd, uint32_t offset);

void ExpressionInit() {
  // SYSTEMTIME time;
  // GetSystemTime(&time);
  // int ticks = GetTickCount();
  // srand(ticks + time.wDay + time.wDayOfWeek * time.wHour +
  //      time.wMilliseconds * time.wMinute - time.wMonth * time.wSecond);
}

int calMain(Sc3VmThread* thd, uint32_t* result) {
  Tokens = GetTokens(thd);
  CurrentToken = 0;

  VmExpressionNode* root = ParseSubExpression(0);

  std::unique_ptr<VmExpressionNode> rootPtr =
      std::unique_ptr<VmExpressionNode>(root);

  *result = Evaluate(thd, rootPtr);

  return 0;
}

uint32_t Evaluate(Sc3VmThread* thd, std::unique_ptr<VmExpressionNode>& root) {
  uint32_t leftVal, rightVal;

  switch (root->exprType) {
    case ET_Multiplication:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal * rightVal;
    case ET_Division:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      if (!rightVal) return 0x7FFFFFFF;
      return leftVal / rightVal;
    case ET_Addition:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal + rightVal;
    case ET_Subtraction:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal - rightVal;
    case ET_Modulo:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      if (!rightVal) return 0x7FFFFFFF;
      return leftVal % rightVal;
    case ET_ShiftLeft:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal << rightVal;
    case ET_ShiftRight:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal >> rightVal;
    case ET_BinaryAnd:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal & rightVal;
    case ET_BinaryXor:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal ^ rightVal;
    case ET_BinaryOr:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal | rightVal;
    case ET_LogicEquals:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal == rightVal;
    case ET_LogicNotEquals:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal != rightVal;
    case ET_LogicLessOrEqual:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal <= rightVal;
    case ET_LogicMoreOrEqual:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal >= rightVal;
    case ET_LogicLess:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal < rightVal;
    case ET_LogicMore:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal > rightVal;
    case ET_Negation:
      leftVal = Evaluate(thd, root->leftExpr);
      return ~leftVal;
    case ET_Assign:
    case ET_MulAssign:
    case ET_DivAssgin:
    case ET_AddAssign:
    case ET_SubAssign:
    case ET_ModAssign:
    case ET_LShAssign:
    case ET_RShAssign:
    case ET_AndAssign:
    case ET_OrAssign:
    case ET_XorAssign:
    case ET_Increment:
    case ET_Decrement:
      AssignValue(thd, root);
      return 0;
      break;
    case ET_Value:
      return root->value;
    case ET_ScrWorkAcc:
      return thd->GameContext->ScrWork[Evaluate(thd, root->rightExpr)];
    case ET_FlagWorkAcc:
      return thd->GameContext->GetFlag(Evaluate(thd, root->rightExpr));
    case ET_ScrBufAcc:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      if (leftVal >= 0) {
        uint8_t* scrBuf = thd->VmContext->ScriptBuffers[thd->ScriptBufferId];
        uint32_t* dataAdrRel = (uint32_t*)scrBuf[leftVal];
        uint32_t dataItemAdrRel = dataAdrRel[rightVal];
        return *(uint32_t*)&scrBuf[dataItemAdrRel];
      } else {
        // TODO: Handle this
        return 0;
      }
      return 0;
    case ET_LabelTableAcc:
      return ScriptGetLabelAddressNum(
          thd->VmContext->ScriptBuffers[thd->ScriptBufferId],
          Evaluate(thd, root->rightExpr));
    case ET_FarLabelTableAcc:
      return 0;
    case ET_ThdWorkAcc:
      return *(
          uint32_t*)(GetMemberPointer(thd, Evaluate(thd, root->rightExpr)));
    case ET_MemoryAcc:
      return 0;
    case ET_Random:
      return Evaluate(thd, root->rightExpr) * (rand() & 0x7FFF) >> 15;
  }
}

void AssignValue(Sc3VmThread* thd, std::unique_ptr<VmExpressionNode>& root) {
  int leftVal = Evaluate(thd, root->leftExpr);
  int rightVal = 0;
  if (root->exprType != ET_Increment && root->exprType != ET_Decrement)
    rightVal = Evaluate(thd, root->rightExpr);

  switch (root->exprType) {
    case ET_Assign:
      leftVal = rightVal;
      break;
    case ET_MulAssign:
      leftVal *= rightVal;
      break;
    case ET_DivAssgin:
      leftVal /= rightVal;
      break;
    case ET_AddAssign:
      leftVal += rightVal;
      break;
    case ET_SubAssign:
      leftVal -= rightVal;
      break;
    case ET_ModAssign:
      leftVal %= rightVal;
      break;
    case ET_LShAssign:
      leftVal <<= rightVal;
      break;
    case ET_RShAssign:
      leftVal >>= rightVal;
      break;
    case ET_AndAssign:
      leftVal &= rightVal;
      break;
    case ET_OrAssign:
      leftVal |= rightVal;
      break;
    case ET_XorAssign:
      leftVal ^= rightVal;
      break;
    case ET_Increment:
      leftVal++;
      break;
    case ET_Decrement:
      leftVal--;
      break;
  }

  int index = Evaluate(thd, root->leftExpr->rightExpr);

  switch (root->leftExpr->exprType) {
    case ET_ScrWorkAcc:
      thd->GameContext->ScrWork[index] = leftVal;
      break;
    case ET_FlagWorkAcc:
      thd->GameContext->SetFlag(index, leftVal);
      break;
    case ET_ThdWorkAcc:
      uint32_t* thdWork = (uint32_t*)GetMemberPointer(thd, index);
      *(thdWork) = leftVal;
      break;
  }
}

VmExpressionNode* ParseSubExpression(int minPrecidence) {
  VmExpressionNode* leftExpr = ParseTerm();
  if (leftExpr == nullptr) return leftExpr;

  if (CurrentToken < Tokens.size()) {
    VmExprToken peek = Tokens[CurrentToken];
    if ((peek.type == ET_Increment || peek.type == ET_Decrement) &&
        peek.precedence >= minPrecidence) {
      CurrentToken++;
      VmExpressionNode* result = new VmExpressionNode();
      result->exprType = peek.type;
      result->leftExpr = std::unique_ptr<VmExpressionNode>(leftExpr);
      leftExpr = result;
      if (CurrentToken >= Tokens.size()) return leftExpr;
    }

    peek = Tokens[CurrentToken];

    while (peek.precedence >= minPrecidence) {
      switch (peek.type) {
        case ET_Multiplication:
        case ET_Division:
        case ET_Addition:
        case ET_Subtraction:
        case ET_Modulo:
        case ET_ShiftLeft:
        case ET_ShiftRight:
        case ET_BinaryAnd:
        case ET_BinaryXor:
        case ET_BinaryOr:
        case ET_LogicEquals:
        case ET_LogicNotEquals:
        case ET_LogicLessOrEqual:
        case ET_LogicMoreOrEqual:
        case ET_LogicLess:
        case ET_LogicMore:
        case ET_Negation:
        case ET_Assign:
        case ET_MulAssign:
        case ET_DivAssgin:
        case ET_AddAssign:
        case ET_SubAssign:
        case ET_ModAssign:
        case ET_LShAssign:
        case ET_RShAssign:
        case ET_AndAssign:
        case ET_OrAssign:
        case ET_XorAssign: {
          CurrentToken++;
          VmExpressionNode* rightExpr = ParseSubExpression(peek.precedence + 1);
          VmExpressionNode* result = new VmExpressionNode();
          result->exprType = peek.type;
          result->leftExpr = std::unique_ptr<VmExpressionNode>(leftExpr);
          result->rightExpr = std::unique_ptr<VmExpressionNode>(rightExpr);
          leftExpr = result;
          if (CurrentToken < Tokens.size())
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

VmExpressionNode* ParseTerm() {
  VmExprToken tok = Tokens[CurrentToken++];
  VmExpressionNode* term = nullptr;
  switch (tok.type) {
    case ET_Value:
      term = new VmExpressionNode();
      term->exprType = tok.type;
      term->value = tok.value;
      break;
    case ET_Negation:
    case ET_ScrWorkAcc:
    case ET_FlagWorkAcc:
    case ET_LabelTableAcc:
    case ET_ThdWorkAcc:
    case ET_Random:
      term = new VmExpressionNode();
      term->exprType = tok.type;
      term->rightExpr = std::unique_ptr<VmExpressionNode>(
          ParseSubExpression(tok.precedence + 1));
      break;
    case ET_ScrBufAcc:
    case ET_FarLabelTableAcc:
      term = new VmExpressionNode();
      term->exprType = tok.type;
      term->leftExpr = std::unique_ptr<VmExpressionNode>(
          ParseSubExpression(tok.precedence + 1));
      term->rightExpr = std::unique_ptr<VmExpressionNode>(
          ParseSubExpression(tok.precedence + 1));
      break;
    default:
      return nullptr;
  }

  return term;
}

std::vector<VmExprToken> GetTokens(Sc3VmThread* thd) {
  std::vector<VmExprToken> tokens;
  VmExprToken curToken;

  if (*thd->Ip) {
    do {
      int8_t tokenType = *thd->Ip;
      if (tokenType >= 0) {
        curToken.type = (VmExprTokenType)tokenType;
        curToken.precedence = *(++thd->Ip);
        thd->Ip++;
        curToken.value = 0;
        tokens.push_back(curToken);
      } else {
        uint8_t* immValue = thd->Ip;
        curToken.type = ET_Value;
        switch (tokenType & 0x60) {
          case 0:
            curToken.value = tokenType & 0x1F;
            if (tokenType & 0x10) curToken.value |= 0xFFFFFFE0;
            thd->Ip++;
            break;
          case 0x20:
            curToken.value = (uint8_t)immValue[1] + ((tokenType & 0x1F) << 8);
            if (tokenType & 0x10) curToken.value |= 0xFFFFE000;
            thd->Ip += 2;
            break;
          case 0x40:
            curToken.value =
                (uint8_t)immValue[1] +
                (((uint8_t)immValue[2] + ((tokenType & 0x1F) << 8)) << 8);
            if (tokenType & 0x10) curToken.value |= 0xFFE00000;
            thd->Ip += 3;
            break;
          case 0x60:
            curToken.value = (((*(thd->Ip + 4) << 8) + +*(thd->Ip + 3))
                              << 8 + *(thd->Ip + 2))
                             << 8 + *(thd->Ip + 1);
            thd->Ip += 5;
            break;
          default:
            break;
        }
        curToken.precedence = *(++thd->Ip);
        tokens.push_back(curToken);
      }
    } while (*thd->Ip);
  }

  thd->Ip++;

  return tokens;
}

}  // namespace Vm

}  // namespace Impacto