#pragma once

#include "../io/io.h"
#include "texture.h"

namespace Impacto {
namespace TexLoad {
bool TextureIsGXT(SDL_RWops* stream);
bool TextureLoadGXT(SDL_RWops* stream, Texture* outTexture);
void VitaUnswizzle(int* x, int* y, int width, int height);
}  // namespace TexLoad
}  // namespace Impacto