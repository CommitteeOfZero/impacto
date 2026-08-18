#include "impacto.h"

#include <ranges>
#include <fmt/format.h>
#include <fmt/ranges.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#include "log.h"
#include "game.h"
#include "util.h"

#include "profile/profile.h"
#include "userconfig.h"
#include "io/physicalfilestream.h"

using namespace Impacto;

static uint64_t t;
static float TickDeltaTime() {
  // TODO: Better FPS lock
  uint64_t t2 = SDL_GetPerformanceCounter();
  float dt = (float)(t2 - t) / (float)SDL_GetPerformanceFrequency();
  t = t2;
  return std::min(dt, 1.0f);
}

void GameLoop() {
  float dt = TickDeltaTime();
  Game::Update(dt);
  Game::Render();
}

void LauncherLoop() {
  float dt = TickDeltaTime();
  Game::LauncherUpdate(dt);
  Game::LauncherRender();
}

#ifdef EMSCRIPTEN
extern "C" void EMSCRIPTEN_KEEPALIVE StartGame() {
  t = SDL_GetPerformanceCounter();
  emscripten_set_main_loop(GameLoop, -1, 0);
}
#endif

template <typename T>
concept is_arg_handler =
    std::invocable<T> || std::invocable<T, std::string_view>;

static void HandleArguments(std::vector<std::string_view> args) {
  for (size_t i = 0; i < args.size(); ++i) {
    std::string_view arg = args[i];

    auto handleArgInput = [&](std::ranges::range auto supportedArgs,
                              is_arg_handler auto action) {
      if (std::find(std::begin(supportedArgs), std::end(supportedArgs), arg) !=
          std::end(supportedArgs)) {
        if constexpr (std::invocable<decltype(action), std::string_view>) {
          if (++i >= args.size()) {
            ImpLog(LogLevel::Fatal, LogChannel::General,
                   "Invalid number of arguments");
            exit(1);
          }
          std::string_view input = args[i];
          if (input[0] == '-') {
            ImpLog(LogLevel::Fatal, LogChannel::General,
                   "Missing parameter for {}", arg);
            exit(1);
          }
          action(input);
        } else if constexpr (std::invocable<decltype(action)>) {
          action();
        }
        return true;
      }
      return false;
    };
    using std::literals::string_view_literals::operator""sv;
    constexpr auto make_handler =
        [](is_arg_handler auto fn,
           std::convertible_to<std::string_view> auto... strs) {
          return std::pair{std::to_array<std::string_view>({strs...}), fn};
        };

    const auto argHandlers = std::tuple{
        make_handler(
            [&](std::string_view input) {
              Impacto::UserConfig::CommonSettings.LogFile = std::string(input);
              Impacto::UserConfig::CommonSettings.LoggingToFile = true;
              LogInitFile();
            },
            "-lf", "--logfile"),
        make_handler(
            [&](std::string_view input) {
              std::optional<Impacto::LogChannel> logChannelOpt =
                  StringToChannel(input);
              if (!logChannelOpt) {
                ImpLog(LogLevel::Fatal, LogChannel::General,
                       "Invalid log channel \"{}\", expected one of {}, All!\n",
                       input,
                       fmt::join(magic_enum::enum_names<Impacto::LogChannel>(),
                                 ", "));
                exit(1);
              };
              auto& logChannels =
                  Impacto::UserConfig::CommonSettings.LogChannels;
              if (!Impacto::UserConfig::OverrideLogChannels) {
                logChannels = {};
                Impacto::UserConfig::OverrideLogChannels = true;
              }
              if (*logChannelOpt == LogChannel::None) {
                logChannels = *logChannelOpt;
              } else {
                logChannels |= *logChannelOpt;
              }
            },
            "-lc", "--logchannel"),
        make_handler(
            [&](std::string_view input) {
              auto logLevelOpt =
                  magic_enum::enum_cast<Impacto::LogLevel>(input);
              if (!logLevelOpt) {
                ImpLog(LogLevel::Fatal, LogChannel::General,
                       "Invalid log level \"{}\", expected one of {}!\n", input,
                       magic_enum::enum_names<Impacto::LogLevel>());
                exit(1);
              }
              Impacto::UserConfig::CommonSettings.LogLvl = *logLevelOpt;
            },
            "-ll", "--loglevel"),

        make_handler(
            [&](std::string_view input) {
              UserConfig ::ActiveGameOverride = input;
            },
            "-g", "--game"),
        make_handler(
            [&](std::string_view input) {
              UserConfig::PatchProfileOverride = input;
            },
            "-p", "--patch"),
        make_handler(
            [&](std::string_view input) { Profile::BasePathsPath = input; },
            "-bp", "--basepathspath"),
        make_handler(
            [&](std::string_view input) {
              Profile::GameDefinitionsPath = input;
            },
            "-gc", "--gamedefinitionspath"),
        make_handler(
            [&](std::string_view input) { UserConfig::UserConfigPath = input; },
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

  if (char* const basePath = SDL_GetBasePath()) {
    std::filesystem::current_path(basePath);
    SDL_free(basePath);
  }

  std::string profilePath;
  LogInit();

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

    while (!Game::ShouldQuit && UserConfig::GetActiveGame().empty()) {
      LauncherLoop();
    }

    if (!Game::ShouldQuit) {
      Game::InitGameProfile();
    }
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
  } catch (...) {
    ImpLog(LogLevel::Fatal, LogChannel::General,
           "Unknown error occured, exiting!\n");
    exit(1);
  }
  return 0;
}
