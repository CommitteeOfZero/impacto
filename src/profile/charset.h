#pragma once

#include "../impacto.h"
#include <flat_hash_map.hpp>

namespace Impacto {
namespace Profile {
namespace Charset {

extern uint8_t *Flags;
extern ska::flat_hash_map<uint32_t, uint16_t> CharacterToSc3;

void Load();

}  // namespace Charset
}  // namespace Profile
}  // namespace Impacto