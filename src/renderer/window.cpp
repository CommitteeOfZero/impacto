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
#include "opengl/window.h"
#include "vulkan/window.h"
#include "dx9/window.h"

namespace Impacto {

#ifdef __APPLE__
namespace MacOS {
void HideSystemUI(bool hide);
}
#endif

void InitWindow() {
  switch (UserConfig::AdvancedSettings.ActiveRenderer) {
#ifndef IMPACTO_DISABLE_OPENGL
    case RendererType::OpenGL:
      Window = new OpenGL::GLWindow();
      break;
#endif
#ifndef IMPACTO_DISABLE_VULKAN
    case RendererType::Vulkan:
      Window = new Vulkan::VulkanWindow();
      break;
#endif
#ifndef IMPACTO_DISABLE_DX9
    case RendererType::DirectX9:
      Window = new DirectX9::DirectX9Window();
      break;
#endif
    default:
      ImpLog(LogLevel::Error, LogChannel::Render,
             "Failed to create window: Unknown or unsupported renderer "
             "selected!\n");
      exit(1);
  }
  Window->SetVSync(UserConfig::GetEffectiveVSync());
  Window->Init();
  SetWindowIcon(Window->SDLWindow);
}

void SetWindowIcon(SDL_Window* window) {
  const char* const iconPath = (Profile::Game::WindowIconPath.has_value())
                                   ? Profile::Game::WindowIconPath->c_str()
                                   : "resources/common/icons/icon.png";

  Io::AssetPath asset;
  asset.FileName = iconPath;
  Io::Stream* streamPtr;
  IoError err = asset.Open(&streamPtr);
  if (err != IoError_OK) {
    ImpLog(LogLevel::Warning, LogChannel::General,
           "Could not open window icon file {:s}\n", iconPath);
    return;
  }
  std::unique_ptr<Io::Stream> stream(streamPtr);
  size_t fileSize = stream->Meta.Size;
  std::vector<uint8_t> fileData(fileSize);
  int64_t bytesRead = stream->Read(fileData.data(), fileSize);

  if (bytesRead != (int64_t)fileSize) {
    ImpLog(LogLevel::Warning, LogChannel::General,
           "Could not read window icon file {:s}\n", iconPath);
    return;
  }

  int width, height, channels;
  uint8_t* image =
      stbi_load_from_memory((const stbi_uc*)fileData.data(), (int)fileSize,
                            &width, &height, &channels, STBI_rgb_alpha);

  if (!image) {
    ImpLog(LogLevel::Warning, LogChannel::General,
           "Could not load window icon from {:s}\n", iconPath);
    return;
  }

  SDL_Surface* surface = SDL_CreateSurfaceFrom(
      width, height, SDL_PIXELFORMAT_RGBA32, image, width * 4);
  if (!surface) {
    ImpLog(LogLevel::Error, LogChannel::General,
           "Could not create SDL surface for window icon from {:s}: {:s}\n",
           iconPath, SDL_GetError());
    stbi_image_free(image);
    return;
  }

  SDL_SetWindowIcon(window, surface);
  SDL_DestroySurface(surface);
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

  SDL_Surface* surface = SDL_CreateSurfaceFrom(
      width, height, SDL_PIXELFORMAT_RGBA32, image, width * 4);
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

  SDL_DestroySurface(surface);
  stbi_image_free(image);
  return cursor;
}

void InitCursors() {
  if (Profile::Game::CursorArrowPath.has_value()) {
    CursorArrow = LoadCursorFromFile(*Profile::Game::CursorArrowPath);
  }
  if (!CursorArrow) {
    CursorArrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
  }

  if (Profile::Game::CursorPointerPath.has_value()) {
    CursorPointer = LoadCursorFromFile(*Profile::Game::CursorPointerPath);
  }
  if (!CursorPointer) {
    CursorPointer = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
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

void BaseWindow::ToggleFullscreen() {
  if (UserConfig::GetActiveGame().empty()) return;
  auto& gameSettings = UserConfig::ActiveGameSettings();
  if (CurrentMode == DisplayMode::Windowed) {
    gameSettings.Display = UserConfig::CommonSettings.PreferBorderless
                               ? DisplayMode::Borderless
                               : DisplayMode::Fullscreen;
  } else {
    gameSettings.Display = DisplayMode::Windowed;
  }
  ApplyWindowSettings(true);
}

SDL_Rect BaseWindow::GetDisplayBounds(std::optional<SDL_WindowFlags> flags) {
  SDL_Rect result{};
  bool haveBounds;

  SDL_WindowFlags windowFlags = flags.value_or(SDL_GetWindowFlags(SDLWindow));
  SDL_DisplayID display = SDL_GetDisplayForWindow(SDLWindow);
  if (windowFlags & SDL_WINDOW_FULLSCREEN) {
    haveBounds = SDL_GetDisplayBounds(display, &result);
  } else {
    haveBounds = SDL_GetDisplayUsableBounds(display, &result);
  }
  if (!haveBounds) {
    ImpLog(LogLevel::Fatal, LogChannel::Render,
           "Failed to get display bounds: {}.\n", SDL_GetError());
    throw std::runtime_error("Failed to get display info.");
  }
  return result;
}

RectF BaseWindow::GetViewport() {
  RectF viewport;

  const float designWidth =
      Profile::Game::HasInit ? Profile::Game::DesignWidth : WindowWidth;
  const float designHeight =
      Profile::Game::HasInit ? Profile::Game::DesignHeight : WindowHeight;

  float scale = fmin((float)WindowWidth / designWidth,
                     (float)WindowHeight / designHeight);
  viewport.Width = designWidth * scale;
  viewport.Height = designHeight * scale;
  viewport.X = ((float)WindowWidth - viewport.Width) / 2.0f;
  viewport.Y = ((float)WindowHeight - viewport.Height) / 2.0f;
  return viewport;
}

RectF BaseWindow::GetLogicalViewport() {
  RectF viewport;
  float scale = fmin((float)LogicalWindowWidth / Profile::Game::DesignWidth,
                     (float)LogicalWindowHeight / Profile::Game::DesignHeight);
  viewport.Width = Profile::Game::DesignWidth * scale;
  viewport.Height = Profile::Game::DesignHeight * scale;
  viewport.X = ((float)LogicalWindowWidth - viewport.Width) / 2.0f;
  viewport.Y = ((float)LogicalWindowHeight - viewport.Height) / 2.0f;
  return viewport;
}

RectF BaseWindow::GetScaledViewport() {
  RectF viewport = GetViewport();
  viewport.Width *= RenderScale;
  viewport.Height *= RenderScale;
  viewport.X *= RenderScale;
  viewport.Y *= RenderScale;
  return viewport;
}

void BaseWindow::RequestFauxBorderless() {
  // Simulate borderless by making window take up full size of the display
  SDL_Rect bounds = GetDisplayBounds(SDL_WINDOW_FULLSCREEN);

  SDL_SetWindowBordered(SDLWindow, true);
  SDL_SetWindowFullscreen(SDLWindow, false);
  SDL_SetWindowSize(SDLWindow, bounds.w, bounds.h);
  SDL_SetWindowPosition(SDLWindow, 0, 0);
  SDL_SetWindowMaximumSize(SDLWindow, bounds.w, bounds.h);
  SDL_SetWindowMinimumSize(SDLWindow, bounds.w, bounds.h);
}

bool BaseWindow::CreateSDLWindow(SDL_WindowFlags flags) {
  auto const& config = UserConfig::CommonSettings;

#if IMPACTO_USE_SDL_HIGHDPI
  flags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;
#endif

#ifdef __ANDROID__
  flags |= SDL_WINDOW_FULLSCREEN;
#else
  flags |= SDL_WINDOW_RESIZABLE;
#endif

  SDLWindow = SDL_CreateWindow("Impacto", config.ResolutionWidth,
                               config.ResolutionHeight, flags);

  if (SDLWindow == NULL) {
    ImpLog(LogLevel::Error, LogChannel::General,
           "Window creation failed: {:s}\n", SDL_GetError());
    return false;
  }
  SDL_GetWindowSizeInPixels(SDLWindow, &WindowWidth, &WindowHeight);
  SDL_GetWindowSize(SDLWindow, &LogicalWindowWidth, &LogicalWindowHeight);
  DpiScale = SDL_GetWindowDisplayScale(SDLWindow);
  return true;
}

void BaseWindow::Update() { UpdateImpl(); }

void BaseWindow::UpdateDisplayMode() {
  SDL_SetWindowMaximumSize(SDLWindow, 0, 0);
  SDL_SetWindowMinimumSize(SDLWindow, 0, 0);
  SDL_DisplayID currentDisplay = SDL_GetDisplayForWindow(SDLWindow);
  if (currentDisplay == 0) {
    ImpLog(LogLevel::Error, LogChannel::General,
           "Failed to get window current display \"{:s}\"\n"
           "\tDefaulting to primary display.\n",
           SDL_GetError());
    currentDisplay = SDL_GetPrimaryDisplay();
  }

  if (CurrentMode == DisplayMode::Fullscreen) {
    UserConfig::CommonSettings.PreferBorderless = false;

#ifdef __APPLE__
    SDL_SetWindowFullscreenMode(SDLWindow, NULL);
    SDL_SetWindowFullscreen(SDLWindow, true);
#else
    SDL_DisplayMode closest{};
    if (SDL_GetClosestFullscreenDisplayMode(
            currentDisplay, WindowWidth, WindowHeight, 0.0f, false, &closest)) {
      SDL_SetWindowFullscreenMode(SDLWindow, &closest);
    }

    SDL_SetWindowFullscreen(SDLWindow, true);
#endif
  } else if (CurrentMode == DisplayMode::Borderless) {
    UserConfig::CommonSettings.PreferBorderless = true;

#ifdef __APPLE__
    const bool fauxBorderless = true;
#else
    const bool fauxBorderless = UserConfig::AdvancedSettings.ForceBorderless;
#endif
    if (fauxBorderless) {
      RequestFauxBorderless();
    } else {
      SDL_SetWindowFullscreenMode(SDLWindow, NULL);
      SDL_SetWindowFullscreen(SDLWindow, true);
    }
  } else {
    SDL_SetWindowBordered(SDLWindow, true);
    SDL_SetWindowFullscreen(SDLWindow, false);
    SDL_SetWindowSize(SDLWindow, LogicalWindowWidth, LogicalWindowHeight);
    SDL_SetWindowPosition(SDLWindow,
                          SDL_WINDOWPOS_CENTERED_DISPLAY(currentDisplay),
                          SDL_WINDOWPOS_CENTERED_DISPLAY(currentDisplay));
    SDL_SetWindowMaximumSize(SDLWindow, LogicalWindowWidth,
                             LogicalWindowHeight);
    SDL_SetWindowMinimumSize(SDLWindow, LogicalWindowWidth,
                             LogicalWindowHeight);
  }

  if (!UserConfig::GetActiveGame().empty()) {
    UserConfig::ActiveGameSettings().Display = CurrentMode;
  }
}

void BaseWindow::LoadSettings() {
  auto const& config = UserConfig::CommonSettings;
  LogicalWindowWidth = config.ResolutionWidth;
  LogicalWindowHeight = config.ResolutionHeight;

  if (!UserConfig::GetActiveGame().empty()) {
    auto const& gameConfig = UserConfig::ActiveGameSettings();

    if (gameConfig.ResolutionWidth.has_value() ^
        gameConfig.ResolutionHeight.has_value()) {
      ImpLog(LogLevel::Warning, LogChannel::Render,
             "Only one of Resolution Height or Resolution Width is configured, "
             "defaulting to game resolution.");
    }
    if (gameConfig.ResolutionWidth && gameConfig.ResolutionHeight) {
      LogicalWindowWidth = *gameConfig.ResolutionWidth;
      LogicalWindowHeight = *gameConfig.ResolutionHeight;
    } else if (Profile::Game::HasInit) {
      LogicalWindowWidth = static_cast<int>(Profile::Game::DesignWidth);
      LogicalWindowHeight = static_cast<int>(Profile::Game::DesignHeight);
    }
    WindowWidth = static_cast<int>(std::round(DpiScale * LogicalWindowWidth));
    WindowHeight = static_cast<int>(std::round(DpiScale * LogicalWindowHeight));
    CurrentMode = gameConfig.Display;
  }
  VSync = UserConfig::GetEffectiveVSync();
  WindowDimensionsChanged = true;
}

void BaseWindow::ApplyWindowSettings(bool loadSettings) {
  if (loadSettings) LoadSettings();
  SetVSync(VSync);

  SDL_SetWindowTitle(SDLWindow, Profile::Game::WindowName.c_str());
  UpdateDisplayMode();
  Update();
}

bool BaseWindow::HandleWindowEvents(SDL_Event const* evt) {
  auto updateState = [this] {
    SDL_GetWindowSizeInPixels(SDLWindow, &WindowWidth, &WindowHeight);
    SDL_GetWindowSize(SDLWindow, &LogicalWindowWidth, &LogicalWindowHeight);
    DpiScale = SDL_GetWindowDisplayScale(SDLWindow);
    RecreateFBOs = true;
    WindowDimensionsChanged = true;

    if (!UserConfig::GetActiveGame().empty()) {
      UserConfig::ActiveGameSettings().Display = CurrentMode;
    }
#ifdef __APPLE__
    MacOS::HideSystemUI(CurrentMode == DisplayMode::Borderless);
#endif
    Update();
  };
  switch (evt->type) {
    case SDL_EVENT_WINDOW_FOCUS_LOST:
    case SDL_EVENT_WINDOW_HIDDEN:
      Input::ClearTouch();
      return true;
    case SDL_EVENT_WINDOW_ENTER_FULLSCREEN: {
#ifdef __APPLE__
      CurrentMode = DisplayMode::Fullscreen;
#else
      auto const* sdlDispMode = SDL_GetWindowFullscreenMode(SDLWindow);
      if (CurrentMode == DisplayMode::Windowed) {
        CurrentMode = sdlDispMode != nullptr ? DisplayMode::Fullscreen
                                             : DisplayMode::Borderless;
      }
#endif
      updateState();
      return true;
    }
    case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
      // Mac OS's simulated borderless can go from fullscreen to borderless, so
      // don't change that
      if (CurrentMode == DisplayMode::Fullscreen) {
        CurrentMode = DisplayMode::Windowed;
      }
      updateState();
      return true;
    case SDL_EVENT_WINDOW_SAFE_AREA_CHANGED:
    case SDL_EVENT_WINDOW_RESIZED:
    case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
      updateState();
      return true;
    default:
      return false;
  }
}

}  // namespace Impacto
