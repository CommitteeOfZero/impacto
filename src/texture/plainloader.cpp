#include "plainloader.h"

namespace Impacto {
namespace TexLoad {

using namespace Impacto::Io;

// This is for a very simple format used for some graphics by various console
// releases
// TODO: big-endian (360/PS3)

enum PlainPixelMode : uint32_t {
  Plain_8Bit_Paletted = 8,
  Plain_32Bit_ARGB = 32,
  Plain_8Bit_Alpha1 = 8 | (1 << 16),
  // Literally the same format, no idea why they made a new version
  Plain_8Bit_Alpha2 = 8 | (2 << 16)
};

bool TextureIsPlain(Stream* stream) {
  stream->Seek(4, RW_SEEK_SET);
  uint32_t mode = ReadLE<uint32_t>(stream);
  bool result = (mode == Plain_8Bit_Paletted || mode == Plain_32Bit_ARGB ||
                 mode == Plain_8Bit_Alpha1 || mode == Plain_8Bit_Alpha2);
  stream->Seek(0, RW_SEEK_SET);
  return result;
}

bool TextureLoadPlain(Stream* stream, Texture* outTexture) {
  uint16_t width = ReadLE<uint16_t>(stream);
  uint16_t height = ReadLE<uint16_t>(stream);
  PlainPixelMode mode = (PlainPixelMode)ReadLE<uint32_t>(stream);

  switch (mode) {
    case Plain_8Bit_Paletted: {  // 8-bit paletted; palette is BGRA
      uint8_t palette[256 * 4];
      stream->Read(palette, 4 * 256);

      outTexture->Init(TexFmt_RGBA, width, height);

      uint8_t* inBuffer = (uint8_t*)malloc(width * height);
      stream->Read(inBuffer, width * height);

      uint8_t* reader = inBuffer;
      for (size_t i = 0; i < outTexture->Buffer.size(); i += 4) {
        uint8_t colorIdx = *reader;
        uint8_t* color = palette + 4 * colorIdx;

        outTexture->Buffer[i + 0] = color[2];
        outTexture->Buffer[i + 1] = color[1];
        outTexture->Buffer[i + 2] = color[0];
        outTexture->Buffer[i + 3] = color[3];

        reader++;
      }

      free(inBuffer);
      return true;
    }

    case Plain_32Bit_ARGB: {  // 32-bit ARGB
      outTexture->Init(TexFmt_RGBA, width, height);

      uint8_t* inBuffer = (uint8_t*)malloc(outTexture->Buffer.size());
      stream->Read(inBuffer, outTexture->Buffer.size());

      for (size_t i = 0; i < outTexture->Buffer.size(); i += 4) {
        outTexture->Buffer[i + 0] = inBuffer[i + 1];
        outTexture->Buffer[i + 1] = inBuffer[i + 2];
        outTexture->Buffer[i + 2] = inBuffer[i + 3];
        outTexture->Buffer[i + 3] = inBuffer[i + 0];
      }

      free(inBuffer);
      return true;
    }

    case Plain_8Bit_Alpha1:
    case Plain_8Bit_Alpha2: {  // No palette, alpha channel only
      // TODO alpha textures
      outTexture->Init(TexFmt_RGBA, width, height);
      std::ranges::fill(outTexture->Buffer, 0xFF);

      uint8_t* inBuffer = (uint8_t*)malloc(width * height);
      stream->Read(inBuffer, width * height);

      uint8_t* reader = inBuffer;
      for (size_t i = 0; i < outTexture->Buffer.size(); i += 4) {
        outTexture->Buffer[i + 3] = *reader;
        reader++;
      }

      free(inBuffer);
      return true;
    }

    default:
      return false;
  }
}

}  // namespace TexLoad
}  // namespace Impacto