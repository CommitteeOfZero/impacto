#include "texture.h"
#include "../io/inputstream.h"
#include "../log.h"
#include <webp/decode.h>

namespace Impacto {
namespace TexLoad {

bool TextureLoadWebP(Io::InputStream* stream, Texture* outTexture) {
  stream->Seek(0, RW_SEEK_END);
  size_t dataSize = stream->Position;
  stream->Seek(0, RW_SEEK_SET);
  uint8_t* rawData = (uint8_t*)malloc(dataSize);
  stream->Read(rawData, dataSize);

  int width = 0, height = 0;
  int res = WebPGetInfo(rawData, dataSize, &width, &height);

  if (!res) {
    stream->Seek(0, RW_SEEK_SET);
    free(rawData);
    return false;
  }

  WebPBitstreamFeatures features;
  auto status = WebPGetFeatures(rawData, dataSize, &features);

  if (status != VP8_STATUS_OK) {
    stream->Seek(0, RW_SEEK_SET);
    free(rawData);
    return false;
  }

  uint8_t* image = 0;
  if (features.has_alpha) {
    outTexture->Format = TexFmt_RGBA;
    image = WebPDecodeRGBA(rawData, dataSize, &width, &height);
  } else {
    outTexture->Format = TexFmt_RGB;
    image = WebPDecodeRGB(rawData, dataSize, &width, &height);
  }

  if (image == 0) {
    stream->Seek(0, RW_SEEK_SET);
    free(rawData);
    return false;
  }

  outTexture->Width = width;
  outTexture->Height = height;
  outTexture->Buffer = image;
  outTexture->BufferSize = (3 + features.has_alpha) * width * height;

  free(rawData);
  return true;
}

static bool _registered = Texture::AddTextureLoader(&TextureLoadWebP);

}  // namespace TexLoad
}  // namespace Impacto