#include "tipspage.h"

#include "textparser.h"

namespace Impacto {

void TipsPage::AddString(Vm::Sc3VmThread* const ctx) {
  Clear();
  TipsTextParserInst.ParseString(*this, ctx);
}

}  // namespace Impacto
