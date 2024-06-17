#pragma once

#include "../spritesheet.h"
#include <flat_hash_map.hpp>

namespace Impacto {
namespace Profile {

inline ska::flat_hash_map<std::string, SpriteSheet> SpriteSheets;
inline ska::flat_hash_map<std::string, Sprite> Sprites;

void LoadSpritesheets();

}  // namespace Profile
}  // namespace Impacto