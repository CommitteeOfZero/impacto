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

using namespace Impacto::Io;

void DecompressBlockDXT1(uint32_t startX, uint32_t startY, uint32_t imageWidth,
                         InputStream* inputStream, uint8_t* outputImage) {
  uint16_t color0 = ReadLE<uint16_t>(inputStream);
  uint16_t color1 = ReadLE<uint16_t>(inputStream);

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

  uint32_t code = ReadLE<uint32_t>(inputStream);

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
                              InputStream* inputStream, uint8_t* outputImage) {
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
                                          InputStream* inputStream,
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

// TODO: Kai's eyes are broken, a problem in here might be why
void DecompressBlockDXT5(uint32_t startX, uint32_t startY, uint32_t imageWidth,
                         InputStream* inputStream, uint8_t* outputImage) {
  uint8_t alpha0 = ReadU8(inputStream);
  uint8_t alpha1 = ReadU8(inputStream);

  uint8_t alphaBits[6];
  inputStream->Read(alphaBits, 6);

  uint32_t alphaCode1 = alphaBits[2] | (alphaBits[3] << 8) |
                        (alphaBits[4] << 16) | (alphaBits[5] << 24);
  uint16_t alphaCode2 = alphaBits[0] | (alphaBits[1] << 8);

  uint16_t color0 = ReadLE<uint16_t>(inputStream);
  uint16_t color1 = ReadLE<uint16_t>(inputStream);

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

  uint32_t code = ReadLE<uint32_t>(inputStream);

  uint8_t r, g, b, a;

  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < 4; i++) {
      uint32_t x = startX + i, y = startY + j;

      if (x >= imageWidth) continue;

      int alphaCodeIndex = 3 * (4 * j + i);
      int alphaCode;

      if (alphaCodeIndex <= 12) {
        alphaCode = (alphaCode2 >> alphaCodeIndex) & 0x07;
      } else if (alphaCodeIndex == 15) {
        alphaCode = (alphaCode2 >> 15) | ((alphaCode1 << 1) & 0x06);
      } else {
        alphaCode = (alphaCode1 >> (alphaCodeIndex - 16)) & 0x07;
      }

      if (alphaCode == 0) {
        a = alpha0;
      } else if (alphaCode == 1) {
        a = alpha1;
      } else {
        if (alpha0 > alpha1) {
          a = ((8 - alphaCode) * alpha0 + (alphaCode - 1) * alpha1) / 7;
        } else {
          if (alphaCode == 6)
            a = 0;
          else if (alphaCode == 7)
            a = 255;
          else
            a = ((6 - alphaCode) * alpha0 + (alphaCode - 1) * alpha1) / 5;
        }
      }

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

      outputImage[(x + imageWidth * y) * 4 + 0] = r;
      outputImage[(x + imageWidth * y) * 4 + 1] = g;
      outputImage[(x + imageWidth * y) * 4 + 2] = b;
      outputImage[(x + imageWidth * y) * 4 + 3] = a;
    }
  }
}

void BlockDecompressImageDXT5(uint32_t width, uint32_t height,
                              InputStream* inputStream, uint8_t* outputImage) {
  uint32_t blockCountX = (width + 3) / 4;
  uint32_t blockCountY = (height + 3) / 4;
  uint32_t blockWidth = (width < 4) ? width : 4;
  uint32_t blockHeight = (height < 4) ? height : 4;

  for (uint32_t j = 0; j < blockCountY; j++) {
    for (uint32_t i = 0; i < blockCountX; i++) {
      DecompressBlockDXT5(i * 4, j * 4, width, inputStream, outputImage);
    }
  }
}

void BlockDecompressImageDXT5VitaSwizzled(uint32_t width, uint32_t height,
                                          InputStream* inputStream,
                                          uint8_t* outputImage) {
  uint32_t blockCountX = (width + 3) / 4;
  uint32_t blockCountY = (height + 3) / 4;
  uint32_t blockWidth = (width < 4) ? width : 4;
  uint32_t blockHeight = (height < 4) ? height : 4;

  for (uint32_t j = 0; j < blockCountY; j++) {
    for (uint32_t i = 0; i < blockCountX; i++) {
      int x = i, y = j;
      VitaUnswizzle(&x, &y, blockCountX, blockCountY);
      DecompressBlockDXT5(x * 4, y * 4, width, inputStream, outputImage);
    }
  }
}
}  // namespace TexLoad
}  // namespace Impacto