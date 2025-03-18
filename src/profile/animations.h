#pragma once

#include <ankerl/unordered_dense.h>
#include "../spriteanimation.h"
#include "../util.h"

namespace Impacto {
namespace Profile {

inline ankerl::unordered_dense::map<std::string, SpriteAnimationDef,
                                    string_hash, std::equal_to<>>
    Animations;

void LoadAnimations();

}  // namespace Profile
}  // namespace Impacto