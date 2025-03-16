#pragma once

#include <ankerl/unordered_dense.h>
#include "../spriteanimation.h"

namespace Impacto {
namespace Profile {

inline ankerl::unordered_dense::map<std::string, SpriteAnimationDef> Animations;

void LoadAnimations();

}  // namespace Profile
}  // namespace Impacto