#include "gxtloader.h"

#include "../log.h"
#include "../util.h"

#include "s3tc.h"

using namespace Impacto::Io;

namespace Impacto {

namespace Gxm {
enum SceGxmTextureType : uint32_t {
  Swizzled = 0x00000000,
  Cube = 0x40000000,
  Linear = 0x60000000,
  Tiled = 0x80000000,
  SwizzledArbitrary = 0xA0000000,
  LinearStrided = 0xC0000000,
  CubeArbitrary = 0xE0000000
};

enum SceGxmTextureBaseFormat : uint32_t {
  U8 = 0x00000000,
  S8 = 0x01000000,
  U4U4U4U4 = 0x02000000,
  U8U3U3U2 = 0x03000000,
  U1U5U5U5 = 0x04000000,
  U5U6U5 = 0x05000000,
  S5S5U6 = 0x06000000,
  U8U8 = 0x07000000,
  S8S8 = 0x08000000,
  U16 = 0x09000000,
  S16 = 0x0A000000,
  F16 = 0x0B000000,
  U8U8U8U8 = 0x0C000000,
  S8S8S8S8 = 0x0D000000,
  U2U10U10U10 = 0x0E000000,
  U16U16 = 0x0F000000,
  S16S16 = 0x10000000,
  F16F16 = 0x11000000,
  F32 = 0x12000000,
  F32M = 0x13000000,
  X8S8S8U8 = 0x14000000,
  X8U24 = 0x15000000,
  U32 = 0x17000000,
  S32 = 0x18000000,
  SE5M9M9M9 = 0x19000000,
  F11F11F10 = 0x1A000000,
  F16F16F16F16 = 0x1B000000,
  U16U16U16U16 = 0x1C000000,
  S16S16S16S16 = 0x1D000000,
  F32F32 = 0x1E000000,
  U32U32 = 0x1F000000,
  PVRT2BPP = 0x80000000,
  PVRT4BPP = 0x81000000,
  PVRTII2BPP = 0x82000000,
  PVRTII4BPP = 0x83000000,
  UBC1 = 0x85000000,
  UBC2 = 0x86000000,
  UBC3 = 0x87000000,
  YUV420P2 = 0x90000000,
  YUV420P3 = 0x91000000,
  YUV422 = 0x92000000,
  P4 = 0x94000000,
  P8 = 0x95000000,
  U8U8U8 = 0x98000000,
  S8S8S8 = 0x99000000,
  U2F10F10F10 = 0x9A000000
};

enum SceGxmTextureSwizzle4Mode : uint32_t {
  ABGR = 0x00000000,
  ARGB = 0x00001000,
  RGBA = 0x00002000,
  BGRA = 0x00003000,
  _1BGR = 0x00004000,
  _1RGB = 0x00005000,
  RGB1 = 0x00006000,
  BGR1 = 0x00007000
};

enum SceGxmTextureSwizzle3Mode : uint32_t { BGR = 0x0000, RGB = 0x1000 };
}  // namespace Gxm

struct SubtextureHeader {
  uint32_t Offset;
  uint32_t Size;
  uint32_t PaletteIdx;
  uint32_t Flags;  // or unused
  uint32_t PixelOrder;
  uint32_t Format;

  uint16_t Width;
  uint16_t Height;
  uint16_t MipmapCount;
};

#define TexfmtCheck(condition)                                    \
  if (!((condition))) {                                           \
    ImpLog(LogLevel::Error, LogChannel::TextureLoad,              \
           "Unsupported texture format 0x{:08x}\n", stx->Format); \
    return false;                                                 \
  }                                                               \
  (void)0

// Vita unswizzle
//
// Thanks @xdanieldzd, @FireyFly and ryg
// https://github.com/xdanieldzd/Scarlet/blob/d8aabf430307d35a81b131e40bb3c9a4828bdd7b/Scarlet/Drawing/ImageBinary.cs
// http://xen.firefly.nu/up/rearrange.c.html
// https://fgiesen.wordpress.com/2009/12/13/decoding-morton-codes/

/* clang-format off */

// Inverse of Part1By1 - "delete" all odd-indexed bits
uint32_t Compact1By1(uint32_t x) {
  x &= 0x55555555;                  // x = -f-e -d-c -b-a -9-8 -7-6 -5-4 -3-2 -1-0
  x = (x ^ (x >>  1)) & 0x33333333; // x = --fe --dc --ba --98 --76 --54 --32 --10
  x = (x ^ (x >>  2)) & 0x0f0f0f0f; // x = ---- fedc ---- ba98 ---- 7654 ---- 3210
  x = (x ^ (x >>  4)) & 0x00ff00ff; // x = ---- ---- fedc ba98 ---- ---- 7654 3210
  x = (x ^ (x >>  8)) & 0x0000ffff; // x = ---- ---- ---- ---- fedc ba98 7654 3210
  return x;
}
uint32_t DecodeMorton2X(uint32_t code) { return Compact1By1(code >> 0); }
uint32_t DecodeMorton2Y(uint32_t code) { return Compact1By1(code >> 1); }

namespace TexLoad {

void VitaUnswizzle(int* x, int* y, int width, int height) {
  // TODO: verify this is even sensible
  int origX = *x, origY = *y;
  if (width == 0) width = 16;
  if (height == 0) height = 16;

  int i = (origY * width) + origX;
  int min = width < height ? width : height;
  int k = Uint32Log2(min);

  if (height < width) {
    // XXXyxyxyx -> XXXxxxyyy
    int j = i >> (2 * k) << (2 * k)
        | (DecodeMorton2Y(i) & (min - 1)) << k
        | (DecodeMorton2X(i) & (min - 1)) << 0;
    *x = j / height;
    *y = j % height;
  }
  else {
    // YYYyxyxyx -> YYYyyyxxx
    int j = i >> (2 * k) << (2 * k)
        | (DecodeMorton2X(i) & (min - 1)) << k
        | (DecodeMorton2Y(i) & (min - 1)) << 0;
    *x = j % width;
    *y = j / width;
  }
}

/* clang-format on */

bool GXTLoadSubtexture(Stream* stream, Texture* outTexture,
                       SubtextureHeader* stx, uint8_t* p4Palettes,
                       uint8_t* p8Palettes, uint32_t p4count) {
  memset(outTexture, 0, sizeof(*outTexture));
  stream->Seek(stx->Offset, RW_SEEK_SET);
  uint32_t baseFormat = (stx->Format & 0xFF000000U);
  uint32_t channelOrder = (stx->Format & 0x0000FFFFU);

  if (stx->PixelOrder != Gxm::Swizzled && stx->PixelOrder != Gxm::Linear) {
    ImpLog(LogLevel::Error, LogChannel::TextureLoad,
           "Unsupported pixel order 0x{:08x}\n", stx->PixelOrder);
    return false;
  }

  switch (baseFormat) {
    // 24bpp RGB
    case Gxm::U8U8U8: {
      TexfmtCheck(channelOrder == Gxm::BGR || channelOrder == Gxm::RGB);

      outTexture->Init(TexFmt_RGB, stx->Width, stx->Height);

      if (channelOrder == Gxm::BGR && stx->PixelOrder == Gxm::Linear) {
        stream->Read(outTexture->Buffer, outTexture->BufferSize);
      } else {
        uint8_t* inBuffer = (uint8_t*)malloc(outTexture->BufferSize);
        uint8_t* reader = inBuffer;
        stream->Read(inBuffer, outTexture->BufferSize);

        for (int y = 0; y < stx->Height; y++) {
          for (int x = 0; x < stx->Width; x++) {
            int outX = x, outY = y;
            if (stx->PixelOrder == Gxm::Swizzled) {
              VitaUnswizzle(&outX, &outY, stx->Width, stx->Height);
            }

            int px = (outX + stx->Width * outY) * 3;

            if (channelOrder == Gxm::RGB) {
              outTexture->Buffer[px + 2] = *reader++;
              outTexture->Buffer[px + 1] = *reader++;
              outTexture->Buffer[px + 0] = *reader++;
            } else if (channelOrder == Gxm::BGR) {
              memcpy(outTexture->Buffer + px, reader, 3);
              reader += 3;
            }
          }
        }

        free(inBuffer);
      }
      break;
    }

    // 32bpp RGBA
    case Gxm::U8U8U8U8: {
      TexfmtCheck(channelOrder == Gxm::ARGB);

      outTexture->Init(TexFmt_RGBA, stx->Width, stx->Height);

      uint8_t* inBuffer = (uint8_t*)malloc(outTexture->BufferSize);
      uint8_t* reader = inBuffer;
      stream->Read(inBuffer, outTexture->BufferSize);

      for (int y = 0; y < stx->Height; y++) {
        for (int x = 0; x < stx->Width; x++) {
          int outX = x, outY = y;
          if (stx->PixelOrder == Gxm::Swizzled) {
            VitaUnswizzle(&outX, &outY, stx->Width, stx->Height);
          }

          int px = (outX + stx->Width * outY) * 4;

          outTexture->Buffer[px + 2] = *reader++;
          outTexture->Buffer[px + 1] = *reader++;
          outTexture->Buffer[px + 0] = *reader++;
          outTexture->Buffer[px + 3] = *reader++;
        }
      }

      free(inBuffer);
      break;
    }

    // 256 color paletted
    case Gxm::P8: {
      TexfmtCheck(channelOrder == Gxm::_1RGB || channelOrder == Gxm::ARGB);

      // PaletteIdx is into *all* palettes (P8s following all P4s), we have P4
      // and P8 separate
      uint8_t* palette = p8Palettes + 4 * 256 * (stx->PaletteIdx - p4count);

      int bytesPerPixel;
      if (channelOrder == Gxm::_1RGB) {
        outTexture->Init(TexFmt_RGB, stx->Width, stx->Height);
        bytesPerPixel = 3;
      } else if (channelOrder == Gxm::ARGB) {
        outTexture->Init(TexFmt_RGBA, stx->Width, stx->Height);
        bytesPerPixel = 4;
      }

      uint8_t* inBuffer = (uint8_t*)malloc(stx->Width * stx->Height);
      uint8_t* reader = inBuffer;
      stream->Read(inBuffer, stx->Width * stx->Height);

      for (int y = 0; y < stx->Height; y++) {
        for (int x = 0; x < stx->Width; x++) {
          int outX = x, outY = y;
          if (stx->PixelOrder == Gxm::Swizzled) {
            VitaUnswizzle(&outX, &outY, stx->Width, stx->Height);
          }

          uint8_t colorIdx = *reader++;
          uint8_t* color = palette + 4 * colorIdx;

          int px = (outX + stx->Width * outY) * bytesPerPixel;

          outTexture->Buffer[px + 2] = color[0];
          outTexture->Buffer[px + 1] = color[1];
          outTexture->Buffer[px + 0] = color[2];
          if (bytesPerPixel == 4) {
            outTexture->Buffer[px + 3] = color[3];
          }
        }
      }

      free(inBuffer);
      break;
    }

    // DXT1, no alpha
    case Gxm::UBC1: {
      outTexture->Init(TexFmt_RGBA, stx->Width, stx->Height);

      if (stx->PixelOrder == Gxm::Swizzled) {
        BlockDecompressImageDXT1VitaSwizzled(stx->Width, stx->Height, stream,
                                             outTexture->Buffer);
      } else {
        BlockDecompressImageDXT1(stx->Width, stx->Height, stream,
                                 outTexture->Buffer);
      }
      break;
    }

    // DXT5
    case Gxm::UBC3: {
      outTexture->Init(TexFmt_RGBA, stx->Width, stx->Height);

      if (stx->PixelOrder == Gxm::Swizzled) {
        BlockDecompressImageDXT5VitaSwizzled(stx->Width, stx->Height, stream,
                                             outTexture->Buffer);
      } else {
        BlockDecompressImageDXT5(stx->Width, stx->Height, stream,
                                 outTexture->Buffer);
      }
      break;
    }

    // 8-bit grayscale
    case Gxm::U8: {
      outTexture->Init(TexFmt_U8, stx->Width, stx->Height);

      if (stx->PixelOrder == Gxm::Swizzled) {
        uint8_t* inBuffer = (uint8_t*)malloc(outTexture->BufferSize);
        uint8_t* reader = inBuffer;
        stream->Read(inBuffer, outTexture->BufferSize);

        for (int y = 0; y < stx->Height; y++) {
          for (int x = 0; x < stx->Width; x++) {
            int outX = x, outY = y;
            VitaUnswizzle(&outX, &outY, stx->Width, stx->Height);

            uint8_t v = *reader++;

            outTexture->Buffer[(outX + stx->Width * outY)] = v;
          }
        }

        free(inBuffer);
      } else {
        stream->Read(outTexture->Buffer, outTexture->BufferSize);
      }
      break;
    }

    default: {
      TexfmtCheck(0);
    }
  }

  return true;
}

static uint32_t const magic = 0x47585400;

bool TextureLoadGXT(Stream* stream, Texture* outTexture) {
  // Read metadata

  if (ReadBE<uint32_t>(stream) != magic) {
    stream->Seek(0, RW_SEEK_SET);
    return false;
  }

  ImpLogSlow(LogLevel::Debug, LogChannel::TextureLoad, "Loading GXT texture\n");

  uint32_t version = ReadLE<uint32_t>(stream);
  (void)version;
  uint32_t subtextureCount = ReadLE<uint32_t>(stream);
  assert(subtextureCount == 1);
  uint32_t subtexturesOffset = ReadLE<uint32_t>(stream);
  (void)subtexturesOffset;
  uint32_t totalTexSize = ReadLE<uint32_t>(stream);
  (void)totalTexSize;
  uint32_t p4Count = ReadLE<uint32_t>(stream);
  uint32_t p8Count = ReadLE<uint32_t>(stream);
  // padding
  stream->Seek(4, RW_SEEK_CUR);

  ImpLogSlow(LogLevel::Debug, LogChannel::TextureLoad,
             "GXT version=0x{:08x}, subtextureCount=0x{:08x}, "
             "subtexturesOffset=0x{:08x}, totalTexSize=0x{:08x}, "
             "p4Count=0x{:08x}, p8Count=0x{:08x}\n",
             version, subtextureCount, subtexturesOffset, totalTexSize, p4Count,
             p8Count);

  SubtextureHeader stx;
  stx.Offset = ReadLE<uint32_t>(stream);
  stx.Size = ReadLE<uint32_t>(stream);
  stx.PaletteIdx = ReadLE<uint32_t>(stream);
  stx.Flags = ReadLE<uint32_t>(stream);
  stx.PixelOrder = ReadLE<uint32_t>(stream);
  stx.Format = ReadLE<uint32_t>(stream);

  stx.Width = ReadLE<uint16_t>(stream);
  stx.Height = ReadLE<uint16_t>(stream);
  stx.MipmapCount = ReadLE<uint16_t>(stream);
  assert(stx.MipmapCount == 1);
  // subtexture header padding
  stream->Seek(2, RW_SEEK_CUR);

  stream->Seek(stx.Size, RW_SEEK_CUR);

  ImpLogSlow(
      LogLevel::Debug, LogChannel::TextureLoad,
      "Subtexture Offset=0x{:08x}, Size=0x{:08x}, PaletteIdx=0x{:08x}, "
      "Flags=0x{:08x}, PixelOrder=0x{:08x}, Format=0x{:08x}, Width=0x{:08x}, "
      "Height=0x{:08x}, MipmapCount=0x{:08x}\n",
      stx.Offset, stx.Size, stx.PaletteIdx, stx.Flags, stx.PixelOrder,
      stx.Format, stx.Width, stx.Height, stx.MipmapCount);

  // Read palettes

  // 4bpp/8bpp => 32bpp palettes
  uint8_t* P4Palettes = NULL;
  uint8_t* P8Palettes = NULL;
  if (p4Count > 0) {
    P4Palettes = (uint8_t*)ImpStackAlloc(4 * 16 * p4Count);
    stream->Read(P4Palettes, 4 * 16 * p4Count);
  }
  if (p8Count > 0) {
    P8Palettes = (uint8_t*)ImpStackAlloc(4 * 256 * p8Count);
    stream->Read(P8Palettes, 4 * 256 * p8Count);
  }

  // Get result
  bool result = GXTLoadSubtexture(stream, outTexture, &stx, P4Palettes,
                                  P8Palettes, p4Count);

  if (P4Palettes) ImpStackFree(P4Palettes);
  if (P8Palettes) ImpStackFree(P8Palettes);

  return result;
}

static bool _registered = Texture::AddTextureLoader(&TextureLoadGXT);

}  // namespace TexLoad
}  // namespace Impacto