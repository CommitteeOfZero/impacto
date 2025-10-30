#include "window.h"
#include "../io/assetpath.h"
#include "../profile/game.h"
#include "../log.h"
#include <stb_image.h>
#include <memory>
#include <vector>

namespace Impacto {

void SetWindowIcon(SDL_Window* window) {
  if (!Profile::WindowIconPath.has_value()) {
    return;
  }

  Io::AssetPath asset;
  asset.FileName = Profile::WindowIconPath->c_str();
  Io::Stream* streamPtr;
  IoError err = asset.Open(&streamPtr);
  if (err != IoError_OK) {
    ImpLog(LogLevel::Warning, LogChannel::General,
           "Could not open window icon file {:s}\n",
           Profile::WindowIconPath->c_str());
    return;
  }
  std::unique_ptr<Io::Stream> stream(streamPtr);
  size_t fileSize = stream->Meta.Size;
  std::vector<uint8_t> fileData(fileSize);
  int64_t bytesRead = stream->Read(fileData.data(), fileSize);

  if (bytesRead != (int64_t)fileSize) {
    ImpLog(LogLevel::Warning, LogChannel::General,
           "Could not read window icon file {:s}\n",
           Profile::WindowIconPath->c_str());
    return;
  }

  int width, height, channels;
  uint8_t* image =
      stbi_load_from_memory((const stbi_uc*)fileData.data(), (int)fileSize,
                            &width, &height, &channels, STBI_rgb_alpha);

  if (!image) {
    ImpLog(LogLevel::Warning, LogChannel::General,
           "Could not load window icon from {:s}\n",
           Profile::WindowIconPath->c_str());
    return;
  }

  SDL_Surface* surface =
      SDL_CreateRGBSurfaceFrom(image, width, height, 32, width * 4, 0x000000FF,
                               0x0000FF00, 0x00FF0000, 0xFF000000);
  if (!surface) {
    ImpLog(LogLevel::Error, LogChannel::General,
           "Could not create SDL surface for window icon from {:s}: {:s}\n",
           Profile::WindowIconPath->c_str(), SDL_GetError());
    stbi_image_free(image);
    return;
  }

  SDL_SetWindowIcon(window, surface);
  SDL_FreeSurface(surface);
  stbi_image_free(image);
}

}  // namespace Impacto
