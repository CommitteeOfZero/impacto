#include "impacto.h"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#include "log.h"
#include "game.h"
#include "util.h"

#include "io/physicalfilestream.h"

using namespace Impacto;

static uint64_t t;

void GameLoop() {
  // TODO: Better FPS lock
  uint64_t t2;
  float dt;
  t2 = SDL_GetPerformanceCounter();
  dt = ((float)(t2 - t) / (float)SDL_GetPerformanceFrequency());
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
#ifdef _WIN32
  if (AttachConsole(ATTACH_PARENT_PROCESS)) {
    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONIN$", "r", stdin);
  }
#endif

  std::string profileName;
  LogSetConsole(true);
  g_LogLevelConsole = LogLevel::Fatal;
  g_LogChannelsConsole = LogChannel::All;
  for (int i = 1; i < argc; ++i) {
    std::string_view arg = argv[i];
    if (arg == "-lc" || arg == "--logchannel") {
      if (i++ < argc) {
        std::string_view input = argv[i];
        g_LogChannelsConsole = StringToChannel(input);
      } else {
        ImpLog(LogLevel::Fatal, LogChannel::General,
               "Invalid number of arguments");
        exit(1);
      }
    } else if (arg == "-ll" || arg == "--loglevel") {
      if (i++ < argc) {
        std::string_view input = argv[i];
        g_LogLevelConsole = StringToLevel(input);
      } else {
        ImpLog(LogLevel::Fatal, LogChannel::General,
               "Invalid number of arguments");
        exit(1);
      }
    } else {
      profileName = arg;
    }
  }
  if (profileName.empty()) {
    Io::Stream* stream;
    IoError err = Io::PhysicalFileStream::Create("profile.txt", &stream);
    if (err != IoError_OK) {
      ImpLog(LogLevel::Fatal, LogChannel::General,
             "Couldn't open profile.txt\n");
      exit(1);
    }
    profileName.resize(stream->Meta.Size, '\0');
    profileName.resize(stream->Read(&profileName[0], stream->Meta.Size));
  }

  TrimString(profileName);
  MakeLowerCase(profileName);

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

  while (!Game::ShouldQuit) {
    GameLoop();
  }

  ImpLog(LogLevel::Info, LogChannel::General, "Bye!\n");

  Game::Shutdown();
#endif

  return 0;
}
