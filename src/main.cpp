#include "impacto.h"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#ifdef __SWITCH__
#include <switch.h>
#endif

#include "log.h"
#include "window.h"
#include "game.h"

#include "io/physicalfilestream.h"

using namespace Impacto;

static uint64_t t;

void GameLoop() {
  // TODO: Better FPS lock
  uint64_t t2;
  float dt;
  do {
    t2 = SDL_GetPerformanceCounter();
    dt = ((float)(t2 - t) / (float)SDL_GetPerformanceFrequency());
  } while (dt < 0.016f);
  t = t2;

  Game::Update(dt);
  Game::Render();
}

#ifdef EMSCRIPTEN
extern "C" void EMSCRIPTEN_KEEPALIVE StartGame() {
  t = SDL_GetPerformanceCounter();
  emscripten_set_main_loop(GameLoop, -1, 0);
}
#endif

int main(int argc, char* argv[]) {
#ifdef EMSCRIPTEN
  EM_ASM(OnGameLoadStart(););
#endif

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

#ifdef EMSCRIPTEN
  EM_ASM(OnGameLoaded(););
#else
  t = SDL_GetPerformanceCounter();

#ifdef __SWITCH__
  while (appletMainLoop() && !Game::ShouldQuit) {
#else
  while (!Game::ShouldQuit) {
#endif
    GameLoop();
  }

  ImpLog(LL_Info, LC_General, "Bye!\n");

  Game::Shutdown();
#endif

  return 0;
}
