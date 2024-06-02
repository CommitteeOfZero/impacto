#pragma once

#include "../io/io.h"
#include "texture.h"

namespace Impacto {
namespace TexLoad {
bool TextureIsPlain(Io::Stream* stream);
bool TextureLoadPlain(Io::Stream* stream, Texture* outTexture);
}  // namespace TexLoad
}  // namespace Impacto