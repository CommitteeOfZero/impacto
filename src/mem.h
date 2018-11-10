#pragma once

#include "impacto.h"

namespace Impacto {

static int const ScrWorkSize = 8000;
static int const FlagWorkSize = 1000;

extern int ScrWork[ScrWorkSize];
extern uint8_t FlagWork[FlagWorkSize];

void SetFlag(uint32_t flagId, uint32_t value);
bool GetFlag(uint32_t flagId);

}  // namespace Impacto