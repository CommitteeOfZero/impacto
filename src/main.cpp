#include "impacto.h"

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <glm/glm.hpp>

#include "log.h"
#include "camera.h"

using namespace Impacto;

SDL_Window* window = NULL;
bool quit = false;
SDL_GLContext glContext;

void GameLoop() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      quit = true;
    }
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  SDL_GL_SwapWindow(window);
}

int main(int argc, char* argv[]) {
  LogSetConsole(true);
  LogLevelConsole = LL_Max;
  LogChannelsConsole = LC_All;

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

  // TODO DPI aware

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

  // Vsync
  SDL_GL_SetSwapInterval(1);

  glViewport(0, 0, w, h);
  glClearColor(0, 0, 0, 0);

  g_Camera.AspectRatio = (float)w / (float)h;
  CameraInit(&g_Camera);

  while (!quit) {
    GameLoop();
  }

  ImpLog(LL_Info, LC_General, "Bye!\n");

  return 0;
}
