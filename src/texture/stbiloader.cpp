#include "texture.h"
#include "../io/stream.h"
#include "../log.h"
#include <stb_image.h>

namespace Impacto {
namespace TexLoad {

int StbiRead(void* user, char* data, int size) {
  return (int)(((Io::Stream*)user)->Read(data, size));
}
void StbiSkip(void* user, int n) { ((Io::Stream*)user)->Seek(n, RW_SEEK_CUR); }
int StbiEof(void* user) {
  Io::Stream* stream = (Io::Stream*)user;
  return stream->Position >= stream->Meta.Size;
}

static stbi_io_callbacks const StbiCallbacks{StbiRead, StbiSkip, StbiEof};

bool TextureLoadSTBI(Io::Stream* stream, Texture* outTexture) {
  int x, y, channels_in_file;

  uint8_t* image = stbi_load_from_callbacks(&StbiCallbacks, stream, &x, &y,
                                            &channels_in_file, STBI_default);
  if (image == 0) {
    stream->Seek(0, RW_SEEK_SET);
    return false;
  }

  switch (channels_in_file) {
    case 1: {
      outTexture->Format = TexFmt_U8;
      break;
    }
    case 3: {
      outTexture->Format = TexFmt_RGB;
      break;
    }
    case 4: {
      outTexture->Format = TexFmt_RGBA;
      break;
    }
    default: {
      ImpLog(LL_Error, LC_TextureLoad, "STBI: unsupported channel count %d\n",
             channels_in_file);
      free(image);
      stream->Seek(0, RW_SEEK_SET);
      return false;
    }
  }

  outTexture->Width = x;
  outTexture->Height = y;
  outTexture->Buffer = image;
  outTexture->BufferSize = channels_in_file * x * y;

  return true;
}

static bool _registered = Texture::AddTextureLoader(&TextureLoadSTBI);

}  // namespace TexLoad
}  // namespace Impacto