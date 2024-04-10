#include "window.h"

#include <glad/glad.h>
#include <SDL_opengl.h>

#include "../renderer.h"

#include "../../log.h"
#include "glc.h"

#include "../../profile/game.h"

#ifndef IMPACTO_DISABLE_IMGUI
#include "imgui_impl_opengl3.h"
#endif
#include "../../game.h"
#include "../../profile/vm.h"

namespace Impacto {
namespace OpenGL {

void GLWindow::UpdateDimensions() {
  WindowDimensionsChanged = false;
  SDL_GL_GetDrawableSize(SDLWindow, &WindowWidth, &WindowHeight);
  if (WindowWidth != lastWidth || WindowHeight != lastHeight ||
      MsaaCount != lastMsaa || RenderScale != lastRenderScale) {
    WindowDimensionsChanged = true;
    ImpLog(LL_Debug, LC_General,
           "Drawable size (pixels): %d x %d (%dx MSAA requested, render scale "
           "%f)\n",
           WindowWidth, WindowHeight, MsaaCount, RenderScale);
  }
  lastWidth = WindowWidth;
  lastHeight = WindowHeight;
  lastMsaa = MsaaCount;
  lastRenderScale = RenderScale;

  int osWindowWidth, osWindowHeight;
  SDL_GetWindowSize(SDLWindow, &osWindowWidth, &osWindowHeight);
  DpiScaleX = (float)WindowWidth / (float)osWindowWidth;
  DpiScaleY = (float)WindowHeight / (float)osWindowHeight;
  SDL_SetWindowInputFocus(SDLWindow);
}

RectF GLWindow::GetViewport() {
  RectF viewport;
  float scale = fmin((float)WindowWidth / Profile::DesignWidth,
                     (float)WindowHeight / Profile::DesignHeight);
  viewport.Width = Profile::DesignWidth * scale;
  viewport.Height = Profile::DesignHeight * scale;
  viewport.X = ((float)WindowWidth - viewport.Width) / 2.0f;
  viewport.Y = ((float)WindowHeight - viewport.Height) / 2.0f;
  return viewport;
}

RectF GLWindow::GetScaledViewport() {
  RectF viewport = GetViewport();
  viewport.Width *= RenderScale;
  viewport.Height *= RenderScale;
  viewport.X *= RenderScale;
  viewport.Y *= RenderScale;
  return viewport;
}

void GLWindow::TryCreateGL(GraphicsApi api) {
  if (GLContext != NULL) return;

  if (SDLWindow) {
    SDL_DestroyWindow(SDLWindow);
    SDLWindow = NULL;
  }

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  // Required for blending to work right in Firefox
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);

  int contextFlags = 0;

  switch (api) {
    case GfxApi_GL:
      ImpLog(LL_Info, LC_General, "Trying to create desktop GL context\n");
      SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "0");
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                          SDL_GL_CONTEXT_PROFILE_CORE);

      contextFlags |= SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;

#if IMPACTO_GL_DEBUG
      contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
#endif
      break;

    case GfxApi_ForceDesktopGLES:
      ImpLog(LL_Info, LC_General,
             "Trying to create GLES on desktop GL context\n");
      SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "0");
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                          SDL_GL_CONTEXT_PROFILE_ES);

      break;

    case GfxApi_ForceNativeGLES:
      ImpLog(LL_Info, LC_General, "Trying to create native GLES context\n");

#ifndef EMSCRIPTEN
      // Emscripten's EGL implementation fails when you ask it for GLES 3.0,
      // though that is what it gives you when building for WebGL 2...
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
      SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "1");
#endif
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                          SDL_GL_CONTEXT_PROFILE_ES);
      break;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags);

  uint32_t windowFlags = SDL_WINDOW_OPENGL;
#if IMPACTO_USE_SDL_HIGHDPI
  windowFlags |= SDL_WINDOW_ALLOW_HIGHDPI;
#endif

  SDLWindow =
      SDL_CreateWindow(Profile::WindowName, SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, 1920, 1080, windowFlags);

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
  } else {
    ActualGraphicsApi = api;
  }

#ifndef IMPACTO_DISABLE_IMGUI
  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForOpenGL(SDLWindow, GLContext);
  ImGui_ImplOpenGL3_Init();
#endif
}

void GLWindow::Init() {
  assert(IsInit == false);
  ImpLog(LL_Info, LC_General, "Creating window\n");
  IsInit = true;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
    ImpLog(LL_Fatal, LC_General, "SDL initialisation failed: %s\n",
           SDL_GetError());
    Shutdown();
    return;
  }

  SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");

  switch (GraphicsApiHint) {
    case GfxApi_GL:
      TryCreateGL(GfxApi_GL);
      TryCreateGL(GfxApi_ForceDesktopGLES);
      TryCreateGL(GfxApi_ForceNativeGLES);
      break;
    case GfxApi_ForceDesktopGLES:
      TryCreateGL(GfxApi_ForceDesktopGLES);
      TryCreateGL(GfxApi_ForceNativeGLES);
      TryCreateGL(GfxApi_GL);
      break;
    case GfxApi_ForceNativeGLES:
      TryCreateGL(GfxApi_ForceNativeGLES);
      TryCreateGL(GfxApi_ForceDesktopGLES);
      TryCreateGL(GfxApi_GL);
      break;
  }

  if (GLContext == NULL) {
    ImpLog(LL_Fatal, LC_General,
           "All options for OpenGL context creation failed\n");
    Shutdown();
    return;
  }

  SDL_GetWindowSize(SDLWindow, &WindowWidth, &WindowHeight);
  ImpLog(LL_Debug, LC_General, "Window size (screen coords): %d x %d\n",
         WindowWidth, WindowHeight);

  bool gladOk;
  if (ActualGraphicsApi == GfxApi_GL) {
    gladOk = gladLoadGLLoader(SDL_GL_GetProcAddress);
  } else {
    gladOk = gladLoadGLES2Loader(SDL_GL_GetProcAddress);
  }
  if (!gladOk) {
    ImpLog(LL_Fatal, LC_General, "GLAD initialisation failed\n");
    Shutdown();
  }

  // At the time of writing, this is on by default on Intel on Windows until we
  // resize the window???
  glDisable(GL_FRAMEBUFFER_SRGB);

#if IMPACTO_GL_DEBUG
  if (ActualGraphicsApi == GfxApi_GL) {
    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
      GLDebug = true;
      glEnable(GL_DEBUG_OUTPUT);
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      glDebugMessageCallbackARB(&LogGLMessageCallback, NULL);
      glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
                               NULL, GL_TRUE);
    } else {
      ImpLog(LL_Error, LC_GL, "Could not get debug context\n");
    }
  } else {
    ImpLog(LL_Error, LC_GL,
           "IMPACTO_GL_DEBUG defined but no debug context requested because "
           "we're using GLES\n");
  }
#endif

  // Vsync
  SDL_GL_SetSwapInterval(0);
}

void GLWindow::SetDimensions(int width, int height, int msaa,
                             float renderScale) {
  ImpLog(LL_Info, LC_General,
         "Attempting to change window dimensions to %d x %d, %dx MSAA, render "
         "scale %f\n",
         width, height, msaa, renderScale);
  assert(width > 0 && height > 0 && msaa >= 0 && renderScale > 0.0f);

  SDL_SetWindowSize(SDLWindow, (int)((float)width / DpiScaleX),
                    (int)((float)height / DpiScaleY));

  MsaaCount = msaa;
  RenderScale = renderScale;
}

void GLWindow::CleanFBOs() {
  if (drawRenderTexture) glDeleteTextures(1, &drawRenderTexture);
  if (ReadRenderTexture) glDeleteTextures(1, &ReadRenderTexture);
  if (DrawRT) GLC::DeleteFramebuffers(1, &DrawRT);
  if (ReadRT) GLC::DeleteFramebuffers(1, &ReadRT);

  drawRenderTexture = ReadRenderTexture = DrawRT = ReadRT = 0;
}

void GLWindow::SwapRTs() {
  GLuint temp;

  temp = DrawRT;
  DrawRT = ReadRT;
  ReadRT = temp;

  temp = drawRenderTexture;
  drawRenderTexture = ReadRenderTexture;
  ReadRenderTexture = temp;

  GLC::BindFramebuffer(GL_DRAW_FRAMEBUFFER, DrawRT);
  GLC::BindFramebuffer(GL_READ_FRAMEBUFFER, ReadRT);
}

void GLWindow::Update() {
  UpdateDimensions();

  Rect viewport = GetViewport();

  if (WindowDimensionsChanged) {
    CleanFBOs();

    glGenFramebuffers(1, &ReadRT);
    GLC::BindFramebuffer(GL_READ_FRAMEBUFFER, ReadRT);
    glGenTextures(1, &ReadRenderTexture);

    glBindTexture(GL_TEXTURE_2D, ReadRenderTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewport.Width, viewport.Height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, ReadRenderTexture, 0);

    glGenFramebuffers(1, &DrawRT);
    GLC::BindFramebuffer(GL_DRAW_FRAMEBUFFER, DrawRT);
    glGenTextures(1, &drawRenderTexture);

    glBindTexture(GL_TEXTURE_2D, drawRenderTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewport.Width, viewport.Height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, drawRenderTexture, 0);
  }

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  if (viewport.X != 0 || viewport.Y != 0) {
    // Clear outside letter-/pillarbox
    // Unfortunately we seem to need to do this every frame - I get content
    // flickering into the margins on Linux otherwise
    GLC::BindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glViewport(0, 0, WindowWidth, WindowHeight);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  SwapRTs();

  glViewport(0, 0, viewport.Width, viewport.Height);
  glClear(GL_COLOR_BUFFER_BIT);

  glEnable(GL_BLEND);

  if (Profile::Vm::GameInstructionSet == +Vm::InstructionSet::CHLCC) {
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
  } else {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
}

void GLWindow::Draw() {
#ifndef IMPACTO_DISABLE_IMGUI
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

  GLC::BindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  GLC::BindFramebuffer(GL_READ_FRAMEBUFFER, DrawRT);

  Rect viewport = GetViewport();

  glBlitFramebuffer(0, 0, viewport.Width, viewport.Height, viewport.X,
                    viewport.Y, viewport.Width + viewport.X,
                    viewport.Height + viewport.Y, GL_COLOR_BUFFER_BIT,
                    GL_NEAREST);

  SDL_GL_SwapWindow(SDLWindow);
}

void GLWindow::Shutdown() {
  CleanFBOs();
  SDL_GL_DeleteContext(GLContext);
  SDL_DestroyWindow(SDLWindow);
  SDL_Quit();
  // TODO move exit to users
  exit(0);
}

}  // namespace OpenGL
}  // namespace Impacto