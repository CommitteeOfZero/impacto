#include "window.h"
#include "../io/assetpath.h"
#include "../profile/game.h"
#include "../log.h"
#include "../userconfig.h"
#include "../profile/gamedefinitions.h"
#include <stb_image.h>
#include <memory>
#include <vector>
#include <optional>
#include "../inputsystem.h"

namespace Impacto {

void SetWindowIcon(SDL_Window* window) {
  if (!Profile::Game::WindowIconPath.has_value()) {
    return;
  }

  Io::AssetPath asset;
  asset.FileName = Profile::Game::WindowIconPath->c_str();
  Io::Stream* streamPtr;
  IoError err = asset.Open(&streamPtr);
  if (err != IoError_OK) {
    ImpLog(LogLevel::Warning, LogChannel::General,
           "Could not open window icon file {:s}\n",
           Profile::Game::WindowIconPath->c_str());
    return;
  }
  std::unique_ptr<Io::Stream> stream(streamPtr);
  size_t fileSize = stream->Meta.Size;
  std::vector<uint8_t> fileData(fileSize);
  int64_t bytesRead = stream->Read(fileData.data(), fileSize);

  if (bytesRead != (int64_t)fileSize) {
    ImpLog(LogLevel::Warning, LogChannel::General,
           "Could not read window icon file {:s}\n",
           Profile::Game::WindowIconPath->c_str());
    return;
  }

  int width, height, channels;
  uint8_t* image =
      stbi_load_from_memory((const stbi_uc*)fileData.data(), (int)fileSize,
                            &width, &height, &channels, STBI_rgb_alpha);

  if (!image) {
    ImpLog(LogLevel::Warning, LogChannel::General,
           "Could not load window icon from {:s}\n",
           Profile::Game::WindowIconPath->c_str());
    return;
  }

  SDL_Surface* surface =
      SDL_CreateRGBSurfaceFrom(image, width, height, 32, width * 4, 0x000000FF,
                               0x0000FF00, 0x00FF0000, 0xFF000000);
  if (!surface) {
    ImpLog(LogLevel::Error, LogChannel::General,
           "Could not create SDL surface for window icon from {:s}: {:s}\n",
           Profile::Game::WindowIconPath->c_str(), SDL_GetError());
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
  if (Profile::Game::CursorArrowPath.has_value()) {
    CursorArrow = LoadCursorFromFile(*Profile::Game::CursorArrowPath);
  }
  if (!CursorArrow) {
    CursorArrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
  }

  if (Profile::Game::CursorPointerPath.has_value()) {
    CursorPointer = LoadCursorFromFile(*Profile::Game::CursorPointerPath);
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
      ActiveCursorType = *RequestedCursorType;
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

void BaseWindow::CreateSDLWindow(Uint32 flags) {
  const SDL_Rect bounds = [flags] {
    SDL_Rect result{};
    bool haveBounds;
    if (flags & SDL_WINDOW_FULLSCREEN) {
      haveBounds = SDL_GetDisplayBounds(0, &result) == 0;
    } else {
      haveBounds = SDL_GetDisplayUsableBounds(0, &result) == 0;
    }
    if (!haveBounds) {
      ImpLog(LogLevel::Error, LogChannel::Render,
             "Failed to get display bounds: {}.\n", SDL_GetError());
      throw std::runtime_error("Failed to get display info.");
    }
    return result;
  }();

  auto clampAspectRatio = [this](int boundW, int boundH) {
    if (WindowWidth > boundW || WindowHeight > boundH) {
      const float aspect =
          static_cast<float>(WindowWidth) / static_cast<float>(WindowHeight);
      int fitW = boundW;
      int fitH = static_cast<int>(boundW / aspect);
      if (fitH > boundH) {
        fitH = boundH;
        fitW = static_cast<int>(boundH * aspect);
      }
      WindowWidth = fitW;
      WindowHeight = fitH;
    }
  };

  auto const& config = UserConfig::CommonSettings;
  WindowWidth = config.ResolutionWidth;
  WindowHeight = config.ResolutionHeight;

  if (!UserConfig::ActiveGame.empty()) {
    auto const& gameConfig = UserConfig::ActiveGameSettings();

    if (gameConfig.ResolutionHeight.has_value() ^
        gameConfig.ResolutionHeight.has_value()) {
      ImpLog(LogLevel::Warning, LogChannel::Render,
             "Only one of Resolution Height or Resolution Width is configured, "
             "defaulting to game resolution.");
    }
    if (gameConfig.ResolutionWidth && gameConfig.ResolutionHeight) {
      WindowWidth = *gameConfig.ResolutionWidth;
      WindowHeight = *gameConfig.ResolutionHeight;
    } else {
      WindowWidth = static_cast<int>(Profile::Game::DesignWidth);
      WindowHeight = static_cast<int>(Profile::Game::DesignHeight);
    }
    if (gameConfig.Fullscreen) {
      flags |= SDL_WINDOW_FULLSCREEN;
      clampAspectRatio(bounds.w, bounds.h);
    }
  }

  flags |= SDL_WINDOW_HIDDEN;
  SDLWindow = SDL_CreateWindow(Profile::Game::WindowName,
                               SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                               WindowWidth, WindowHeight, flags);
  if (SDLWindow == NULL) {
    ImpLog(LogLevel::Error, LogChannel::General,
           "Window creation failed: {:s}\n", SDL_GetError());
    throw std::runtime_error("Failed to create window");
  }
  SDL_GetWindowSize(SDLWindow, &WindowWidth, &WindowHeight);

  if ((flags & SDL_WINDOW_FULLSCREEN) == 0) {
    int top, left, bottom, right;

    if (SDL_GetWindowBordersSize(SDLWindow, &top, &left, &bottom, &right) ==
        0) {
      const int targetWidth = bounds.w - (left + right);
      const int targetHeight = bounds.h - (top + bottom);
      clampAspectRatio(targetWidth, targetHeight);
      const int posX = bounds.x + left + (targetWidth - WindowWidth) / 2;
      const int posY = bounds.y + top + (targetHeight - WindowHeight) / 2;

      SDL_SetWindowSize(SDLWindow, WindowWidth, WindowHeight);
      SDL_SetWindowPosition(SDLWindow, posX, posY);
    }
  }

  ImpLog(LogLevel::Debug, LogChannel::General,
         "Window size (screen coords): {:d} x {:d}\n", WindowWidth,
         WindowHeight);
  SDL_ShowWindow(SDLWindow);
  SDL_GetWindowSize(SDLWindow, &WindowWidth, &WindowHeight);
}
}  // namespace Impacto
