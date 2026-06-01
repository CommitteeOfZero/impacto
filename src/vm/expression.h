#pragma once

#include "vm.h"
#include "sc3stream.h"

namespace Impacto {

namespace Vm {

int ExpressionEval(Sc3VmThread* thread);
int ExpressionSkip(uint8_t* ip);

}  // namespace Vm

}  // namespace Impacto