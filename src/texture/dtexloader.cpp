#include "texture.h"
#include "../io/inputstream.h"
#include "../log.h"

namespace Impacto {
namespace TexLoad {

static uint32_t const magic = 0x44544558;

bool TextureLoadDTEX(Io::InputStream* stream, Texture* outTexture) {
  // Read metadata
  if (Io::ReadBE<uint32_t>(stream) != magic) {
    stream->Seek(0, RW_SEEK_SET);
    return false;
  }

  ImpLog(LL_Debug, LC_TextureLoad, "Loading DTEX texture\n");

  uint16_t width = Io::ReadLE<uint16_t>(stream);
  uint16_t height = Io::ReadLE<uint16_t>(stream);
  uint32_t type = Io::ReadLE<uint32_t>(stream);
  uint32_t size = Io::ReadLE<uint32_t>(stream);

  memset(outTexture, 0, sizeof(*outTexture));

  outTexture->Width = width;
  outTexture->Height = height;
  outTexture->Buffer = (uint8_t*)malloc(size);
  outTexture->BufferSize = size;
  if (((type >> 27) & 7) == 2)
    outTexture->Format = TexFmt_DC_A;
  else if (((type >> 27) & 7) == 1)
    outTexture->Format = TexFmt_DC;
  stream->Read(outTexture->Buffer, outTexture->BufferSize);

  return true;
}

static bool _registered = Texture::AddTextureLoader(&TextureLoadDTEX);

}  // namespace TexLoad
}  // namespace Impacto