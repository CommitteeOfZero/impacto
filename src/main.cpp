#include "impacto.h"

#include "log.h"
#include "io/vfs.h"
#include "shader.h"
#include "window.h"
#include "workqueue.h"
#include "game.h"

using namespace Impacto;

static uint64_t t;

void GameLoop() {
  uint64_t t2 = SDL_GetPerformanceCounter();
  float dt = ((float)(t2 - t) / (float)SDL_GetPerformanceFrequency());
  t = t2;

  g_Game->Update(dt);
  g_Game->Render();
}

void InitSystems() {
  LogSetConsole(true);
  g_LogLevelConsole = LL_Debug;
  g_LogChannelsConsole = LC_All;

  VfsInit();
  WorkQueueInit();
  ShaderInit();
}

int main(int argc, char* argv[]) {
  InitSystems();

  // Game::InitModelViewer();
  Game::InitVmTest();

  t = SDL_GetPerformanceCounter();

  while (!g_Game->ShouldQuit) {
    GameLoop();
  }

  ImpLog(LL_Info, LC_General, "Bye!\n");

  if (g_Game) delete g_Game;

  return 0;
}
