#pragma once

#include "textpage.h"

namespace Impacto {

struct TipsPage : public TextPage {
 public:
  void AddString(Vm::Sc3VmThread* ctx);
};

}  // namespace Impacto
