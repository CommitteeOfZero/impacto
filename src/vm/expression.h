#pragma once
#pragma once

#include "vm.h"
#include "sc3stream.h"

namespace Impacto {

namespace Vm {

int ExpressionEval(Sc3VmThread* thread, bool isString = false);
int ExpressionSkip(uint8_t* ip);
}  // namespace Vm

}  // namespace Impacto