#include "impacto.h"

#include <glm/glm.hpp>

#include "log.h"
#include "io/vfs.h"
#include "3d/scene.h"
#include "shader.h"
#include "window.h"

using namespace Impacto;

bool quit = false;
static uint64_t t;

void Update(float dt) { g_Scene.Update(dt); }

void Render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  g_Scene.Render();

  SDL_GL_SwapWindow(g_SDLWindow);
}

void GameLoop() {
  uint64_t t2 = SDL_GetPerformanceCounter();
  float dt = ((float)(t2 - t) / (float)SDL_GetPerformanceFrequency());
  t = t2;

  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      quit = true;
    }
  }

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

  return 0;
}
