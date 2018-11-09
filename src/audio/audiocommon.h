#pragma once

#include "al.h"
#include "alc.h"
#include "alext.h"

namespace Impacto {
namespace Audio {

enum AudioChannelGroup { ACG_BGM, ACG_SE, ACG_Voice, ACG_Count };
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

enum AudioChannelState {
  ACS_Stopped,
  ACS_Playing,
  ACS_FadingIn,
  ACS_FadingOut
};

class AudioChannel;
class AudioStream;

}  // namespace Audio
}  // namespace Impacto