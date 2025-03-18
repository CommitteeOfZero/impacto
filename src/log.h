#pragma once

#include "impacto.h"
#include "util.h"

#ifndef IMPACTO_DISABLE_OPENGL
#include <glad/glad.h>
#endif

namespace Impacto {
enum class LogLevel {
  Off = 0,
  Fatal = 1,
  Error = 2,
  Warning = 3,
  Info = 4,
  Debug = 5,
  Trace = 6,
  Max
};

enum class LogChannel : uint32_t {
  None = 0,
  General = (1 << 0),
  IO = (1 << 1),
  Render = (1 << 2),
  ModelLoad = (1 << 3),
  GL = (1 << 4),
  Renderable3D = (1 << 5),
  TextureLoad = (1 << 6),
  Scene = (1 << 7),
  VM = (1 << 8),
  Expr = (1 << 9),
  VMStub = (1 << 10),
  Audio = (1 << 11),
  Profile = (1 << 12),
  Video = (1 << 13),
  All = 0xFFFFFFFF
};

constexpr LogChannel operator~(LogChannel channel) {
  return static_cast<LogChannel>(~to_underlying(channel));
}
constexpr LogChannel operator|(LogChannel channel, LogChannel other) {
  return static_cast<LogChannel>(to_underlying(channel) | to_underlying(other));
}
constexpr LogChannel operator&(LogChannel channel, LogChannel other) {
  return static_cast<LogChannel>(to_underlying(channel) & to_underlying(other));
}
constexpr LogChannel operator^(LogChannel channel, LogChannel other) {
  return static_cast<LogChannel>(to_underlying(channel) ^ to_underlying(other));
}
constexpr LogChannel& operator|=(LogChannel& channel, LogChannel other) {
  channel = channel | other;
  return channel;
}
constexpr LogChannel& operator&=(LogChannel& channel, LogChannel other) {
  channel = channel & other;
  return channel;
}
constexpr LogChannel& operator^=(LogChannel& channel, LogChannel other) {
  channel = channel ^ other;
  return channel;
}

constexpr auto ChannelToString(LogChannel channel) {
  using enum LogChannel;
  switch (channel) {
    case General:
      return "General";
    case IO:
      return "IO";
    case Render:
      return "Render";
    case ModelLoad:
      return "ModelLoad";
    case GL:
      return "GL";
    case Renderable3D:
      return "Renderable3D";
    case TextureLoad:
      return "TextureLoad";
    case Scene:
      return "Scene";
    case VM:
      return "VM";
    case VMStub:
      return "VMStub";
    case Expr:
      return "Expr";
    case Audio:
      return "Audio";
    case Profile:
      return "Profile";
    case Video:
      return "Video";
    default:
      assert(false);
      return "";
  }
}

constexpr auto LevelToString(LogLevel level) {
  using enum LogLevel;
  switch (level) {
    case Off:
      return "Off";
    case Fatal:
      return "Fatal";
    case Error:
      return "Error";
    case Warning:
      return "Warning";
    case Info:
      return "Info";
    case Debug:
      return "Debug";
    case Trace:
      return "Trace";
    default:
      assert(false);
      return "";
  }
}

inline LogLevel g_LogLevelFile = LogLevel::Off;
inline LogLevel g_LogLevelConsole = LogLevel::Off;
inline LogChannel g_LogChannelsFile = LogChannel::None;
inline LogChannel g_LogChannelsConsole = LogChannel::None;

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