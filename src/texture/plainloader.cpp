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

bool TextureIsPlain(InputStream* stream) {
  stream->Seek(4, RW_SEEK_SET);
  uint32_t mode = ReadLE<uint32_t>(stream);
  bool result = (mode == Plain_8Bit_Paletted || mode == Plain_32Bit_ARGB ||
                 mode == Plain_8Bit_Alpha1 || mode == Plain_8Bit_Alpha2);
  stream->Seek(0, RW_SEEK_SET);
  return result;
}

bool TextureLoadPlain(InputStream* stream, Texture* outTexture) {
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
      uint8_t* writer = outTexture->Buffer;

      for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
          uint8_t colorIdx = *reader;
          uint8_t* color = palette + 4 * colorIdx;

          writer[0] = color[2];
          writer[1] = color[1];
          writer[2] = color[0];
          writer[3] = color[3];

          reader++;
          writer += 4;
        }
      }

      free(inBuffer);
      return true;
    }

    case Plain_32Bit_ARGB: {  // 32-bit ARGB
      outTexture->Init(TexFmt_RGBA, width, height);

      uint8_t* inBuffer = (uint8_t*)malloc(outTexture->BufferSize);
      stream->Read(inBuffer, outTexture->BufferSize);

      uint8_t* reader = inBuffer;
      uint8_t* writer = outTexture->Buffer;

      for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
          writer[0] = reader[1];
          writer[1] = reader[2];
          writer[2] = reader[3];
          writer[3] = reader[0];

          reader += 4;
          writer += 4;
        }
      }

      free(inBuffer);
      return true;
    }

    case Plain_8Bit_Alpha1:
    case Plain_8Bit_Alpha2: {  // No palette, alpha channel only
      // TODO alpha textures
      outTexture->Init(TexFmt_RGBA, width, height);
      memset(outTexture->Buffer, 0xFF, outTexture->BufferSize);

      uint8_t* inBuffer = (uint8_t*)malloc(width * height);
      stream->Read(inBuffer, width * height);

      uint8_t* reader = inBuffer;
      uint8_t* writer = outTexture->Buffer + 3;

      for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
          *writer = *reader;

          reader++;
          writer += 4;
        }
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