#include "impacto.h"

#include "../vendor/nuklear/nuklear_sdl_gl3.h"

#include <glm/glm.hpp>

#include "log.h"
#include "io/vfs.h"
#include "3d/scene.h"
#include "shader.h"
#include "window.h"

using namespace Impacto;

bool quit = false;
static uint64_t t;

int const NkMaxVertexMemory = 1024 * 1024;
int const NkMaxElementMemory = 1024 * 1024;

// TODO: We probably want to turn off Nuklear entirely in full-game release
// builds

void Update(float dt) { g_Scene.Update(dt); }

void Render() {
  SDL_GetWindowSize(g_SDLWindow, &g_WindowWidth, &g_WindowHeight);
  glViewport(0, 0, g_WindowWidth, g_WindowHeight);

  glClearColor(0.2f, 0.2f, 0.2f, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  g_Scene.Render();

#ifdef IMPACTO_GL_DEBUG
  // Nuklear spams these
  glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
                           GL_FALSE);
#endif
  nk_sdl_render(NK_ANTI_ALIASING_OFF, NkMaxVertexMemory, NkMaxElementMemory);
#ifdef IMPACTO_GL_DEBUG
  glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
                           GL_TRUE);
#endif

  SDL_GL_SwapWindow(g_SDLWindow);
}

void GameLoop() {
  uint64_t t2 = SDL_GetPerformanceCounter();
  float dt = ((float)(t2 - t) / (float)SDL_GetPerformanceFrequency());
  t = t2;

  SDL_Event e;
  nk_input_begin(g_Nk);
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      quit = true;
      break;
    }

    nk_sdl_handle_event(&e);
  }
  nk_input_end(g_Nk);

  Update(dt);
  Render();
}

void InitSystems() {
  LogSetConsole(true);
  g_LogLevelConsole = LL_Max;
  g_LogChannelsConsole = LC_All;

  VfsInit();
  WindowInit();
  ShaderInit();
  SceneInit();
}

int main(int argc, char* argv[]) {
  InitSystems();

  t = SDL_GetPerformanceCounter();

  while (!quit) {
    GameLoop();
  }

  ImpLog(LL_Info, LC_General, "Bye!\n");

  WindowShutdown();

  return 0;
}
