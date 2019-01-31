#include "gxtloader.h"

#include "../log.h"
#include "../util.h"

#include "s3tc.h"
#include "bntxloader.h"
#include "bcdecode.h"

using namespace Impacto::Io;

namespace Impacto {

namespace TexLoad {

int DIV_ROUND_UP(int n, int d) { return (n + d - 1) / d; }

int round_up(int x, int y) { return ((x - 1) | (y - 1)) + 1; }

int getAddrBlockLinear(int x, int y, int width, int bpp, int base_address,
                       int block_height) {
  int image_width_in_gobs = DIV_ROUND_UP(width * bpp, 64);

  int GOB_address =
      (base_address +
       (y / (8 * block_height)) * 512 * block_height * image_width_in_gobs +
       (x * bpp / 64) * 512 * block_height +
       (y % (8 * block_height) / 8) * 512);

  x *= bpp;

  int Address = (GOB_address + ((x % 64) / 32) * 256 + ((y % 8) / 2) * 64 +
                 ((x % 32) / 16) * 32 + (y % 2) * 16 + (x % 16));

  return Address;
}

uint8_t* _swizzle(int width, int height, int blkWidth, int blkHeight, int bpp,
                  int tileMode, int alignment, int size_range, uint8_t* data,
                  int toSwizzle) {
  int block_height = 1 << size_range;

  width = DIV_ROUND_UP(width, blkWidth);
  height = DIV_ROUND_UP(height, blkHeight);
  int pitch, surfSize;
  if (tileMode == 0) {
    pitch = round_up(width * bpp, 32);
    surfSize = round_up(pitch * height, alignment);
  } else {
    pitch = round_up(width * bpp, 64);
    surfSize = round_up(pitch * round_up(height, block_height * 8), alignment);
  }
  auto result = (uint8_t*)malloc(surfSize);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int pos = 0;
      if (tileMode == 0)
        pos = y * pitch + x * bpp;

      else {
        pos = getAddrBlockLinear(x, y, width, bpp, 0, block_height);

        int pos_ = (y * width + x) * bpp;

        if (pos + bpp <= surfSize) {
          if (toSwizzle) {
            for (int i = 0; i < bpp; i++) result[pos + i] = data[pos_ + i];
          }

          else {
            for (int i = 0; i < bpp; i++) result[pos_ + i] = data[pos + i];
          }
        }
      }
    }
  }
  return result;
}

static uint64_t const magic = 0x424E5458;

int BPPbyFormat(TextureFormatType format) {
  switch (format) {
    case R5G6B5:
      break;
    case R8G8:
      break;
    case R16:
      break;
    case R8G8B8A8:
      return 4;
      break;
    case R11G11B10:
      break;
    case R32:
      break;
    case BC1:
      return 8;
      break;
    case BC2:
      return 16;
      break;
    case BC3:
      return 16;
      break;
    case BC4:
      return 8;
      break;
    case BC5:
      return 16;
      break;
    case BC6:
      return 16;
      break;
    case BC7:
      return 8;
      break;
    case ASTC4x4:
      break;
    case ASTC5x4:
      break;
    case ASTC5x5:
      break;
    case ASTC6x5:
      break;
    case ASTC6x6:
      break;
    case ASTC8x5:
      break;
    case ASTC8x6:
      break;
    case ASTC8x8:
      break;
    case ASTC10x5:
      break;
    case ASTC10x6:
      break;
    case ASTC10x8:
      break;
    case ASTC10x10:
      break;
    case ASTC12x10:
      break;
    case ASTC12x12:
      break;
    default:;
  }
}

TextureNX::TextureNX() {}

uint32_t TextureNX::Pow2RoundUp(uint32_t Value) {
  Value--;

  Value |= (Value >> 1);
  Value |= (Value >> 2);
  Value |= (Value >> 4);
  Value |= (Value >> 8);
  Value |= (Value >> 16);

  return ++Value;
}

uint32_t TextureNX::GetPow2HeightInTexels() {
  uint32_t Pow2Height = Pow2RoundUp(Height);

  switch (FormatType) {
    case BC1:
    case BC2:
    case BC3:
    case BC4:
    case BC5:
    case ASTC4x4:
    case ASTC5x4:
      return (Pow2Height + 3) / 4;

    case ASTC5x5:
    case ASTC6x5:
    case ASTC8x5:
      return (Pow2Height + 4) / 5;

    case ASTC6x6:
    case ASTC8x6:
    case ASTC10x6:
      return (Pow2Height + 5) / 6;

    case ASTC8x8:
    case ASTC10x8:
      return (Pow2Height + 7) / 8;

    case ASTC10x10:
    case ASTC12x10:
      return (Pow2Height + 9) / 10;

    case ASTC12x12:
      return (Pow2Height + 11) / 12;
  }

  return Pow2Height;
}

uint32_t TextureNX::GetBytesPerTexel() {
  switch (FormatType) {
    case R5G6B5:
    case R8G8:
    case R16:
      return 2;

    case R8G8B8A8:
    case R11G11B10:
    case R32:
      return 4;

    case BC1:
    case BC4:
      return 8;

    case BC2:
    case BC3:
    case BC5:
    case ASTC4x4:
    case ASTC5x4:
    case ASTC5x5:
    case ASTC6x5:
    case ASTC6x6:
    case ASTC8x5:
    case ASTC8x6:
    case ASTC8x8:
    case ASTC10x5:
    case ASTC10x6:
    case ASTC10x8:
    case ASTC10x10:
    case ASTC12x10:
    case ASTC12x12:
      return 16;
  }
}

uint32_t TextureNX::GetBlockHeight() { return 1 << BlockHeightLog2; }

uint8_t* BCnDecompress(uint8_t* dataBuff, TextureNX element, int n) {
  int s = element.Height * element.Width * 4;
  uint8_t* dst = (uint8_t*)malloc(s);

  BcnDecode(dst, s, dataBuff,
            element.GetBytesPerTexel() * element.Height * element.Width,
            element.Width, element.Height, n, BcnDecoderFormatRGBA, 0);

  return dst;
}

bool TextureLoadBNTX(InputStream* stream, Texture* outTexture) {
  // Read metadata

  if (ReadBE<uint32_t>(stream) != magic) {
    stream->Seek(0, RW_SEEK_SET);
    return false;
  }
  bool result = false;
  ImpLogSlow(LL_Debug, LC_TextureLoad, "Loading BNTX texture\n");

  stream->Seek(4, RW_SEEK_CUR);
  // uint64_t BnTxSignature = ReadLE<uint64_t>(stream);
  uint32_t DataLength = ReadLE<uint32_t>(stream);
  uint16_t ByteOrderMark = ReadLE<uint16_t>(stream);
  uint16_t FormatRevision = ReadLE<uint16_t>(stream);
  uint32_t NameAddress = ReadLE<uint32_t>(stream);
  uint32_t StringsAddress = ReadLE<uint32_t>(stream);
  uint32_t RelocAddress = ReadLE<uint32_t>(stream);
  uint32_t FileLength = ReadLE<uint32_t>(stream);
  uint32_t NXSignature = ReadLE<uint32_t>(stream);

  uint32_t TexturesCount = ReadLE<uint32_t>(stream);
  ImpLog(LL_Debug, LC_General, "%d\n", TexturesCount);
  uint64_t InfoPtrsAddress = ReadLE<uint64_t>(stream);
  uint64_t DataBlkAddress = ReadLE<uint64_t>(stream);
  uint64_t DictAddress = ReadLE<uint64_t>(stream);
  uint32_t StrDictLength = ReadLE<uint32_t>(stream);

  for (int Index = 0; Index < TexturesCount; Index++) {
    stream->Seek(InfoPtrsAddress + Index * 8, 0);
    uint64_t offset = ReadLE<uint64_t>(stream);

    stream->Seek(offset, 0);

    uint32_t BRTISignature = ReadLE<uint32_t>(stream);

    // CheckSignature(L"BRTI", BRTISignature);

    uint32_t BRTILength0 = ReadLE<uint32_t>(stream);

    uint64_t BRTILength1 = ReadLE<uint64_t>(stream);

    uint8_t TileMode = ReadLE<uint8_t>(stream);

    uint8_t Dimensions = ReadLE<uint8_t>(stream);

    uint16_t unknown = ReadLE<uint16_t>(stream);
    uint16_t SwizzleSize = ReadLE<uint16_t>(stream);
    uint16_t MipmapCount = ReadLE<uint16_t>(stream);
    uint16_t MultiSampleCount = ReadLE<uint16_t>(stream);
    uint16_t Reversed1A = ReadLE<uint16_t>(stream);

    uint32_t Format = ReadLE<uint32_t>(stream);

    uint32_t AccessFlags = ReadLE<uint32_t>(stream);

    uint32_t Width = ReadLE<uint32_t>(stream);
    uint32_t Height = ReadLE<uint32_t>(stream);
    uint32_t Depth = ReadLE<uint32_t>(stream);
    uint32_t ArrayCount = ReadLE<uint32_t>(stream);
    uint32_t BlockHeightLog2 = ReadLE<uint32_t>(stream);
    uint32_t Reserved38 = ReadLE<uint32_t>(stream);
    uint32_t Reserved3C = ReadLE<uint32_t>(stream);
    uint32_t Reserved40 = ReadLE<uint32_t>(stream);
    uint32_t Reserved44 = ReadLE<uint32_t>(stream);
    uint32_t Reserved48 = ReadLE<uint32_t>(stream);
    uint32_t Reserved4C = ReadLE<uint32_t>(stream);
    uint32_t DataLength = ReadLE<uint32_t>(stream);
    uint32_t Alignment = ReadLE<uint32_t>(stream);
    uint32_t ChannelTypes = ReadLE<uint32_t>(stream);
    uint32_t TextureType2 = ReadLE<uint32_t>(stream);
    uint64_t NameAddress = ReadLE<uint64_t>(stream);
    uint64_t ParentAddress = ReadLE<uint64_t>(stream);
    uint64_t PtrsAddress = ReadLE<uint64_t>(stream);

    stream->Seek(NameAddress, 0);

    std::string Name;

    std::vector<uint64_t> MipOffsets(MipmapCount);

    stream->Seek(PtrsAddress, 0);

    uint64_t BaseOffset = ReadLE<uint64_t>(stream);
    /*
    for (int Mip = 1; Mip < MipmapCount; Mip++) {
      uint64_t mipOffset = ReadLE<uint64_t>(stream);

      MipOffsets.push_back(mipOffset - BaseOffset);
      ImpLog(LL_Debug, LC_General, "%d\n", mipOffset);
    }*/

    stream->Seek(BaseOffset, RW_SEEK_SET);

    uint8_t* dataBuff = (uint8_t*)malloc(DataLength);
    stream->Read(dataBuff, DataLength);

    TextureNX element = TextureNX();
    element.Name = Name;
    element.Width = Width;
    element.Height = Height;
    element.ArrayCount = ArrayCount;
    element.BlockHeightLog2 = BlockHeightLog2;
    element.MipmapCount = MipmapCount;
    element.MipOffsets = MipOffsets;
    element.Channel0Type = (ChannelType)((ChannelTypes >> 0) & 0xff);
    element.Channel1Type = (ChannelType)((ChannelTypes >> 8) & 0xff);
    element.Channel2Type = (ChannelType)((ChannelTypes >> 16) & 0xff);
    element.Channel3Type = (ChannelType)((ChannelTypes >> 24) & 0xff);
    element.Type = (TextureType)(TextureType2);
    element.FormatType = (TextureFormatType)((Format >> 8) & 0xff);
    element.FormatVariant = (TextureFormatVar)((Format >> 0) & 0xff);
    element.TilingMode = TileMode;
    element.Alignment = Alignment;

    if (element.MipmapCount >= 1) {
      if (element.TilingMode) {
        int bpp = BPPbyFormat(element.FormatType);
        int blk_width = 4;
        int blk_height = 4;
        auto unswizzled = _swizzle(element.Width, element.Height, blk_width,
                                   blk_height, bpp, 1, element.Alignment,
                                   element.BlockHeightLog2, dataBuff, 0);
        free(dataBuff);
        dataBuff = unswizzled;
      }

      switch (element.FormatType) {
        case BC1: {
          void* oldBuff = dataBuff;
          dataBuff = BCnDecompress(dataBuff, element, 1);
          free(oldBuff);
        } break;
        case BC2: {
          void* oldBuff = dataBuff;
          dataBuff = BCnDecompress(dataBuff, element, 2);
          free(oldBuff);
        } break;

        case BC3: {
          void* oldBuff = dataBuff;
          dataBuff = BCnDecompress(dataBuff, element, 3);
          free(oldBuff);
        } break;

        case BC5: {
          void* oldBuff = dataBuff;
          dataBuff = BCnDecompress(dataBuff, element, 5);
          free(oldBuff);
        } break;

        case TextureFormatType::R8G8B8A8:

          break;
      }

      outTexture->Buffer = dataBuff;
      outTexture->BufferSize = element.Height * element.Width * 4;
      outTexture->Width = element.Width;
      outTexture->Height = element.Height;
      outTexture->Format = TexFmt_RGBA;
      result = true;
      break;
    }
  }

  // Get result

  return result;
}  // namespace TexLoad

static bool _registered = Texture::AddTextureLoader(&TextureLoadBNTX);

}  // namespace TexLoad
}  // namespace Impacto