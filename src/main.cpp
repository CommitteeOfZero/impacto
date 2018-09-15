#include "impacto.h"

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <glm/glm.hpp>

#include "log.h"
#include "camera.h"
#include "io/vfs.h"
#include "io/mpkdriver.h"
#include "character3d.h"
#include "shader.h"

using namespace Impacto;

SDL_Window* window = NULL;
bool quit = false;
SDL_GLContext glContext;

Character3D characters[4];

void GameLoop() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      quit = true;
    }
  }

  float const radius = 20.0f;
  static float angle = 0.0f;
  glm::vec3 position =
      glm::vec3(radius * cos(angle), radius, radius * sin(angle));
  angle += 0.02f;
  if (angle >= 2 * M_PI) angle -= 2 * M_PI;
  CameraMove(&g_Camera, position);
  CameraRecalculate(&g_Camera);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  characters[0].Update();
  characters[0].Render();
  characters[1].Update();
  characters[1].Render();

  SDL_GL_SwapWindow(window);
}

int main(int argc, char* argv[]) {
  LogSetConsole(true);
  LogLevelConsole = LL_Max;
  LogChannelsConsole = LC_All;

  MpkRegisterDriver();

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
    ImpLog(LL_Fatal, LC_General, "SDL initialisation failed: %s\n",
           SDL_GetError());
    return 1;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  // TODO do we need a stencil buffer?
  // SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

  // TODO DPI aware

  int contextFlags = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
  // TODO this should really be a runtime decision
#ifdef IMPACTO_GL_DEBUG
  contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
#endif

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags);

  window = SDL_CreateWindow("impacto", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, 1280, 720,
                            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

  if (window == NULL) {
    ImpLog(LL_Fatal, LC_General, "SDL window creation failed: %s\n",
           SDL_GetError());
    return 1;
  }

  int w, h;
  SDL_GetWindowSize(window, &w, &h);
  ImpLog(LL_Debug, LC_General, "Window size (screen coords): %d x %d\n", w, h);

  glContext = SDL_GL_CreateContext(window);
  if (glContext == NULL) {
    ImpLog(LL_Fatal, LC_General, "OpenGL context creation failed: %s\n",
           SDL_GetError());
    return 1;
  }

  SDL_GL_GetDrawableSize(window, &w, &h);
  ImpLog(LL_Debug, LC_General, "Drawable size (pixels): %d x %d\n", w, h);

  glewExperimental = GL_TRUE;
  GLenum glewErr = glewInit();
  if (glewErr != GLEW_OK) {
    ImpLog(LL_Fatal, LC_General, "GLEW initialisation failed: %s\n",
           glewGetErrorString(glewErr));
    return 1;
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
  SDL_GL_SetSwapInterval(1);

  glViewport(0, 0, w, h);
  glClearColor(0.2f, 0.2f, 0.2f, 0);

  g_Camera.AspectRatio = (float)w / (float)h;
  CameraInit(&g_Camera);

  ShaderInit();
  Character3D::Init();
  characters[0].Load(0);
  characters[0].Submit();
  characters[1].MakePlane();
  characters[1].Submit();

  while (!quit) {
    GameLoop();
  }

  ImpLog(LL_Info, LC_General, "Bye!\n");

  return 0;
}
