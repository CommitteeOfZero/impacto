#include <glad/glad.h>
#include <SDL_opengl.h>

#include "window.h"
#include "log.h"

#include "../vendor/nuklear/nuklear_sdl_gl3.h"

namespace Impacto {

static bool IsInit = false;
int g_WindowWidth = 0;
int g_WindowHeight = 0;
int g_MsaaCount = 0;
float g_RenderScale = 1.0f;
SDL_Window* g_SDLWindow;
SDL_GLContext g_GLContext;
bool g_WindowDimensionsChanged;

float g_DesignWidth = 1280.0f;
float g_DesignHeight = 720.0f;

GLuint g_DrawRT = 0;
GLuint g_ReadRT = 0;
GLuint g_ReadRenderTexture = 0;
static GLuint drawRenderTexture = 0;

int lastWidth = -1;
int lastHeight = -1;
int lastMsaa = 0;
float lastRenderScale = 1.0f;

void WindowUpdateDimensions() {
  g_WindowDimensionsChanged = false;
  SDL_GL_GetDrawableSize(g_SDLWindow, &g_WindowWidth, &g_WindowHeight);
  if (g_WindowWidth != lastWidth || g_WindowHeight != lastHeight ||
      g_MsaaCount != lastMsaa || g_RenderScale != lastRenderScale) {
    g_WindowDimensionsChanged = true;
    ImpLog(LL_Debug, LC_General,
           "Drawable size (pixels): %d x %d (%dx MSAA requested, render scale "
           "%f)\n",
           g_WindowWidth, g_WindowHeight, g_MsaaCount, g_RenderScale);
  }
  lastWidth = g_WindowWidth;
  lastHeight = g_WindowHeight;
  lastMsaa = g_MsaaCount;
  lastRenderScale = g_RenderScale;
}

RectF WindowGetViewport() {
  RectF viewport;
  float scale = fmin((float)g_WindowWidth / g_DesignWidth,
                     (float)g_WindowHeight / g_DesignHeight);
  viewport.Width = g_DesignWidth * scale;
  viewport.Height = g_DesignHeight * scale;
  viewport.X = ((float)g_WindowWidth - viewport.Width) / 2.0f;
  viewport.Y = ((float)g_WindowHeight - viewport.Height) / 2.0f;
  return viewport;
}

RectF WindowGetScaledViewport() {
  RectF viewport = WindowGetViewport();
  viewport.Width *= g_RenderScale;
  viewport.Height *= g_RenderScale;
  viewport.X *= g_RenderScale;
  viewport.Y *= g_RenderScale;
  return viewport;
}

void WindowAdjustEventCoordinates(SDL_Event* ev) {
  // TODO touch (x/y normalized floats)
  Rect viewport = WindowGetViewport();
  if (ev->type == SDL_MOUSEMOTION) {
    ev->motion.x -= viewport.X;
    ev->motion.y += viewport.Y;
  } else if (ev->type == SDL_MOUSEBUTTONDOWN || ev->type == SDL_MOUSEBUTTONUP) {
    ev->button.x -= viewport.X;
    ev->button.y += viewport.Y;
  }
}

void WindowInit() {
  assert(IsInit == false);
  ImpLog(LL_Info, LC_General, "Creating window\n");
  IsInit = true;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
    ImpLog(LL_Fatal, LC_General, "SDL initialisation failed: %s\n",
           SDL_GetError());
    WindowShutdown();
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);

  // TODO DPI aware

  int contextFlags = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
  // TODO this should really be a runtime decision
#ifdef IMPACTO_GL_DEBUG
  contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
#endif

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags);

  g_SDLWindow = SDL_CreateWindow("impacto", SDL_WINDOWPOS_UNDEFINED,
                                 SDL_WINDOWPOS_UNDEFINED, 1280, 720,
                                 SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

  if (g_SDLWindow == NULL) {
    ImpLog(LL_Fatal, LC_General, "SDL window creation failed: %s\n",
           SDL_GetError());
    WindowShutdown();
  }

  SDL_GetWindowSize(g_SDLWindow, &g_WindowWidth, &g_WindowHeight);
  ImpLog(LL_Debug, LC_General, "Window size (screen coords): %d x %d\n",
         g_WindowWidth, g_WindowHeight);

  g_GLContext = SDL_GL_CreateContext(g_SDLWindow);
  if (g_GLContext == NULL) {
    ImpLog(LL_Fatal, LC_General, "OpenGL context creation failed: %s\n",
           SDL_GetError());
    WindowShutdown();
  }

  bool gladOk = gladLoadGLLoader(SDL_GL_GetProcAddress);
  if (!gladOk) {
    ImpLog(LL_Fatal, LC_General, "GLAD initialisation failed\n");
    WindowShutdown();
  }

#ifdef IMPACTO_GL_DEBUG
  GLint flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallbackARB(&LogGLMessageCallback, NULL);
    glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
                             GL_TRUE);
  } else {
    ImpLog(LL_Error, LC_GL, "Could not get debug context\n");
  }
#endif

  // Vsync
  SDL_GL_SetSwapInterval(0);
}

void WindowSetDimensions(int width, int height, int msaa, float renderScale) {
  ImpLog(LL_Info, LC_General,
         "Attempting to change window dimensions to %d x %d, %dx MSAA, render "
         "scale %f\n",
         width, height, msaa, renderScale);
  assert(width > 0 && height > 0 && msaa >= 0 && renderScale > 0.0f);

  SDL_SetWindowSize(g_SDLWindow, width, height);
  g_MsaaCount = msaa;
  g_RenderScale = renderScale;
}

void CleanFBOs() {
  if (drawRenderTexture) glDeleteTextures(1, &drawRenderTexture);
  if (g_ReadRenderTexture) glDeleteTextures(1, &g_ReadRenderTexture);
  if (g_DrawRT) glDeleteFramebuffers(1, &g_DrawRT);
  if (g_ReadRT) glDeleteFramebuffers(1, &g_ReadRT);

  drawRenderTexture = g_ReadRenderTexture = g_DrawRT = g_ReadRT = 0;
}

void WindowSwapRTs() {
  GLuint temp;

  temp = g_DrawRT;
  g_DrawRT = g_ReadRT;
  g_ReadRT = temp;

  temp = drawRenderTexture;
  drawRenderTexture = g_ReadRenderTexture;
  g_ReadRenderTexture = temp;

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, g_DrawRT);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, g_ReadRT);
}

void WindowUpdate() {
  WindowUpdateDimensions();

  Rect viewport = WindowGetViewport();

  if (g_WindowDimensionsChanged) {
    CleanFBOs();

    glGenFramebuffers(1, &g_ReadRT);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, g_ReadRT);
    glGenTextures(1, &g_ReadRenderTexture);

    glBindTexture(GL_TEXTURE_2D, g_ReadRenderTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewport.Width, viewport.Height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, g_ReadRenderTexture, 0);

    glGenFramebuffers(1, &g_DrawRT);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, g_DrawRT);
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
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glViewport(0, 0, g_WindowWidth, g_WindowHeight);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  WindowSwapRTs();

  glViewport(0, 0, viewport.Width, viewport.Height);
  glClear(GL_COLOR_BUFFER_BIT);
}

void WindowDraw() {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, g_DrawRT);

  Rect viewport = WindowGetViewport();

  glBlitFramebuffer(0, 0, viewport.Width, viewport.Height, viewport.X,
                    viewport.Y, viewport.Width + viewport.X,
                    viewport.Height + viewport.Y, GL_COLOR_BUFFER_BIT,
                    GL_NEAREST);

  SDL_GL_SwapWindow(g_SDLWindow);
}

void WindowShutdown() {
  CleanFBOs();
  SDL_GL_DeleteContext(g_GLContext);
  SDL_DestroyWindow(g_SDLWindow);
  SDL_Quit();
}

}  // namespace Impacto