#include "window.h"

#include "../renderer.h"

#include "../../log.h"

#include "../../profile/game.h"

#include "../../game.h"
#include "../../profile/vm.h"

#ifdef PLATFORM_DREAMCAST
#include <kos.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glkos.h>
#endif

namespace Impacto {
namespace LegacyOpenGL {

void LegacyGLWindow::UpdateDimensions() { WindowDimensionsChanged = false; }

RectF LegacyGLWindow::GetViewport() {
  RectF viewport;
  float scale = fmin((float)WindowWidth / Profile::DesignWidth,
                     (float)WindowHeight / Profile::DesignHeight);
  viewport.Width = Profile::DesignWidth * scale;
  viewport.Height = Profile::DesignHeight * scale;
  viewport.X = ((float)WindowWidth - viewport.Width) / 2.0f;
  viewport.Y = ((float)WindowHeight - viewport.Height) / 2.0f;
  return viewport;
}

RectF LegacyGLWindow::GetScaledViewport() {
  RectF viewport = GetViewport();
  viewport.Width *= RenderScale;
  viewport.Height *= RenderScale;
  viewport.X *= RenderScale;
  viewport.Y *= RenderScale;
  return viewport;
}

void LegacyGLWindow::Init() {
  assert(IsInit == false);
  ImpLog(LL_Info, LC_General, "Creating window\n");
  IsInit = true;

#ifdef IMPACTO_SDL1_COMPAT
  uint32_t sdlInitFlags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;
#else
  uint32_t sdlInitFlags = SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER;
#endif

  if (SDL_Init(sdlInitFlags) != 0) {
    ImpLog(LL_Fatal, LC_General, "SDL initialisation failed: %s\n",
           SDL_GetError());
    Shutdown();
    return;
  }

#ifdef IMPACTO_SDL1_COMPAT
  SDL_JoystickEventState(SDL_ENABLE);
#endif

#ifdef PLATFORM_DREAMCAST
  WindowWidth = 640.0f;
  WindowHeight = 480.0f;

  RectF viewport = GetViewport();

  glKosInit();
#else
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                      SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

  uint32_t windowFlags = SDL_WINDOW_OPENGL;
  SDLWindow = SDL_CreateWindow(Profile::WindowName, SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, 1280, 720, windowFlags);

  if (SDLWindow == NULL) {
    ImpLog(LL_Error, LC_General, "Window creation failed: %s\n",
           SDL_GetError());
    return;
  }

  GLContext = SDL_GL_CreateContext(SDLWindow);
  if (GLContext == NULL) {
    ImpLog(LL_Error, LC_General, "OpenGL context creation failed: %s\n",
           SDL_GetError());
    return;
  }

  SDL_GetWindowSize(SDLWindow, &WindowWidth, &WindowHeight);
  ImpLog(LL_Debug, LC_General, "Window size (screen coords): %d x %d\n",
         WindowWidth, WindowHeight);

  RectF viewport = GetViewport();
  bool gladOk = gladLoadGLLoader(SDL_GL_GetProcAddress);
  if (!gladOk) {
    ImpLog(LL_Fatal, LC_General, "GLAD initialisation failed\n");
    Shutdown();
  }

  SDL_GL_SetSwapInterval(0);
#endif

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glViewport(viewport.X, viewport.Y, viewport.Width, viewport.Height);
  glEnable(GL_TEXTURE_2D);
  glShadeModel(GL_SMOOTH);
}

void LegacyGLWindow::SetDimensions(int width, int height, int msaa,
                                   float renderScale) {
  ImpLog(LL_Info, LC_General,
         "Attempting to change window dimensions to %d x %d, %dx MSAA, render "
         "scale %f\n",
         width, height, msaa, renderScale);
  assert(width > 0 && height > 0 && msaa >= 0 && renderScale > 0.0f);

  MsaaCount = msaa;
  RenderScale = renderScale;
}

void LegacyGLWindow::SwapRTs() {}

void LegacyGLWindow::Update() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void LegacyGLWindow::Draw() {
#ifdef PLATFORM_DREAMCAST
  glKosSwapBuffers();
#elif IMPACTO_SDL1_COMPAT
  SDL_GL_SwapBuffers();
#else
  SDL_GL_SwapWindow(SDLWindow);
#endif
}

void LegacyGLWindow::Shutdown() {
  // TODO move exit to users
  exit(0);
}

}  // namespace LegacyOpenGL
}  // namespace Impacto