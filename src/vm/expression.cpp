#include "expression.h"

namespace Impacto {
namespace Vm {

int _currentToken;
CALCExpressionNode* parseSubExpression(int minPrecidence);
CALCExpressionNode* parseTerm();
uint32_t evaluate(Sc3VmThread*, std::unique_ptr<CALCExpressionNode>&);
void assignValue(Sc3VmThread*, std::unique_ptr<CALCExpressionNode>&);
std::vector<CALCToken> getTokens(Sc3VmThread* thd);
std::vector<CALCToken> _tokens;

void CALCinit() {
  // SYSTEMTIME time;
  // GetSystemTime(&time);
  // int ticks = GetTickCount();
  // srand(ticks + time.wDay + time.wDayOfWeek * time.wHour +
  //      time.wMilliseconds * time.wMinute - time.wMonth * time.wSecond);
}

int calMain(Sc3VmThread* thd, uint32_t* result) {
  _tokens = getTokens(thd);
  _currentToken = 0;

  CALCExpressionNode* root = parseSubExpression(0);

  std::unique_ptr<CALCExpressionNode> rootPtr =
      std::unique_ptr<CALCExpressionNode>(root);

  *result = evaluate(thd, rootPtr);

  return 0;
}

uint32_t evaluate(Sc3VmThread* thd, std::unique_ptr<CALCExpressionNode>& root) {
  uint32_t leftVal, rightVal;

  switch (root->exprType) {
    case CALCTokenType::Multiplication:
      leftVal = evaluate(thd, root->leftExpr);
      rightVal = evaluate(thd, root->rightExpr);
      return leftVal * rightVal;
    case CALCTokenType::Division:
      leftVal = evaluate(thd, root->leftExpr);
      rightVal = evaluate(thd, root->rightExpr);
      if (!rightVal) return 0x7FFFFFFF;
      return leftVal / rightVal;
    case CALCTokenType::Addition:
      leftVal = evaluate(thd, root->leftExpr);
      rightVal = evaluate(thd, root->rightExpr);
      return leftVal + rightVal;
    case CALCTokenType::Subtraction:
      leftVal = evaluate(thd, root->leftExpr);
      rightVal = evaluate(thd, root->rightExpr);
      return leftVal - rightVal;
    case CALCTokenType::Modulo:
      leftVal = evaluate(thd, root->leftExpr);
      rightVal = evaluate(thd, root->rightExpr);
      if (!rightVal) return 0x7FFFFFFF;
      return leftVal % rightVal;
    case CALCTokenType::ShiftLeft:
      leftVal = evaluate(thd, root->leftExpr);
      rightVal = evaluate(thd, root->rightExpr);
      return leftVal << rightVal;
    case CALCTokenType::ShiftRight:
      leftVal = evaluate(thd, root->leftExpr);
      rightVal = evaluate(thd, root->rightExpr);
      return leftVal >> rightVal;
    case CALCTokenType::BinaryAnd:
      leftVal = evaluate(thd, root->leftExpr);
      rightVal = evaluate(thd, root->rightExpr);
      return leftVal & rightVal;
    case CALCTokenType::BinaryXor:
      leftVal = evaluate(thd, root->leftExpr);
      rightVal = evaluate(thd, root->rightExpr);
      return leftVal ^ rightVal;
    case CALCTokenType::BinaryOr:
      leftVal = evaluate(thd, root->leftExpr);
      rightVal = evaluate(thd, root->rightExpr);
      return leftVal | rightVal;
    case CALCTokenType::LogicEquals:
      leftVal = evaluate(thd, root->leftExpr);
      rightVal = evaluate(thd, root->rightExpr);
      return leftVal == rightVal;
    case CALCTokenType::LogicNotEquals:
      leftVal = evaluate(thd, root->leftExpr);
      rightVal = evaluate(thd, root->rightExpr);
      return leftVal != rightVal;
    case CALCTokenType::LogicLessOrEqual:
      leftVal = evaluate(thd, root->leftExpr);
      rightVal = evaluate(thd, root->rightExpr);
      return leftVal <= rightVal;
    case CALCTokenType::LogicMoreOrEqual:
      leftVal = evaluate(thd, root->leftExpr);
      rightVal = evaluate(thd, root->rightExpr);
      return leftVal >= rightVal;
    case CALCTokenType::LogicLess:
      leftVal = evaluate(thd, root->leftExpr);
      rightVal = evaluate(thd, root->rightExpr);
      return leftVal < rightVal;
    case CALCTokenType::LogicMore:
      leftVal = evaluate(thd, root->leftExpr);
      rightVal = evaluate(thd, root->rightExpr);
      return leftVal > rightVal;
    case CALCTokenType::Negation:
      leftVal = evaluate(thd, root->leftExpr);
      return ~leftVal;
    case CALCTokenType::Assign:
    case CALCTokenType::MulAssign:
    case CALCTokenType::DivAssgin:
    case CALCTokenType::AddAssign:
    case CALCTokenType::SubAssign:
    case CALCTokenType::ModAssign:
    case CALCTokenType::LShAssign:
    case CALCTokenType::RShAssign:
    case CALCTokenType::AndAssign:
    case CALCTokenType::OrAssign:
    case CALCTokenType::XorAssign:
    case CALCTokenType::Increment:
    case CALCTokenType::Decrement:
      assignValue(thd, root);
      return 0;
      break;
    case CALCTokenType::Value:
      return root->value;
    case CALCTokenType::ScrWorkAcc:
      return 0;
      // return ScrWork[evaluate(thd, root->rightExpr)];
    case CALCTokenType::FlagWorkAcc:
      return 0;
      // return GetFlag(evaluate(thd, root->rightExpr));
    case CALCTokenType::ScrBufAcc:
      leftVal = evaluate(thd, root->leftExpr);
      rightVal = evaluate(thd, root->rightExpr);
      if (leftVal >= 0) {
        // unsigned __int8* script =
        //    (unsigned __int8*)ScrBuf[thd->script_buffer_id];
        // unsigned __int8* array = script + leftVal;
        // return (*(unsigned __int8*)(array + 4 * rightVal + 3) << 24) |
        //       (*(unsigned __int8*)(array + 4 * rightVal + 2) << 16) |
        //       (*(unsigned __int8*)(array + 4 * rightVal + 1) << 8) |
        //       *(unsigned __int8*)(array + 4 * rightVal);
      } else {
        // uint8_t *array = (uint8_t *)getSC3ThreadContext(-a); // parameter
        // is thread ID int32_t result = READ_LITTLE_ENDIAN_INT32(array + (4
        // * b));
      }
      return 0;
    case CALCTokenType::LabelTableAcc:
      return 0;
      // return SCRgetLabelAddressNum(thd->script_buffer_id,
      //                             evaluate(thd, root->rightExpr));
    case CALCTokenType::FarLabelTableAcc:
      return 0;
    case CALCTokenType::ThdWorkAcc:
      return 0;
      // return *(&thd->THD_FLAG + evaluate(thd, root->rightExpr));
    case CALCTokenType::MemoryAcc:
      return 0;
    case CALCTokenType::Random:
      return evaluate(thd, root->rightExpr) * (rand() & 0x7FFF) >> 15;
  }
}

void assignValue(Sc3VmThread* thd, std::unique_ptr<CALCExpressionNode>& root) {
  int leftVal = evaluate(thd, root->leftExpr);
  int rightVal = 0;
  if (root->exprType != CALCTokenType::Increment &&
      root->exprType != CALCTokenType::Decrement)
    rightVal = evaluate(thd, root->rightExpr);

  switch (root->exprType) {
    case CALCTokenType::Assign:
      leftVal = rightVal;
      break;
    case CALCTokenType::MulAssign:
      leftVal *= rightVal;
      break;
    case CALCTokenType::DivAssgin:
      leftVal /= rightVal;
      break;
    case CALCTokenType::AddAssign:
      leftVal += rightVal;
      break;
    case CALCTokenType::SubAssign:
      leftVal -= rightVal;
      break;
    case CALCTokenType::ModAssign:
      leftVal %= rightVal;
      break;
    case CALCTokenType::LShAssign:
      leftVal <<= rightVal;
      break;
    case CALCTokenType::RShAssign:
      leftVal >>= rightVal;
      break;
    case CALCTokenType::AndAssign:
      leftVal &= rightVal;
      break;
    case CALCTokenType::OrAssign:
      leftVal |= rightVal;
      break;
    case CALCTokenType::XorAssign:
      leftVal ^= rightVal;
      break;
    case CALCTokenType::Increment:
      leftVal++;
      break;
    case CALCTokenType::Decrement:
      leftVal--;
      break;
  }

  int index = evaluate(thd, root->leftExpr->rightExpr);

  switch (root->leftExpr->exprType) {
    case CALCTokenType::ScrWorkAcc:
      // ScrWork[index] = leftVal;
      break;
    case CALCTokenType::FlagWorkAcc:
      // SetFlag(index, leftVal);
      break;
    case CALCTokenType::ThdWorkAcc:
      //*(&thd->THD_FLAG + index) = leftVal;
      break;
  }
}

CALCExpressionNode* parseSubExpression(int minPrecidence) {
  CALCExpressionNode* leftExpr = parseTerm();
  if (leftExpr == nullptr) return leftExpr;

  if (_currentToken < _tokens.size()) {
    CALCToken peek = _tokens[_currentToken];
    if ((peek.type == CALCTokenType::Increment ||
         peek.type == CALCTokenType::Decrement) &&
        peek.precedence >= minPrecidence) {
      _currentToken++;
      CALCExpressionNode* result = new CALCExpressionNode();
      result->exprType = peek.type;
      result->leftExpr = std::unique_ptr<CALCExpressionNode>(leftExpr);
      leftExpr = result;
      if (_currentToken >= _tokens.size()) return leftExpr;
    }

    peek = _tokens[_currentToken];

    while (peek.precedence >= minPrecidence) {
      switch (peek.type) {
        case CALCTokenType::Multiplication:
        case CALCTokenType::Division:
        case CALCTokenType::Addition:
        case CALCTokenType::Subtraction:
        case CALCTokenType::Modulo:
        case CALCTokenType::ShiftLeft:
        case CALCTokenType::ShiftRight:
        case CALCTokenType::BinaryAnd:
        case CALCTokenType::BinaryXor:
        case CALCTokenType::BinaryOr:
        case CALCTokenType::LogicEquals:
        case CALCTokenType::LogicNotEquals:
        case CALCTokenType::LogicLessOrEqual:
        case CALCTokenType::LogicMoreOrEqual:
        case CALCTokenType::LogicLess:
        case CALCTokenType::LogicMore:
        case CALCTokenType::Negation:
        case CALCTokenType::Assign:
        case CALCTokenType::MulAssign:
        case CALCTokenType::DivAssgin:
        case CALCTokenType::AddAssign:
        case CALCTokenType::SubAssign:
        case CALCTokenType::ModAssign:
        case CALCTokenType::LShAssign:
        case CALCTokenType::RShAssign:
        case CALCTokenType::AndAssign:
        case CALCTokenType::OrAssign:
        case CALCTokenType::XorAssign: {
          _currentToken++;
          CALCExpressionNode* rightExpr =
              parseSubExpression(peek.precedence + 1);
          CALCExpressionNode* result = new CALCExpressionNode();
          result->exprType = peek.type;
          result->leftExpr = std::unique_ptr<CALCExpressionNode>(leftExpr);
          result->rightExpr = std::unique_ptr<CALCExpressionNode>(rightExpr);
          leftExpr = result;
          if (_currentToken < _tokens.size())
            peek = _tokens[_currentToken];
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

CALCExpressionNode* parseTerm() {
  CALCToken tok = _tokens[_currentToken++];
  CALCExpressionNode* term = nullptr;
  switch (tok.type) {
    case CALCTokenType::Value:
      term = new CALCExpressionNode();
      term->exprType = tok.type;
      term->value = tok.value;
      break;
    case CALCTokenType::Negation:
    case CALCTokenType::ScrWorkAcc:
    case CALCTokenType::FlagWorkAcc:
    case CALCTokenType::LabelTableAcc:
    case CALCTokenType::ThdWorkAcc:
    case CALCTokenType::Random:
      term = new CALCExpressionNode();
      term->exprType = tok.type;
      term->rightExpr = std::unique_ptr<CALCExpressionNode>(
          parseSubExpression(tok.precedence + 1));
      break;
    case CALCTokenType::ScrBufAcc:
    case CALCTokenType::FarLabelTableAcc:
      term = new CALCExpressionNode();
      term->exprType = tok.type;
      term->leftExpr = std::unique_ptr<CALCExpressionNode>(
          parseSubExpression(tok.precedence + 1));
      term->rightExpr = std::unique_ptr<CALCExpressionNode>(
          parseSubExpression(tok.precedence + 1));
      break;
    default:
      return nullptr;
  }

  return term;
}

std::vector<CALCToken> getTokens(Sc3VmThread* thd) {
  std::vector<CALCToken> tokens;
  CALCToken curToken;

  if (*thd->Ip) {
    do {
      int8_t tokenType = *thd->Ip;
      if (tokenType >= 0) {
        curToken.type = (CALCTokenType)tokenType;
        curToken.precedence = *(++thd->Ip);
        thd->Ip++;
        curToken.value = 0;
        tokens.push_back(curToken);
      } else {
        uint8_t* immValue = thd->Ip;
        curToken.type = CALCTokenType::Value;
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