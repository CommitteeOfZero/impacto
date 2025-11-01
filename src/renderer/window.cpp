#include "window.h"
#include "../io/assetpath.h"
#include "../profile/game.h"
#include "../log.h"
#include <stb_image.h>
#include <memory>
#include <vector>
#include <optional>
#include "../inputsystem.h"

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

static SDL_Cursor* CursorArrow = nullptr;
static SDL_Cursor* CursorPointer = nullptr;
static SDL_Cursor* CurrentCursor = nullptr;
static std::optional<CursorType> RequestedCursorType;
static Input::Device PreviousInputDevice = Input::Device::Mouse;

static SDL_Cursor* LoadCursorFromFile(const std::string& path) {
  Io::AssetPath asset;
  asset.FileName = path.c_str();
  Io::Stream* streamPtr;
  IoError err = asset.Open(&streamPtr);
  if (err != IoError_OK) {
    ImpLog(LogLevel::Warning, LogChannel::General,
           "Could not open cursor file {:s}\n", path.c_str());
    return nullptr;
  }
  std::unique_ptr<Io::Stream> stream(streamPtr);
  size_t fileSize = stream->Meta.Size;
  std::vector<uint8_t> fileData(fileSize);
  int64_t bytesRead = stream->Read(fileData.data(), fileSize);
  if (bytesRead != (int64_t)fileSize) {
    ImpLog(LogLevel::Warning, LogChannel::General,
           "Could not read cursor file {:s}\n", path.c_str());
    return nullptr;
  }

  int width, height, channels;
  uint8_t* image =
      stbi_load_from_memory((const stbi_uc*)fileData.data(), (int)fileSize,
                            &width, &height, &channels, STBI_rgb_alpha);
  if (!image) {
    ImpLog(LogLevel::Warning, LogChannel::General,
           "Could not load cursor image from {:s}\n", path.c_str());
    return nullptr;
  }

  SDL_Surface* surface =
      SDL_CreateRGBSurfaceFrom(image, width, height, 32, width * 4, 0x000000FF,
                               0x0000FF00, 0x00FF0000, 0xFF000000);
  if (!surface) {
    ImpLog(LogLevel::Error, LogChannel::General,
           "Could not create SDL surface for cursor from {:s}: {:s}\n",
           path.c_str(), SDL_GetError());
    stbi_image_free(image);
    return nullptr;
  }

  SDL_Cursor* cursor = SDL_CreateColorCursor(surface, 0, 0);
  if (!cursor) {
    ImpLog(LogLevel::Error, LogChannel::General,
           "SDL_CreateColorCursor failed for {:s}: {:s}\n", path.c_str(),
           SDL_GetError());
  }

  SDL_FreeSurface(surface);
  stbi_image_free(image);
  return cursor;
}

void InitCursors() {
  if (Profile::CursorArrowPath.has_value()) {
    CursorArrow = LoadCursorFromFile(*Profile::CursorArrowPath);
  }
  if (!CursorArrow) {
    CursorArrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
  }

  if (Profile::CursorPointerPath.has_value()) {
    CursorPointer = LoadCursorFromFile(*Profile::CursorPointerPath);
  }
  if (!CursorPointer) {
    CursorPointer = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
  }

  if (CursorArrow) {
    SDL_SetCursor(CursorArrow);
    CurrentCursor = CursorArrow;
  }
}

void RequestCursor(CursorType type) {
  if (Input::CurrentInputDevice == Input::Device::Mouse) {
    if (type == CursorType::Pointer && CursorPointer) {
      if (CurrentCursor != CursorPointer) {
        RequestedCursorType = CursorType::Pointer;
      } else {
        RequestedCursorType = std::nullopt;
      }
    } else {
      if (CurrentCursor != CursorArrow) {
        RequestedCursorType = CursorType::Default;
      } else {
        RequestedCursorType = std::nullopt;
      }
    }
  }
}

void ApplyCursorForFrame() {
  SDL_Cursor* actualCursor = SDL_GetCursor();

  if (Input::CurrentInputDevice != PreviousInputDevice) {
    CurrentCursor = actualCursor;
    PreviousInputDevice = Input::CurrentInputDevice;
  }

  SDL_Cursor* desired = CursorArrow;

  if (Input::CurrentInputDevice == Input::Device::Mouse) {
    if (RequestedCursorType.has_value()) {
      if (*RequestedCursorType == CursorType::Pointer && CursorPointer) {
        desired = CursorPointer;
      } else if (*RequestedCursorType == CursorType::Default) {
        desired = CursorArrow;
      }
      RequestedCursorType = std::nullopt;
    } else {
      if (actualCursor != CursorArrow && actualCursor != CursorPointer) {
        desired = CursorArrow;
      } else {
        desired = actualCursor;
      }
    }
  } else {
    RequestedCursorType = std::nullopt;
    desired = CursorArrow;
  }

  if (desired && desired != actualCursor) {
    SDL_SetCursor(desired);
    CurrentCursor = desired;
  } else {
    CurrentCursor = actualCursor;
  }
}

}  // namespace Impacto
