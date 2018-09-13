#include <time.h>
#include <stdarg.h>

#include "log.h"
#include "util.h"

namespace Impacto {

// TODO Color output in console?

LogLevel LogLevelFile = LL_Off;
LogLevel LogLevelConsole = LL_Off;
uint32_t LogChannelsFile = 0;
uint32_t LogChannelsConsole = 0;

bool LoggingToConsole = false;
bool LoggingToFile = false;

char* Timestamp() {
  time_t timestamp = time(NULL);
  tm* ptm = gmtime(&timestamp);

  char* buffer = (char*)malloc(20);
  strftime(buffer, 20, "%F %T", ptm);
  return buffer;
}

const char* LevelToString(LogLevel level) {
  assert(level > LL_Off && level < LL_Max);
  switch (level) {
    case LL_Fatal:
      return "Fatal";
    case LL_Error:
      return "Error";
    case LL_Warning:
      return "Warning";
    case LL_Info:
      return "Info";
    case LL_Debug:
      return "Debug";
    case LL_Trace:
      return "Trace";
    default:
      assert(false);
      return "";
  }
}

const char* ChannelToString(LogChannel channel) {
  switch (channel) {
    case LC_General:
      return "General";
    case LC_IO:
      return "IO";
    case LC_Render:
      return "Render";
    case LC_ModelLoad:
      return "ModelLoad";
    case LC_GL:
      return "GL";
    case LC_Character3D:
      return "Character3D";
    case LC_TextureLoad:
      return "TextureLoad";
    default:
      assert(false);
      return "";
  }
}

void ConsoleWrite(const char* str) { printf("%s", str); }

void ImpLog(LogLevel level, LogChannel channel, const char* format, ...) {
  assert(level > 0 && channel > 0);
  bool any = false;
  if (LoggingToConsole && level <= LogLevelConsole &&
      (LogChannelsConsole & channel)) {
    any = true;
  }
  if (LoggingToFile && level <= LogLevelFile && (LogChannelsFile & channel)) {
    any = true;
  }
  if (!any) return;

  char* ts = Timestamp();

  const char* levelStr = LevelToString(level);
  const char* channelStr = ChannelToString(channel);

  size_t headSize =
      snprintf(NULL, 0, "[%s][%s][%s] ", ts, levelStr, channelStr);

  va_list args;
  va_start(args, format);
  size_t tailSize = vsnprintf(NULL, 0, format, args);

  char* line = (char*)ImpStackAlloc(headSize + tailSize + 1);

  snprintf(line, headSize + 1, "[%s][%s][%s] ", ts, levelStr, channelStr);
  vsnprintf(line + headSize, tailSize + 1, format, args);
  line[headSize + tailSize] = '\0';

  if (LoggingToConsole && level <= LogLevelConsole &&
      (LogChannelsConsole & channel)) {
    ConsoleWrite(line);
  }
  // TODO file

  va_end(args);
  free(ts);
  ImpStackFree(line);
}

void LogSetFile(char* path) {
  assert(false);  // TODO
}

void LogSetConsole(bool enabled) { LoggingToConsole = enabled; }

void GLAPIENTRY LogGLMessageCallback(GLenum source, GLenum type, GLuint id,
                                     GLenum severity, GLsizei length,
                                     const GLchar* message,
                                     const void* userParam) {
  LogLevel level;
  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH_ARB:
      level = LL_Error;
      break;
    case GL_DEBUG_SEVERITY_MEDIUM_ARB:
      level = LL_Warning;
      break;
    case GL_DEBUG_SEVERITY_LOW_ARB:
    default:
      level = LL_Info;
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

  ImpLog(level, LC_GL, "type=%s, source=%s, id=%d, message: %s\n", typeStr,
         sourceStr, id, message);
}

}  // namespace Impacto