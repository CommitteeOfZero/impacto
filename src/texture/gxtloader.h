#pragma once

#include "../io/io.h"
#include "texture.h"

namespace Impacto {
namespace TexLoad {
void VitaUnswizzle(int* x, int* y, int width, int height);
}  // namespace TexLoad
}  // namespace Impacto