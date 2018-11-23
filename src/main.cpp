#include "impacto.h"

#include "log.h"
#include "io/vfs.h"
#include "window.h"
#include "workqueue.h"
#include "game.h"

using namespace Impacto;

static uint64_t t;

void GameLoop() {
  uint64_t t2 = SDL_GetPerformanceCounter();
  float dt = ((float)(t2 - t) / (float)SDL_GetPerformanceFrequency());
  t = t2;

  Game::Update(dt);
  Game::Render();
}

void InitSystems() {
  LogSetConsole(true);
  g_LogLevelConsole = LL_Max;
  g_LogChannelsConsole = LC_All;

  Io::VfsInit();
  WorkQueue::Init();
}

int main(int argc, char* argv[]) {
  InitSystems();

  Game::InitFromProfile("modelviewer");

  Game::InitModelViewer();

  t = SDL_GetPerformanceCounter();

  while (!Game::ShouldQuit) {
    GameLoop();
  }

  ImpLog(LL_Info, LC_General, "Bye!\n");

  Game::Shutdown();

  return 0;
}
