//
// Heavily based on
// https://github.com/Benjamin-Dobell/s3tc-dxt-decompression
//
// -----------------------------------------------------------------------------
// Copyright (c) 2009 Benjamin Dobell, Glass Echidna
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// -----------------------------------------------------------------------------

#include "s3tc.h"
#include "gxtloader.h"

namespace Impacto {
namespace TexLoad {
void DecompressBlockDXT1(uint32_t startX, uint32_t startY, uint32_t imageWidth,
                         SDL_RWops* inputStream, uint8_t* outputImage) {
  uint16_t color0 = SDL_ReadLE16(inputStream);
  uint16_t color1 = SDL_ReadLE16(inputStream);

  uint32_t temp;

  temp = (color0 >> 11) * 255 + 16;
  uint8_t r0 = (uint8_t)((temp / 32 + temp) / 32);
  temp = ((color0 & 0x07E0) >> 5) * 255 + 32;
  uint8_t g0 = (uint8_t)((temp / 64 + temp) / 64);
  temp = (color0 & 0x001F) * 255 + 16;
  uint8_t b0 = (uint8_t)((temp / 32 + temp) / 32);

  temp = (color1 >> 11) * 255 + 16;
  uint8_t r1 = (uint8_t)((temp / 32 + temp) / 32);
  temp = ((color1 & 0x07E0) >> 5) * 255 + 32;
  uint8_t g1 = (uint8_t)((temp / 64 + temp) / 64);
  temp = (color1 & 0x001F) * 255 + 16;
  uint8_t b1 = (uint8_t)((temp / 32 + temp) / 32);

  uint32_t code = SDL_ReadLE32(inputStream);

  uint8_t r, g, b;

  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < 4; i++) {
      uint32_t x = startX + i, y = startY + j;

      if (x >= imageWidth) continue;

      uint8_t positionCode = (code >> 2 * (4 * j + i)) & 0x03;

      switch (positionCode) {
        case 0:
          r = r0, g = g0, b = b0;
          break;
        case 1:
          r = r1, g = g1, b = b1;
          break;
        case 2:
          if (color0 > color1) {
            r = (2 * r0 + r1) / 3;
            g = (2 * g0 + g1) / 3;
            b = (2 * b0 + b1) / 3;
          } else {
            r = (r0 + 2 * r1) / 3;
            g = (g0 + 2 * g1) / 3;
            b = (b0 + 2 * b1) / 3;
          }
          break;
        case 3:
          if (color0 > color1) {
            r = (r0 + r1) / 2, g = (g0 + g1) / 2, b = (b0 + b1) / 2;
          } else {
            r = g = b = 0;
          }
          break;
      }

      outputImage[(x + imageWidth * y) * 3 + 0] = r;
      outputImage[(x + imageWidth * y) * 3 + 1] = g;
      outputImage[(x + imageWidth * y) * 3 + 2] = b;
    }
  }
}

void BlockDecompressImageDXT1(uint32_t width, uint32_t height,
                              SDL_RWops* inputStream, uint8_t* outputImage) {
  uint32_t blockCountX = (width + 3) / 4;
  uint32_t blockCountY = (height + 3) / 4;
  uint32_t blockWidth = (width < 4) ? width : 4;
  uint32_t blockHeight = (height < 4) ? height : 4;

  for (uint32_t j = 0; j < blockCountY; j++) {
    for (uint32_t i = 0; i < blockCountX; i++) {
      DecompressBlockDXT1(i * 4, j * 4, width, inputStream, outputImage);
    }
  }
}

void BlockDecompressImageDXT1VitaSwizzled(uint32_t width, uint32_t height,
                                          SDL_RWops* inputStream,
                                          uint8_t* outputImage) {
  uint32_t blockCountX = (width + 3) / 4;
  uint32_t blockCountY = (height + 3) / 4;
  uint32_t blockWidth = (width < 4) ? width : 4;
  uint32_t blockHeight = (height < 4) ? height : 4;

  for (uint32_t j = 0; j < blockCountY; j++) {
    for (uint32_t i = 0; i < blockCountX; i++) {
      int x = i, y = j;
      VitaUnswizzle(&x, &y, blockCountX, blockCountY);
      DecompressBlockDXT1(x * 4, y * 4, width, inputStream, outputImage);
    }
  }
}
}  // namespace TexLoad
}  // namespace Impacto