#pragma once

#include "../spritesheet.h"
#include <ankerl/unordered_dense.h>

namespace Impacto {
namespace Profile {

inline ankerl::unordered_dense::map<std::string, SpriteSheet> SpriteSheets;
inline ankerl::unordered_dense::map<std::string, Sprite> Sprites;

void LoadSpritesheets();

}  // namespace Profile
}  // namespace Impacto