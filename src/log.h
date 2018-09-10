#pragma once

#include "impacto.h"

namespace Impacto {

enum LogLevel {
  LL_Off = 0,
  LL_Fatal = 1,
  LL_Error = 2,
  LL_Warning = 3,
  LL_Info = 4,
  LL_Debug = 5,
  LL_Trace = 6,
  LL_Max
};

enum LogChannel : uint32_t {
  LC_General = (1 << 0),
  LC_IO = (1 << 1),
  LC_Render = (1 << 2),

  LC_All = 0xFFFFFFFF
};

extern LogLevel LogLevelFile;
extern LogLevel LogLevelConsole;
extern uint32_t LogChannelsFile;
extern uint32_t LogChannelsConsole;

void LogSetFile(char* path);
void LogSetConsole(bool enabled);

void ImpLog(LogLevel level, LogChannel channel, const char* format, ...);
#ifdef IMPACTO_ENABLE_SLOW_LOG
#define ImpLogSlow ImpLog
#else
#define ImpLogSlow(...) (void)0
#endif

}  // namespace Impacto