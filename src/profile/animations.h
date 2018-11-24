#pragma once

#include <flat_hash_map.hpp>
#include "../spriteanimation.h"

namespace Impacto {
namespace Profile {

extern ska::flat_hash_map<std::string, SpriteAnimationDef> Animations;

void LoadAnimations();

}  // namespace Profile
}  // namespace Impacto