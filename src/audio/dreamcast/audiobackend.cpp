#include "audiobackend.h"

#include "../../log.h"

#include <kos.h>
#include <oggvorbis/sndoggvorbis.h>

namespace Impacto {
namespace Audio {
namespace Dreamcast {

bool AudioBackend::Init() {
  snd_stream_init();
  sndoggvorbis_init();
  return true;
}

void AudioBackend::Shutdown() {
  sndoggvorbis_stop();
  sndoggvorbis_shutdown();
  snd_stream_shutdown();
}

};  // namespace Dreamcast
};  // namespace Audio
};  // namespace Impacto
