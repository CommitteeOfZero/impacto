#pragma once

#include "impacto.h"

#include <glad/glad.h>

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
  LC_ModelLoad = (1 << 3),
  LC_GL = (1 << 4),
  LC_Object3D = (1 << 5),
  LC_TextureLoad = (1 << 6),
  LC_Scene = (1 << 7),
  LC_All = 0xFFFFFFFF
};

extern LogLevel g_LogLevelFile;
extern LogLevel g_LogLevelConsole;
extern uint32_t g_LogChannelsFile;
extern uint32_t g_LogChannelsConsole;

void LogSetFile(char* path);
void LogSetConsole(bool enabled);

void ImpLog(LogLevel level, LogChannel channel, const char* format, ...);
#ifdef IMPACTO_ENABLE_SLOW_LOG
#define ImpLogSlow ImpLog
#else
#define ImpLogSlow(...) (void)0
#endif

void GLAPIENTRY LogGLMessageCallback(GLenum source, GLenum type, GLuint id,
                                     GLenum severity, GLsizei length,
                                     const GLchar* message,
                                     const void* userParam);

}  // namespace Impacto