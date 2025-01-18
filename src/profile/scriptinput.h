#pragma once

#include "../util.h"
#include <flat_hash_map.hpp>

namespace Impacto {
namespace Profile {
namespace ScriptInput {

inline ska::flat_hash_map<int, int> PADToKeyboard;
inline ska::flat_hash_map<int, int> PADToMouse;
inline ska::flat_hash_map<int, int> PADToController;

inline int KB_PAD1A;
inline int KB_PAD1B;
inline int KB_PAD1X;
inline int KB_PAD1Y;
inline int KB_PAD1SELECT;
inline int KB_PAD1START;
inline int KB_PAD1UP;
inline int KB_PAD1DOWN;
inline int KB_PAD1LEFT;
inline int KB_PAD1RIGHT;
inline int KB_PAD1L1;
inline int KB_PAD1L2;
inline int KB_PAD1L3;
inline int KB_PAD1R1;
inline int KB_PAD1R2;
inline int KB_PAD1R3;

inline int MS_LEFT;
inline int MS_RIGHT;

inline int GP_PAD1A;
inline int GP_PAD1B;
inline int GP_PAD1X;
inline int GP_PAD1Y;
inline int GP_PAD1SELECT;
inline int GP_PAD1START;
inline int GP_PAD1UP;
inline int GP_PAD1DOWN;
inline int GP_PAD1LEFT;
inline int GP_PAD1RIGHT;
inline int GP_PAD1L1;
inline int GP_PAD1L2;
inline int GP_PAD1L3;
inline int GP_PAD1R1;
inline int GP_PAD1R2;
inline int GP_PAD1R3;

inline int PADcustomType;
inline int PADcustomSizeA;
inline int PADcustomSizeB;
inline uint32_t PADcustomA[65];
inline uint32_t PADcustomB[65];

void Configure();

}  // namespace ScriptInput
}  // namespace Profile
}  // namespace Impacto