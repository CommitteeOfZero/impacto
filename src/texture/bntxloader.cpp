#include "../log.h"

#include "bntxloader.h"
#include "bcdecode.h"

using namespace Impacto::Io;

namespace Impacto {

namespace TexLoad {

int CountLsbZeros(int value) {
  int count = 0;

  while (((value >> count) & 1) == 0) {
    count++;
  }

  return count;
}

int Pow2RoundUp(int value) {
  value--;

  value |= (value >> 1);
  value |= (value >> 2);
  value |= (value >> 4);
  value |= (value >> 8);
  value |= (value >> 16);

  return ++value;
}

int DivRoundUp(int lhs, int rhs) { return (lhs + (rhs - 1)) / rhs; }

uint8_t* UnSwizzle(int width, int height, int blkWidth, int blkHeight, int bpp,
                   int blkHeightLog2, uint8_t* data) {
  width = DivRoundUp(width, blkWidth);
  height = DivRoundUp(height, blkHeight);

  blkHeight = 1 << blkHeightLog2;

  int bppShift = CountLsbZeros(bpp);
  int widthInGobs = DivRoundUp(width * bpp, 64);
  int pow2Height = Pow2RoundUp(height);

  while (blkHeight * 8 > pow2Height && blkHeight > 1) {
    blkHeight >>= 1;
  }

  int bhMask = (blkHeight * 8) - 1;
  int bhShift = CountLsbZeros(blkHeight * 8);
  int robSize = 512 * blkHeight * widthInGobs;
  int xShift = CountLsbZeros(512 * blkHeight);
  int pitch = (width * bpp + 3) & ~3;
  auto result = (uint8_t*)malloc(height * pitch);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int outOffs = (y * width + x) * bpp;
      int x1 = x << bppShift;
      int position = (y >> bhShift) * robSize;
      position += (x1 >> 6) << xShift;
      position += ((y & bhMask) >> 3) << 9;
      position += ((x1 & 0x3f) >> 5) << 8;
      position += ((y & 0x07) >> 1) << 6;
      position += ((x1 & 0x1f) >> 4) << 5;
      position += ((y & 0x01) >> 0) << 4;
      position += ((x1 & 0x0f) >> 0) << 0;

      for (int i = 0; i < bpp; i++) result[outOffs + i] = data[position + i];
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
  ImpLog(LL_Warning, LC_TextureLoad, "Unknown texture format, returning 0!\n");
  return 0;
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
    default:
      break;
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
    default:
      break;
  }
  ImpLog(LL_Warning, LC_TextureLoad,
         "Unknown number of bytes per texel, returning 0!\n");
  return 0;
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
        auto unswizzled =
            UnSwizzle(element.Width, element.Height, blk_width, blk_height, bpp,
                      element.BlockHeightLog2, dataBuff);
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

        default:
          ImpLog(LL_Warning, LC_TextureLoad, "Unknown texture format!\n");
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