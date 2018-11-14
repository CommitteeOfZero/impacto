#pragma once

#include "../io/io.h"
#include "texture.h"

namespace Impacto {
namespace TexLoad {
bool TextureIsPlain(Io::InputStream* stream);
bool TextureLoadPlain(Io::InputStream* stream, Texture* outTexture);
}  // namespace TexLoad
}  // namespace Impacto