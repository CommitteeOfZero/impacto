#pragma once

#include "../../data/tipssystem.h"

namespace Impacto {
namespace Profile {
namespace TipsSystem {

extern Impacto::TipsSystem::TipsSystemType Type;

extern int MaxTipsCount;
extern uint32_t MaxTipDataSize;

void Configure();

}  // namespace TipsSystem
}  // namespace Profile
}  // namespace Impacto