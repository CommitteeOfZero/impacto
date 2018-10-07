#include "expression.h"

#include "../game.h"

namespace Impacto {

namespace Vm {

uint32_t Evaluate(Sc3VmThread*, std::unique_ptr<VmExpressionNode>&);
void AssignValue(Sc3VmThread*, std::unique_ptr<VmExpressionNode>&);

void ExpressionInit() {
  // SYSTEMTIME time;
  // GetSystemTime(&time);
  // int ticks = GetTickCount();
  // srand(ticks + time.wDay + time.wDayOfWeek * time.wHour +
  //      time.wMilliseconds * time.wMinute - time.wMonth * time.wSecond);
}

int calMain(Sc3VmThread* thd, uint32_t* result) {
  ExpressionParser* expressionState = new ExpressionParser(thd);

  VmExpressionNode* root = expressionState->ParseSubExpression(0);

  std::unique_ptr<VmExpressionNode> rootPtr =
      std::unique_ptr<VmExpressionNode>(root);

  *result = Evaluate(thd, rootPtr);

  delete expressionState;

  return 0;
}

uint32_t Evaluate(Sc3VmThread* thd, std::unique_ptr<VmExpressionNode>& node) {
  uint32_t leftVal, rightVal;

  switch (node->ExprType) {
    case ET_Multiply:
      leftVal = Evaluate(thd, node->LeftExpr);
      rightVal = Evaluate(thd, node->RightExpr);
      return leftVal * rightVal;
    case ET_Divide:
      leftVal = Evaluate(thd, node->LeftExpr);
      rightVal = Evaluate(thd, node->RightExpr);
      if (!rightVal) return 0x7FFFFFFF;
      return leftVal / rightVal;
    case ET_Add:
      leftVal = Evaluate(thd, node->LeftExpr);
      rightVal = Evaluate(thd, node->RightExpr);
      return leftVal + rightVal;
    case ET_Subtract:
      leftVal = Evaluate(thd, node->LeftExpr);
      rightVal = Evaluate(thd, node->RightExpr);
      return leftVal - rightVal;
    case ET_Modulo:
      leftVal = Evaluate(thd, node->LeftExpr);
      rightVal = Evaluate(thd, node->RightExpr);
      if (!rightVal) return 0x7FFFFFFF;
      return leftVal % rightVal;
    case ET_LeftShift:
      leftVal = Evaluate(thd, node->LeftExpr);
      rightVal = Evaluate(thd, node->RightExpr);
      return leftVal << rightVal;
    case ET_RightShift:
      leftVal = Evaluate(thd, node->LeftExpr);
      rightVal = Evaluate(thd, node->RightExpr);
      return leftVal >> rightVal;
    case ET_BitwiseAnd:
      leftVal = Evaluate(thd, node->LeftExpr);
      rightVal = Evaluate(thd, node->RightExpr);
      return leftVal & rightVal;
    case ET_BitwiseXor:
      leftVal = Evaluate(thd, node->LeftExpr);
      rightVal = Evaluate(thd, node->RightExpr);
      return leftVal ^ rightVal;
    case ET_BitwiseOr:
      leftVal = Evaluate(thd, node->LeftExpr);
      rightVal = Evaluate(thd, node->RightExpr);
      return leftVal | rightVal;
    case ET_Equal:
      leftVal = Evaluate(thd, node->LeftExpr);
      rightVal = Evaluate(thd, node->RightExpr);
      return leftVal == rightVal;
    case ET_NotEqual:
      leftVal = Evaluate(thd, node->LeftExpr);
      rightVal = Evaluate(thd, node->RightExpr);
      return leftVal != rightVal;
    case ET_LessThanEqual:
      leftVal = Evaluate(thd, node->LeftExpr);
      rightVal = Evaluate(thd, node->RightExpr);
      return leftVal <= rightVal;
    case ET_MoreThanEqual:
      leftVal = Evaluate(thd, node->LeftExpr);
      rightVal = Evaluate(thd, node->RightExpr);
      return leftVal >= rightVal;
    case ET_LessThan:
      leftVal = Evaluate(thd, node->LeftExpr);
      rightVal = Evaluate(thd, node->RightExpr);
      return leftVal < rightVal;
    case ET_GreaterThan:
      leftVal = Evaluate(thd, node->LeftExpr);
      rightVal = Evaluate(thd, node->RightExpr);
      return leftVal > rightVal;
    case ET_Negation:
      leftVal = Evaluate(thd, node->LeftExpr);
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
      AssignValue(thd, node);
      return 0;
      break;
    case ET_ImmediateValue:
      return node->Value;
    case ET_FuncGlobalVars:
      return thd->GameContext->ScrWork[Evaluate(thd, node->RightExpr)];
    case ET_FuncFlags:
      return thd->GameContext->GetFlag(Evaluate(thd, node->RightExpr));
    case ET_FuncDataAccess:
      leftVal = Evaluate(thd, node->LeftExpr);
      rightVal = Evaluate(thd, node->RightExpr);
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
    case ET_FuncLabelTable:
      return ScriptGetLabelAddressNum(
          thd->VmContext->ScriptBuffers[thd->ScriptBufferId],
          Evaluate(thd, node->RightExpr));
    case ET_FuncFarLabelTable:
      return 0;
    case ET_FuncThreadVars:
      return *(uint32_t*)(thd->GetMemberPointer(
          thd, Evaluate(thd, node->RightExpr)));
    case ET_FuncDMA:
    case ET_FuncUnk2F:
    case ET_FuncUnk30:
    case ET_FuncNop31:
    case ET_FuncNop32:
      return 0;
    case ET_FuncRandom:
      return Evaluate(thd, node->RightExpr) * (rand() & 0x7FFF) >> 15;
  }
}

void AssignValue(Sc3VmThread* thd, std::unique_ptr<VmExpressionNode>& root) {
  int leftVal = Evaluate(thd, root->LeftExpr);
  int rightVal = 0;
  if (root->ExprType != ET_Increment && root->ExprType != ET_Decrement)
    rightVal = Evaluate(thd, root->RightExpr);

  switch (root->ExprType) {
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
  }

  int index = Evaluate(thd, root->LeftExpr->RightExpr);

  switch (root->LeftExpr->ExprType) {
    case ET_FuncGlobalVars:
      thd->GameContext->ScrWork[index] = leftVal;
      break;
    case ET_FuncFlags:
      thd->GameContext->SetFlag(index, leftVal);
      break;
    case ET_FuncThreadVars:
      uint32_t* thdWork = (uint32_t*)thd->GetMemberPointer(thd, index);
      *(thdWork) = leftVal;
      break;
  }
}

ExpressionParser::ExpressionParser(Sc3VmThread* thd) {
  GetTokens(thd);
  CurrentToken = 0;
}

VmExpressionNode* ExpressionParser::ParseSubExpression(int minPrecidence) {
  VmExpressionNode* leftExpr = ParseTerm();
  if (leftExpr == nullptr) return leftExpr;

  if (CurrentToken < Tokens.size()) {
    VmExprToken peek = Tokens[CurrentToken];
    if ((peek.Type == ET_Increment || peek.Type == ET_Decrement) &&
        peek.Precedence >= minPrecidence) {
      CurrentToken++;
      VmExpressionNode* result = new VmExpressionNode();
      result->ExprType = peek.Type;
      result->LeftExpr = std::unique_ptr<VmExpressionNode>(leftExpr);
      leftExpr = result;
      if (CurrentToken >= Tokens.size()) return leftExpr;
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
          VmExpressionNode* rightExpr = ParseSubExpression(peek.Precedence + 1);
          VmExpressionNode* result = new VmExpressionNode();
          result->ExprType = peek.Type;
          result->LeftExpr = std::unique_ptr<VmExpressionNode>(leftExpr);
          result->RightExpr = std::unique_ptr<VmExpressionNode>(rightExpr);
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

VmExpressionNode* ExpressionParser::ParseTerm() {
  VmExprToken tok = Tokens[CurrentToken++];
  VmExpressionNode* term = nullptr;
  switch (tok.Type) {
    case ET_ImmediateValue:
      term = new VmExpressionNode();
      term->ExprType = tok.Type;
      term->Value = tok.Value;
      break;
    case ET_Negation:
    case ET_FuncGlobalVars:
    case ET_FuncFlags:
    case ET_FuncLabelTable:
    case ET_FuncThreadVars:
    case ET_FuncRandom:
      term = new VmExpressionNode();
      term->ExprType = tok.Type;
      term->RightExpr = std::unique_ptr<VmExpressionNode>(
          ParseSubExpression(tok.Precedence + 1));
      break;
    case ET_FuncDataAccess:
    case ET_FuncFarLabelTable:
      term = new VmExpressionNode();
      term->ExprType = tok.Type;
      term->LeftExpr = std::unique_ptr<VmExpressionNode>(
          ParseSubExpression(tok.Precedence + 1));
      term->RightExpr = std::unique_ptr<VmExpressionNode>(
          ParseSubExpression(tok.Precedence + 1));
      break;
    default:
      return nullptr;
  }

  return term;
}

void ExpressionParser::GetTokens(Sc3VmThread* thd) {
  VmExprToken curToken;

  if (*thd->Ip) {
    do {
      int8_t tokenType = *thd->Ip;
      if (tokenType >= 0) {
        curToken.Type = (VmExprTokenType)tokenType;
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
            curToken.Value = (uint8_t)immValue[1] + ((tokenType & 0x1F) << 8);
            if (tokenType & 0x10) curToken.Value |= 0xFFFFE000;
            thd->Ip += 2;
            break;
          case 0x40:
            curToken.Value =
                (uint8_t)immValue[1] +
                (((uint8_t)immValue[2] + ((tokenType & 0x1F) << 8)) << 8);
            if (tokenType & 0x10) curToken.Value |= 0xFFE00000;
            thd->Ip += 3;
            break;
          case 0x60:
            curToken.Value = (((*(thd->Ip + 4) << 8) + +*(thd->Ip + 3))
                              << 8 + *(thd->Ip + 2))
                             << 8 + *(thd->Ip + 1);
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