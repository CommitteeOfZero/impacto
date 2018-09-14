#pragma once

#include "../io/io.h"
#include "texture.h"

namespace Impacto {
bool TextureLoadGXT(SDL_RWops* stream, Texture* outTexture);

namespace TexLoad {
void VitaUnswizzle(int* x, int* y, int width, int height);
}
}  // namespace Impacto