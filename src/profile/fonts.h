#pragma once

#include "../font.h"
#include <ankerl/unordered_dense.h>

namespace Impacto {
namespace Profile {

inline ankerl::unordered_dense::map<std::string, Font*> Fonts;

void LoadFonts();

}  // namespace Profile
}  // namespace Impacto