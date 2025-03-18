#pragma once

#include "../util.h"
#include <ankerl/unordered_dense.h>

namespace Impacto {
namespace Profile {
namespace ScriptInput {

enum InputAxisDir {
  AxisPos = 1,
  AxisNeg = -1,
};

inline ankerl::unordered_dense::map<uint32_t, std::vector<uint32_t>>
    PADToKeyboard;
inline ankerl::unordered_dense::map<uint32_t, uint32_t> PADToMouse;
inline ankerl::unordered_dense::map<uint32_t, uint32_t> PADToController;
inline ankerl::unordered_dense::map<uint32_t, std::pair<uint32_t, InputAxisDir>>
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