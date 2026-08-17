#pragma once

#include "../text/fonts/font.h"
#include "../util.h"
#include <ankerl/unordered_dense.h>

namespace Impacto {
namespace Profile {

inline ankerl::unordered_dense::map<std::string, Fonts::Font*, string_hash,
                                    std::equal_to<>>
    Fonts;

void LoadFonts();

}  // namespace Profile
}  // namespace Impacto