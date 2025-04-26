#pragma once

#include "../util.h"
namespace Impacto {
namespace Audio {

enum AudioChannelGroup { ACG_BGM, ACG_SE, ACG_Voice, ACG_Movie, ACG_Count };
enum AudioChannelId {
  AC_SE0 = 0,
  AC_SE1,
  AC_SE2,
  AC_VOICE0,
  AC_VOICE1,
  AC_VOICE2,
  AC_REV,  // backlog
  AC_BGM0,
  AC_BGM1,
  AC_BGM2,
  AC_SSE,  // "system sound effect"
  AC_Count
};

constexpr inline auto format_as(AudioChannelId channel) -> int {
  return to_underlying(channel);
}

enum AudioChannelState {
  ACS_Stopped,
  ACS_Playing,
  ACS_Paused,
  ACS_FadingIn,
  ACS_FadingOut
};

constexpr inline auto format_as(AudioChannelState state) -> std::string_view {
  switch (state) {
    case ACS_Stopped:
      return "ACS_Stopped";
    case ACS_Playing:
      return "ACS_Playing";
    case ACS_Paused:
      return "ACS_Paused";
    case ACS_FadingIn:
      return "ACS_FadingIn";
    case ACS_FadingOut:
      return "ACS_FadingOut";
  }
  return "Unknown";
}

}  // namespace Audio
}  // namespace Impacto