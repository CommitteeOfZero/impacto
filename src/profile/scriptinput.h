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

inline ska::flat_hash_map<uint32_t, uint32_t> PADToKeyboard;
inline ska::flat_hash_map<uint32_t, uint32_t> PADToMouse;
inline ska::flat_hash_map<uint32_t, uint32_t> PADToController;
inline ska::flat_hash_map<uint32_t, std::pair<uint32_t, InputAxisDir>>
    PADToControllerAxis;

inline uint32_t KB_PAD1A;
inline uint32_t KB_PAD1B;
inline uint32_t KB_PAD1X;
inline uint32_t KB_PAD1Y;
inline uint32_t KB_PAD1SELECT;
inline uint32_t KB_PAD1START;
inline uint32_t KB_PAD1UP;
inline uint32_t KB_PAD1DOWN;
inline uint32_t KB_PAD1LEFT;
inline uint32_t KB_PAD1RIGHT;
inline uint32_t KB_PAD1L1;
inline uint32_t KB_PAD1L2;
inline uint32_t KB_PAD1L3;
inline uint32_t KB_PAD1R1;
inline uint32_t KB_PAD1R2;
inline uint32_t KB_PAD1R3;
inline uint32_t KB_PAD1L1_ALT;
inline uint32_t KB_PAD1UP_RS;
inline uint32_t KB_PAD1DOWN_RS;

inline uint32_t MS_LEFT;
inline uint32_t MS_RIGHT;

inline uint32_t GP_PAD1A;
inline uint32_t GP_PAD1B;
inline uint32_t GP_PAD1X;
inline uint32_t GP_PAD1Y;
inline uint32_t GP_PAD1SELECT;
inline uint32_t GP_PAD1START;
inline uint32_t GP_PAD1UP;
inline uint32_t GP_PAD1DOWN;
inline uint32_t GP_PAD1LEFT;
inline uint32_t GP_PAD1RIGHT;
inline uint32_t GP_PAD1L1;
inline uint32_t GP_PAD1L3;
inline uint32_t GP_PAD1R1;
inline uint32_t GP_PAD1R3;

inline uint32_t GPA_PAD1LX;
inline uint32_t GPA_PAD1LY;
inline uint32_t GPA_PAD1RX;
inline uint32_t GPA_PAD1RY;
inline uint32_t GPA_PAD1LT;
inline uint32_t GPA_PAD1RT;

inline uint32_t PADcustomType;
inline uint32_t PADcustomSizeA;
inline uint32_t PADcustomSizeB;
inline uint32_t PADcustomA[65];
inline uint32_t PADcustomB[65];

void Configure();

}  // namespace ScriptInput
}  // namespace Profile
}  // namespace Impacto