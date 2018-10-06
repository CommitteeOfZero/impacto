#include "expression.h"

#include "../game.h"

namespace Impacto {
namespace Vm {

int g_CurrentToken;
std::vector<VmExprToken> g_Tokens;
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
  g_Tokens = GetTokens(thd);
  g_CurrentToken = 0;

  VmExpressionNode* root = ParseSubExpression(0);

  std::unique_ptr<VmExpressionNode> rootPtr =
      std::unique_ptr<VmExpressionNode>(root);

  *result = Evaluate(thd, rootPtr);

  return 0;
}

uint32_t Evaluate(Sc3VmThread* thd, std::unique_ptr<VmExpressionNode>& root) {
  uint32_t leftVal, rightVal;

  switch (root->exprType) {
    case VmExprTokenType::Multiplication:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal * rightVal;
    case VmExprTokenType::Division:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      if (!rightVal) return 0x7FFFFFFF;
      return leftVal / rightVal;
    case VmExprTokenType::Addition:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal + rightVal;
    case VmExprTokenType::Subtraction:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal - rightVal;
    case VmExprTokenType::Modulo:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      if (!rightVal) return 0x7FFFFFFF;
      return leftVal % rightVal;
    case VmExprTokenType::ShiftLeft:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal << rightVal;
    case VmExprTokenType::ShiftRight:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal >> rightVal;
    case VmExprTokenType::BinaryAnd:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal & rightVal;
    case VmExprTokenType::BinaryXor:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal ^ rightVal;
    case VmExprTokenType::BinaryOr:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal | rightVal;
    case VmExprTokenType::LogicEquals:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal == rightVal;
    case VmExprTokenType::LogicNotEquals:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal != rightVal;
    case VmExprTokenType::LogicLessOrEqual:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal <= rightVal;
    case VmExprTokenType::LogicMoreOrEqual:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal >= rightVal;
    case VmExprTokenType::LogicLess:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal < rightVal;
    case VmExprTokenType::LogicMore:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      return leftVal > rightVal;
    case VmExprTokenType::Negation:
      leftVal = Evaluate(thd, root->leftExpr);
      return ~leftVal;
    case VmExprTokenType::Assign:
    case VmExprTokenType::MulAssign:
    case VmExprTokenType::DivAssgin:
    case VmExprTokenType::AddAssign:
    case VmExprTokenType::SubAssign:
    case VmExprTokenType::ModAssign:
    case VmExprTokenType::LShAssign:
    case VmExprTokenType::RShAssign:
    case VmExprTokenType::AndAssign:
    case VmExprTokenType::OrAssign:
    case VmExprTokenType::XorAssign:
    case VmExprTokenType::Increment:
    case VmExprTokenType::Decrement:
      AssignValue(thd, root);
      return 0;
      break;
    case VmExprTokenType::Value:
      return root->value;
    case VmExprTokenType::ScrWorkAcc:
      return thd->GameContext->ScrWork[Evaluate(thd, root->rightExpr)];
    case VmExprTokenType::FlagWorkAcc:
      return thd->GameContext->GetFlag(Evaluate(thd, root->rightExpr));
    case VmExprTokenType::ScrBufAcc:
      leftVal = Evaluate(thd, root->leftExpr);
      rightVal = Evaluate(thd, root->rightExpr);
      if (leftVal >= 0) {
        uint8_t* scrBuf = thd->VmContext->ScriptBuffers[thd->ScriptBufferId];
        uint8_t* arr = scrBuf + leftVal;
        return (*(uint8_t*)(arr + 4 * rightVal + 3) << 24) |
               (*(uint8_t*)(arr + 4 * rightVal + 2) << 16) |
               (*(uint8_t*)(arr + 4 * rightVal + 1) << 8) |
               *(uint8_t*)(arr + 4 * rightVal);
      } else {
        // TODO: Handle this
        return 0;
      }
      return 0;
    case VmExprTokenType::LabelTableAcc:
      return 0;
      // return SCRgetLabelAddressNum(thd->script_buffer_id,
      //                             evaluate(thd, root->rightExpr));
    case VmExprTokenType::FarLabelTableAcc:
      return 0;
    case VmExprTokenType::ThdWorkAcc:
      return *(
          uint32_t*)(GetMemberPointer(thd, Evaluate(thd, root->rightExpr)));
    case VmExprTokenType::MemoryAcc:
      return 0;
    case VmExprTokenType::Random:
      return Evaluate(thd, root->rightExpr) * (rand() & 0x7FFF) >> 15;
  }
}

void AssignValue(Sc3VmThread* thd, std::unique_ptr<VmExpressionNode>& root) {
  int leftVal = Evaluate(thd, root->leftExpr);
  int rightVal = 0;
  if (root->exprType != VmExprTokenType::Increment &&
      root->exprType != VmExprTokenType::Decrement)
    rightVal = Evaluate(thd, root->rightExpr);

  switch (root->exprType) {
    case VmExprTokenType::Assign:
      leftVal = rightVal;
      break;
    case VmExprTokenType::MulAssign:
      leftVal *= rightVal;
      break;
    case VmExprTokenType::DivAssgin:
      leftVal /= rightVal;
      break;
    case VmExprTokenType::AddAssign:
      leftVal += rightVal;
      break;
    case VmExprTokenType::SubAssign:
      leftVal -= rightVal;
      break;
    case VmExprTokenType::ModAssign:
      leftVal %= rightVal;
      break;
    case VmExprTokenType::LShAssign:
      leftVal <<= rightVal;
      break;
    case VmExprTokenType::RShAssign:
      leftVal >>= rightVal;
      break;
    case VmExprTokenType::AndAssign:
      leftVal &= rightVal;
      break;
    case VmExprTokenType::OrAssign:
      leftVal |= rightVal;
      break;
    case VmExprTokenType::XorAssign:
      leftVal ^= rightVal;
      break;
    case VmExprTokenType::Increment:
      leftVal++;
      break;
    case VmExprTokenType::Decrement:
      leftVal--;
      break;
  }

  int index = Evaluate(thd, root->leftExpr->rightExpr);

  switch (root->leftExpr->exprType) {
    case VmExprTokenType::ScrWorkAcc:
      thd->GameContext->ScrWork[index] = leftVal;
      break;
    case VmExprTokenType::FlagWorkAcc:
      thd->GameContext->SetFlag(index, leftVal);
      break;
    case VmExprTokenType::ThdWorkAcc:
      uint32_t* thdWork = (uint32_t*)GetMemberPointer(thd, index);
      *(thdWork) = leftVal;
      break;
  }
}

VmExpressionNode* ParseSubExpression(int minPrecidence) {
  VmExpressionNode* leftExpr = ParseTerm();
  if (leftExpr == nullptr) return leftExpr;

  if (g_CurrentToken < g_Tokens.size()) {
    VmExprToken peek = g_Tokens[g_CurrentToken];
    if ((peek.type == VmExprTokenType::Increment ||
         peek.type == VmExprTokenType::Decrement) &&
        peek.precedence >= minPrecidence) {
      g_CurrentToken++;
      VmExpressionNode* result = new VmExpressionNode();
      result->exprType = peek.type;
      result->leftExpr = std::unique_ptr<VmExpressionNode>(leftExpr);
      leftExpr = result;
      if (g_CurrentToken >= g_Tokens.size()) return leftExpr;
    }

    peek = g_Tokens[g_CurrentToken];

    while (peek.precedence >= minPrecidence) {
      switch (peek.type) {
        case VmExprTokenType::Multiplication:
        case VmExprTokenType::Division:
        case VmExprTokenType::Addition:
        case VmExprTokenType::Subtraction:
        case VmExprTokenType::Modulo:
        case VmExprTokenType::ShiftLeft:
        case VmExprTokenType::ShiftRight:
        case VmExprTokenType::BinaryAnd:
        case VmExprTokenType::BinaryXor:
        case VmExprTokenType::BinaryOr:
        case VmExprTokenType::LogicEquals:
        case VmExprTokenType::LogicNotEquals:
        case VmExprTokenType::LogicLessOrEqual:
        case VmExprTokenType::LogicMoreOrEqual:
        case VmExprTokenType::LogicLess:
        case VmExprTokenType::LogicMore:
        case VmExprTokenType::Negation:
        case VmExprTokenType::Assign:
        case VmExprTokenType::MulAssign:
        case VmExprTokenType::DivAssgin:
        case VmExprTokenType::AddAssign:
        case VmExprTokenType::SubAssign:
        case VmExprTokenType::ModAssign:
        case VmExprTokenType::LShAssign:
        case VmExprTokenType::RShAssign:
        case VmExprTokenType::AndAssign:
        case VmExprTokenType::OrAssign:
        case VmExprTokenType::XorAssign: {
          g_CurrentToken++;
          VmExpressionNode* rightExpr = ParseSubExpression(peek.precedence + 1);
          VmExpressionNode* result = new VmExpressionNode();
          result->exprType = peek.type;
          result->leftExpr = std::unique_ptr<VmExpressionNode>(leftExpr);
          result->rightExpr = std::unique_ptr<VmExpressionNode>(rightExpr);
          leftExpr = result;
          if (g_CurrentToken < g_Tokens.size())
            peek = g_Tokens[g_CurrentToken];
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
  VmExprToken tok = g_Tokens[g_CurrentToken++];
  VmExpressionNode* term = nullptr;
  switch (tok.type) {
    case VmExprTokenType::Value:
      term = new VmExpressionNode();
      term->exprType = tok.type;
      term->value = tok.value;
      break;
    case VmExprTokenType::Negation:
    case VmExprTokenType::ScrWorkAcc:
    case VmExprTokenType::FlagWorkAcc:
    case VmExprTokenType::LabelTableAcc:
    case VmExprTokenType::ThdWorkAcc:
    case VmExprTokenType::Random:
      term = new VmExpressionNode();
      term->exprType = tok.type;
      term->rightExpr = std::unique_ptr<VmExpressionNode>(
          ParseSubExpression(tok.precedence + 1));
      break;
    case VmExprTokenType::ScrBufAcc:
    case VmExprTokenType::FarLabelTableAcc:
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
        curToken.type = VmExprTokenType::Value;
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

void* GetMemberPointer(Sc3VmThread* thd, uint32_t offset) {
  switch (offset) {
    case TO_Flag:
      return &thd->Flag;
    case TO_ExecPri:
      return &thd->ExecPriority;
    case TO_ScrBuf:
      return &thd->ScriptBufferId;
    case TO_WaitCount:
      return &thd->WaitCounter;
    case TO_ScrParam:
      return &thd->ScriptParam;
    case TO_ScrAddr:
      return &thd->Ip;
    case TO_LoopCount:
      return &thd->LoopCounter;
    case TO_LoopAddr:
      return &thd->LoopAddress;
    case TO_RetCount:
      return &thd->ReturnCount;
    case TO_RetAddr:
      return &thd->ReturnAdresses;
    case TO_RetScrBuf:
      return &thd->ReturnGroupIds;
    case TO_DrawPri:
      return &thd->DrawPriority;
    case TO_DrawType:
      return &thd->DrawType;
    case TO_Alpha:
      return &thd->Alpha;
    case TO_Temp1:
      return &thd->Temp1;
    case TO_Temp2:
      return &thd->Temp2;
    default:
      if ((offset - TO_ThdVarBegin) >= 0) {
        return &thd->Variables[offset - TO_ThdVarBegin];
      }
      break;
  }
}

}  // namespace Vm
}  // namespace Impacto