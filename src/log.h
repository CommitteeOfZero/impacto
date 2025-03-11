#pragma once

#include "impacto.h"

#ifndef IMPACTO_DISABLE_OPENGL
#include <glad/glad.h>
#endif

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
  LC_Renderable3D = (1 << 5),
  LC_TextureLoad = (1 << 6),
  LC_Scene = (1 << 7),
  LC_VM = (1 << 8),
  LC_Expr = (1 << 9),
  LC_VMStub = (1 << 10),
  LC_Audio = (1 << 11),
  LC_Profile = (1 << 12),
  LC_Video = (1 << 13),
  LC_All = 0xFFFFFFFF
};

inline LogLevel g_LogLevelFile = LL_Off;
inline LogLevel g_LogLevelConsole = LL_Off;
inline uint32_t g_LogChannelsFile = 0;
inline uint32_t g_LogChannelsConsole = 0;

void LogSetFile(char* path);
void LogSetConsole(bool enabled);

void ImpLogImpl(LogLevel level, LogChannel channel, fmt::string_view format,
                fmt::format_args args, size_t tailSize);

template <typename... T>
void ImpLog(LogLevel level, LogChannel channel, fmt::format_string<T...> format,
            T&&... args) {
  size_t tailSize = fmt::formatted_size(format, std::forward<T>(args)...);
  ImpLogImpl(level, channel, format.get(), fmt::make_format_args(args...),
             tailSize);
}
#if IMPACTO_ENABLE_SLOW_LOG
#define ImpLogSlow ImpLog
#else
#define ImpLogSlow(...) (void)0
#endif

#ifndef IMPACTO_DISABLE_OPENGL
void GLAPIENTRY LogGLMessageCallback(GLenum source, GLenum type, GLuint id,
                                     GLenum severity, GLsizei length,
                                     const GLchar* message,
                                     const void* userParam);
#endif

}  // namespace Impacto