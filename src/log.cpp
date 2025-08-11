#include <time.h>
#include <cstdarg>

#include <fmt/chrono.h>
#include <SDL_log.h>
#include "log.h"
#include "util.h"
#include "io/physicalfilestream.h"
namespace Impacto {

// TODO Color output in console?

static SDL_LogOutputFunction DefaultLoggingFunction = nullptr;
static std::unique_ptr<Io::Stream> FileLogStream = nullptr;

bool CheckLogConfig(LogLevel level, LogChannel channel) {
  bool any = false;
  if ((LoggingToConsole || LoggingToFile) && (level <= g_LogLevel) &&
      (g_LogChannels & channel) != LogChannel::None) {
    any = true;
  }
  return any;
}

void ConsoleWrite(LogLevel level, std::string_view str) {
  assert(level > LogLevel::Off && level < LogLevel::Max);
  switch (level) {
    case LogLevel::Fatal:
      SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "%s", str.data());
      break;
    case LogLevel::Error:
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", str.data());
      break;
    case LogLevel::Warning:
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s", str.data());
      break;
    case LogLevel::Info:
      SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s", str.data());
      break;
    case LogLevel::Debug:
      SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "%s", str.data());
      break;
    case LogLevel::Trace:
      SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "%s", str.data());
      break;
    default:
      assert(false);
  }
}

void ImpLogImpl(LogLevel level, LogChannel channel, fmt::string_view format,
                fmt::format_args args, size_t tailSize) {
  constexpr size_t maxChannelSize = 16;
  constexpr size_t maxTimestampSize = 21;
  assert(level > LogLevel::Off && channel > LogChannel::None);
  const size_t lineBufferSize =
      maxChannelSize + maxTimestampSize + tailSize + 1;
  auto* line = static_cast<char*>(ImpStackAlloc(lineBufferSize));

  std::string_view channelStr = ChannelToString(channel);
  time_t timestamp = time(nullptr);
  auto tsFormat = fmt::format_to_n(
      line, maxTimestampSize, "[{:%Y-%m-%d %H:%M:%S}]", fmt::gmtime(timestamp));
  auto channelFormat =
      fmt::format_to_n(tsFormat.out, maxChannelSize, "[{}] ", channelStr);
  auto tailFormat =
      fmt::vformat_to_n(channelFormat.out, tailSize + 1, format, args);
  *tailFormat.out = '\0';

  ConsoleWrite(level, line);
}

void LogToFile(void* userdata, [[maybe_unused]] int category,
               SDL_LogPriority priority, const char* message) {
  if (!FileLogStream) return;
  static constexpr const char* SDL_priority_prefixes[] = {
      NULL, "VERBOSE", "DEBUG", "INFO", "WARN", "ERROR", "CRITICAL"};

  std::string logBuf =
      fmt::format("{:s}: {:s}\n", SDL_priority_prefixes[priority], message);
  FileLogStream->Write(logBuf.data(), logBuf.size());
}

void SDLLogger(void* userdata, [[maybe_unused]] int category,
               SDL_LogPriority priority, const char* message) {
  if (LoggingToFile) {
    LogToFile(userdata, category, priority, message);
  }
  if (LoggingToConsole) {
    DefaultLoggingFunction(userdata, category, priority, message);
  }
}

void SetSDLLogger(SDL_LogOutputFunction loggingFunction) {
  [[maybe_unused]] static const bool fetchDefaultLogger = [] {
    if (!DefaultLoggingFunction) {
      SDL_LogGetOutputFunction(&DefaultLoggingFunction, nullptr);
    }
    return true;
  }();

  if (loggingFunction) {
    SDL_LogSetOutputFunction(loggingFunction, nullptr);
  } else {
    SDL_LogSetOutputFunction(DefaultLoggingFunction, nullptr);
  }
}

void LogSetFile(std::string const& path) {
  using CF = Io::PhysicalFileStream::CreateFlagsMode;
  Io::Stream* stream;
  IoError err = Io::PhysicalFileStream::Create(
      path, &stream, CF::CREATE | CF::CREATE_DIRS | CF::WRITE | CF::UNBUFFERED);
  if (err != IoError_OK) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Failed to open save file for writing\n");
    return;
  }
  FileLogStream.reset(stream);
  LoggingToFile = true;
}

void LogSetConsole(bool enabled) { LoggingToConsole = enabled; }

void LogInit() {
  SetSDLLogger(SDLLogger);
  SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_VERBOSE);
}

#ifndef IMPACTO_DISABLE_OPENGL
void GLAPIENTRY LogGLMessageCallback(GLenum source, GLenum type, GLuint id,
                                     GLenum severity, GLsizei length,
                                     const GLchar* message,
                                     const void* userParam) {
  LogLevel level;
  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH_ARB:
      level = LogLevel::Error;
      break;
    case GL_DEBUG_SEVERITY_MEDIUM_ARB:
      level = LogLevel::Warning;
      break;
    case GL_DEBUG_SEVERITY_LOW_ARB:
    default:
      level = LogLevel::Info;
      break;
  }

  const char typeError[] = "Error";
  const char typeDeprecated[] = "DeprecatedBehaviour";
  const char typeUB[] = "UndefinedBehaviour";
  const char typePerf[] = "Performance";
  const char typePortability[] = "Portability";
  const char typeOther[] = "Other";

  const char* typeStr;
  switch (type) {
    case GL_DEBUG_TYPE_ERROR_ARB:
      typeStr = typeError;
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
      typeStr = typeDeprecated;
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
      typeStr = typeUB;
      break;
    case GL_DEBUG_TYPE_PERFORMANCE_ARB:
      typeStr = typePerf;
      break;
    case GL_DEBUG_TYPE_PORTABILITY_ARB:
      typeStr = typePortability;
      break;
    case GL_DEBUG_TYPE_OTHER_ARB:
    default:
      typeStr = typeOther;
      break;
  }

  const char sourceApi[] = "API";
  const char sourceSc[] = "ShaderCompiler";
  const char sourceWin[] = "WindowSystem";
  const char source3rdParty[] = "ThirdParty";
  const char sourceApplication[] = "Application";
  const char sourceOther[] = "Other";

  const char* sourceStr;
  switch (source) {
    case GL_DEBUG_SOURCE_API_ARB:
      sourceStr = sourceApi;
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
      sourceStr = sourceSc;
      break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
      sourceStr = sourceWin;
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
      sourceStr = source3rdParty;
      break;
    case GL_DEBUG_SOURCE_APPLICATION_ARB:
      sourceStr = sourceApplication;
      break;
    case GL_DEBUG_SOURCE_OTHER_ARB:
    default:
      sourceStr = sourceOther;
      break;
  }

  ImpLog(level, LogChannel::GL,
         "type={:s}, source={:s}, id={:d}, message: {:s}\n", typeStr, sourceStr,
         id, message);
}
#endif

}  // namespace Impacto