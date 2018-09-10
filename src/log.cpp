#include <time.h>
#include <stdarg.h>

#include "log.h"

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

}  // namespace Impacto