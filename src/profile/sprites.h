#pragma once

#include "../spritesheet.h"
#include "../util.h"
#include <ankerl/unordered_dense.h>

namespace Impacto {
namespace Profile {

inline ankerl::unordered_dense::map<std::string, SpriteSheet, string_hash,
                                    std::equal_to<>>
    SpriteSheets;
inline ankerl::unordered_dense::map<std::string, Sprite, string_hash,
                                    std::equal_to<>>
    Sprites;

void LoadSpritesheets();

}  // namespace Profile
}  // namespace Impacto