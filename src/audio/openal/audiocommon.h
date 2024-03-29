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

#include "../audiocommon.h"
