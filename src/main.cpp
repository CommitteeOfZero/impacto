#include "impacto.h"

#include "log.h"
#include "io/vfs.h"
#include "window.h"
#include "workqueue.h"
#include "game.h"

using namespace Impacto;

static uint64_t t;
static Game* game;

void GameLoop() {
  uint64_t t2 = SDL_GetPerformanceCounter();
  float dt = ((float)(t2 - t) / (float)SDL_GetPerformanceFrequency());
  t = t2;

  game->Update(dt);
  game->Render();
}

void InitSystems() {
  LogSetConsole(true);
  g_LogLevelConsole = LL_Max;
  g_LogChannelsConsole = LC_All;

  VfsInit();
  WorkQueueInit();
}

int main(int argc, char* argv[]) {
  InitSystems();

  game = Game::CreateModelViewer();

  t = SDL_GetPerformanceCounter();

  while (!game->ShouldQuit) {
    GameLoop();
  }

  ImpLog(LL_Info, LC_General, "Bye!\n");

  if (game) delete game;

  return 0;
}
