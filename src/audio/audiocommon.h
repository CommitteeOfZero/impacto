#pragma once

#if IMPACTO_OPENAL_INCLUDE_SUBDIR
#include <AL/al.h>
#include <AL/alc.h>
#else
#include "al.h"
#include "alc.h"
#endif

#if IMPACTO_OPENAL_HAVE_ALEXT
#include "alext.h"
#else
// emscripten has these but not alext.h /shrug
#define AL_FORMAT_MONO_FLOAT32 0x10010
#define AL_FORMAT_STEREO_FLOAT32 0x10011
#endif

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