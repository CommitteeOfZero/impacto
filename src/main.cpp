#include "impacto.h"

#include <ranges>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#include "log.h"
#include "game.h"
#include "util.h"

#include "profile/profile.h"
#include "profile/userconfig.h"
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
  dt = std::min(dt, 1.0f);

  Game::Update(dt);
  Game::Render();
}

#ifdef EMSCRIPTEN
extern "C" void EMSCRIPTEN_KEEPALIVE StartGame() {
  t = SDL_GetPerformanceCounter();
  emscripten_set_main_loop(GameLoop, -1, 0);
}
#endif

static void HandleArguments(std::vector<std::string_view> args) {
  bool hasSetChannel = false;
  for (size_t i = 0; i < args.size(); ++i) {
    std::string_view arg = args[i];
    auto handleArgInput = [&](std::ranges::range auto supportedArgs,
                              std::invocable<std::string_view> auto action) {
      if (std::find(std::begin(supportedArgs), std::end(supportedArgs), arg) !=
          std::end(supportedArgs)) {
        if (i++ < args.size()) {
          std::string_view input = args[i];
          action(input);
          return true;
        } else {
          ImpLog(LogLevel::Fatal, LogChannel::General,
                 "Invalid number of arguments");
          exit(1);
        }
      }
      return false;
    };
    using std::literals::string_view_literals::operator""sv;
    constexpr auto make_handler =
        [](std::invocable<std::string_view> auto fn,
           std::convertible_to<std::string_view> auto... strs) {
          return std::pair{std::to_array<std::string_view>({strs...}), fn};
        };

    const auto argHandlers = std::tuple{
        make_handler(
            [&](std::string_view input) { LogSetFile(std::string(input)); },
            "-lf", "--logfile"),
        make_handler(
            [&](std::string_view input) {
              auto inputChannel = StringToChannel(input);
              if (!hasSetChannel) {
                g_LogChannels = {};
                hasSetChannel = true;
              }
              if (inputChannel == LogChannel::None) {
                g_LogChannels = inputChannel;
              } else {
                g_LogChannels |= inputChannel;
              }
            },
            "-lc", "--logchannel"),
        make_handler(
            [&](std::string_view input) { g_LogLevel = StringToLevel(input); },
            "-ll", "--loglevel"),
        make_handler(
            [&](std::string_view input) {
              Profile::UserConfig ::ActiveGame = input;
            },
            "-g", "--game"),
        make_handler(
            [&](std::string_view input) {
              Profile::UserConfig ::PatchOverride = input;
            },
            "-p", "--patch"),
        make_handler(
            [&](std::string_view input) { Profile::BaseConfigPath = input; },
            "-bc", "--baseconfigpath"),
        make_handler(
            [&](std::string_view input) { Profile::UserConfigPath = input; },
            "-uc", "--userconfigpath"),
    };

    std::apply(
        [&](auto&&... h) {
          return ((handleArgInput(h.first, h.second)) || ...);
        },
        argHandlers);
  }
};

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

  std::string profilePath;
  LogInit();
  g_LogChannels = LogChannel::All;
  g_LogLevel = LogLevel::Fatal;
#if __SWITCH__
  LogSetFile("Impacto_Log.txt");
#else
  LogSetConsole(true);
#endif

  std::vector<std::string_view> arguments;
  for (int i = 1; i < argc; ++i) {
    arguments.push_back(argv[i]);
  }

  HandleArguments(arguments);
  if (arguments.empty()) {
    Io::Stream* stream;
    IoError err = Io::PhysicalFileStream::Create("args.txt", &stream);
    if (err == IoError_OK) {
      std::string fileContents(stream->Meta.Size, '\0');
      stream->Read(fileContents.data(), stream->Meta.Size);
      arguments.clear();
      for (auto&& part : std::views::split(fileContents, ' ')) {
        arguments.emplace_back(
            std::string_view(&*part.begin(), std::ranges::distance(part)));
      }
      HandleArguments(arguments);
      delete stream;
    }
  }

#ifdef EMSCRIPTEN
  // Emscripten's EGL requests a window framebuffer with antialiasing by default
  // (as WebGL does)
  // Emscripten's SDL2 port fails to change this even with MSAA set to 0 in the
  // context parameters
  EM_ASM(EGL.antialias = false;);
#endif
  try {
    Profile::Init();
    Profile::Configure();
    Game::Init();

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
  } catch (std::exception const& e) {
    ImpLog(LogLevel::Fatal, LogChannel::General,
           "Fatal error occured: {}, exiting!\n", e.what());
    exit(1);
  }
  return 0;
}
