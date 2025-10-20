#pragma once

#include "../impacto.h"
#include <ankerl/unordered_dense.h>

namespace Impacto {
namespace Profile {
namespace Charset {

inline ankerl::unordered_dense::map<uint32_t, uint16_t> CharacterToSc3;

void Load();

}  // namespace Charset
}  // namespace Profile
}  // namespace Impacto