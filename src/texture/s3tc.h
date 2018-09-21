#pragma once

#include <SDL_rwops.h>

namespace Impacto {
namespace TexLoad {
// Note: Unlike Benjamin's original implementation, these decompress to 24-bit
// RGB, not 32-bit 1RGB
void DecompressBlockDXT1(uint32_t startX, uint32_t startY, uint32_t imageWidth,
                         SDL_RWops* inputStream, uint8_t* outputImage);
void BlockDecompressImageDXT1(uint32_t width, uint32_t height,
                              SDL_RWops* inputStream, uint8_t* outputImage);
void BlockDecompressImageDXT1VitaSwizzled(uint32_t width, uint32_t height,
                                          SDL_RWops* inputStream,
                                          uint8_t* outputImage);
}  // namespace TexLoad
}  // namespace Impacto