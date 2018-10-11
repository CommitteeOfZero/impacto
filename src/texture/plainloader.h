#pragma once

#include "../io/io.h"
#include "texture.h"

namespace Impacto {
namespace TexLoad {
bool TextureIsPlain(SDL_RWops* stream);
bool TextureLoadPlain(SDL_RWops* stream, Texture* outTexture);
}  // namespace TexLoad
}  // namespace Impacto