#include "impacto.h"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#include "log.h"
#include "window.h"
#include "game.h"

#include "io/physicalfilestream.h"

using namespace Impacto;

static uint64_t t;

void GameLoop() {
  uint64_t t2 = SDL_GetPerformanceCounter();
  float dt = ((float)(t2 - t) / (float)SDL_GetPerformanceFrequency());
  t = t2;

  Game::Update(dt);
  Game::Render();
}

int main(int argc, char* argv[]) {
  LogSetConsole(true);
  g_LogLevelConsole = LL_Max;
  g_LogChannelsConsole = LC_All;

  Io::InputStream* stream;
  IoError err = Io::PhysicalFileStream::Create("profile.txt", &stream);
  if (err != IoError_OK) {
    ImpLog(LL_Fatal, LC_General, "Couldn't open profile.txt\n");
    Window::Shutdown();
  }

  std::string profileName;
  profileName.resize(stream->Meta.Size, '\0');
  profileName.resize(stream->Read(&profileName[0], stream->Meta.Size));

#ifdef EMSCRIPTEN
  // Emscripten's EGL requests a window framebuffer with antialiasing by default
  // (as WebGL does)
  // Emscripten's SDL2 port fails to change this even with MSAA set to 0 in the
  // context parameters
  EM_ASM(EGL.antialias = false;);
#endif

  Game::InitFromProfile(profileName);

  t = SDL_GetPerformanceCounter();

#ifdef EMSCRIPTEN
  emscripten_set_main_loop(GameLoop, -1, 1);
#else
  while (!Game::ShouldQuit) {
    GameLoop();
  }

  ImpLog(LL_Info, LC_General, "Bye!\n");

  Game::Shutdown();
#endif

  return 0;
}
