#pragma once

#include "../io/stream.h"

namespace Impacto {
namespace TexLoad {
// Note: Unlike Benjamin's original implementation, these decompress to 24-bit
// RGB, not 32-bit 1RGB
void DecompressBlockDXT1(uint32_t startX, uint32_t startY, uint32_t imageWidth,
                         Io::Stream* stream, uint8_t* outputImage);
void BlockDecompressImageDXT1(uint32_t width, uint32_t height,
                              Io::Stream* stream, uint8_t* outputImage);
void BlockDecompressImageDXT1VitaSwizzled(uint32_t width, uint32_t height,
                                          Io::Stream* stream,
                                          uint8_t* outputImage);

void DecompressBlockDXT5(uint32_t startX, uint32_t startY, uint32_t imageWidth,
                         Io::Stream* stream, uint8_t* outputImage);
void BlockDecompressImageDXT5(uint32_t width, uint32_t height,
                              Io::Stream* stream, uint8_t* outputImage);
void BlockDecompressImageDXT5VitaSwizzled(uint32_t width, uint32_t height,
                                          Io::Stream* stream,
                                          uint8_t* outputImage);
}  // namespace TexLoad
}  // namespace Impacto