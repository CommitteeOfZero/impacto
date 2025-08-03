#pragma once

#include <AL/al.h>
#include <AL/alc.h>

#if IMPACTO_OPENAL_HAVE_ALEXT
#include "alext.h"
#else
// emscripten has these but not alext.h /shrug
#define AL_FORMAT_MONO_FLOAT32 0x10010
#define AL_FORMAT_STEREO_FLOAT32 0x10011
#endif

#include "../audiocommon.h"
