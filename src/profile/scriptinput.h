#pragma once

#include "../util.h"
#include <flat_hash_map.hpp>

namespace Impacto {
namespace Profile {
namespace ScriptInput {

enum InputAxisDir {
  AxisPos = 1,
  AxisNeg = -1,
};

inline ska::flat_hash_map<uint32_t, std::vector<uint32_t>> PADToKeyboard;
inline ska::flat_hash_map<uint32_t, uint32_t> PADToMouse;
inline ska::flat_hash_map<uint32_t, uint32_t> PADToController;
inline ska::flat_hash_map<uint32_t, std::pair<uint32_t, InputAxisDir>>
    PADToControllerAxis;

inline uint32_t PADcustomType;
inline uint32_t PADcustomSizeA;
inline uint32_t PADcustomSizeB;
inline uint32_t PADcustomA[65];
inline uint32_t PADcustomB[65];

void Configure();

}  // namespace ScriptInput
}  // namespace Profile
}  // namespace Impacto