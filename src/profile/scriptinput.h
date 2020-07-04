#pragma once

#include "../util.h"
#include <flat_hash_map.hpp>

namespace Impacto {
namespace Profile {
namespace ScriptInput {

extern ska::flat_hash_map<int, int> PADToKeyboard;
extern ska::flat_hash_map<int, int> PADToMouse;
extern ska::flat_hash_map<int, int> PADToController;

extern int KB_PAD1A;
extern int KB_PAD1B;
extern int KB_PAD1X;
extern int KB_PAD1Y;
extern int KB_PAD1SELECT;
extern int KB_PAD1START;
extern int KB_PAD1UP;
extern int KB_PAD1DOWN;
extern int KB_PAD1LEFT;
extern int KB_PAD1RIGHT;
extern int KB_PAD1L1;
extern int KB_PAD1L2;
extern int KB_PAD1L3;
extern int KB_PAD1R1;
extern int KB_PAD1R2;
extern int KB_PAD1R3;

extern int MS_PAD1A;
extern int MS_PAD1B;

extern int GP_PAD1A;
extern int GP_PAD1B;
extern int GP_PAD1X;
extern int GP_PAD1Y;
extern int GP_PAD1SELECT;
extern int GP_PAD1START;
extern int GP_PAD1UP;
extern int GP_PAD1DOWN;
extern int GP_PAD1LEFT;
extern int GP_PAD1RIGHT;
extern int GP_PAD1L1;
extern int GP_PAD1L2;
extern int GP_PAD1L3;
extern int GP_PAD1R1;
extern int GP_PAD1R2;
extern int GP_PAD1R3;

void Configure();

}  // namespace ScriptInput
}  // namespace Profile
}  // namespace Impacto