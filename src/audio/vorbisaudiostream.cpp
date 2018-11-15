#include "vorbisaudiostream.h"
#include "../log.h"
#include <algorithm>

using namespace Impacto::Io;

namespace Impacto {
namespace Audio {

static size_t OvRwRead(void* ptr, size_t size, size_t nmemb, void* datasource) {
  return ((InputStream*)datasource)->Read(ptr, size * nmemb) / size;
}
static int OvRwSeek(void* datasource, ogg_int64_t offset, int whence) {
  return ((InputStream*)datasource)->Seek(offset, whence);
}
static long OvRwTell(void* datasource) {
  return ((InputStream*)datasource)->Position;
}
ov_callbacks OvRwCallbacks = {OvRwRead, OvRwSeek, NULL, OvRwTell};

AudioStream* VorbisAudioStream::Create(InputStream* stream) {
  VorbisAudioStream* result = 0;
  OggVorbis_File Vf;
  bool VfOpen = false;

  int err = ov_open_callbacks(stream, &Vf, NULL, 0, OvRwCallbacks);
  if (err < 0) {
    goto fail;
  }
  VfOpen = true;
  ImpLog(LL_Info, LC_Audio, "Creating Vorbis stream\n");

  vorbis_info* info = ov_info(&Vf, -1);
  if (info->channels != 1 && info->channels != 2) {
    ImpLog(LL_Error, LC_Audio,
           "Got Vorbis file with unsupported channel count %d\n",
           info->channels);
    goto fail;
  }

  result = new VorbisAudioStream;
  result->BaseStream = stream;
  memcpy(&result->Vf, &Vf, sizeof(Vf));
  result->VfOpen = true;

  return result;

fail:
  if (VfOpen) {
    ov_clear(&Vf);
  }
  if (result) {
    result->VfOpen = false;
    result->BaseStream = 0;
    delete result;
  }
  stream->Seek(0, RW_SEEK_SET);
  return 0;
}

void VorbisAudioStream::InitWithInfo(vorbis_info* info) {
  Duration = ov_pcm_total(&Vf, -1);
  LoopEnd = Duration;
  ChannelCount = info->channels;

  vorbis_comment* comments = ov_comment(&Vf, -1);
  int looplength = 0;
  for (int i = 0; i < comments->comments; i++) {
    if (strncmp(comments->user_comments[i], "LOOPSTART",
                std::min(comments->comment_lengths[i],
                         (int)strlen("LOOPSTART"))) == 0) {
      LoopStart = atoi(comments->user_comments[i] + strlen("LOOPSTART") + 1);
    }
    /*
    // Not actually implemented in the reference...
    if (strncmp(comments->user_comments[i], "LOOPLENGTH",
                std::min(comments->comment_lengths[i],
                         (int)strlen("LOOPLENGTH"))) == 0) {
      looplength = atoi(comments->user_comments[i] + strlen("LOOPLENGTH") + 1);
    }
    */
  }
  if (looplength > 0) {
    LoopEnd = LoopStart + looplength;
  }

  SampleRate = info->rate;
  BitDepth = 16;
}

VorbisAudioStream::~VorbisAudioStream() {
  if (VfOpen) {
    ov_clear(&Vf);
  }
}

int VorbisAudioStream::Read(void* buffer, int samples) {
  int current_section;
  int totalSamplesRead = 0;
  while (samples > 0) {
    long ret = ov_read(
        &Vf, (char*)buffer, samples * (BitDepth / 8) * ChannelCount,
        (SDL_BYTEORDER == SDL_BIG_ENDIAN), (BitDepth / 8), 1, &current_section);
    if (ret <= 0) {
      if (ret < 0) {
        ImpLog(LL_Error, LC_Audio,
               "ov_read %d samples at %d failed with code %d\n", samples,
               ReadPosition, ret);
      }
      // else ret == 0 => eof
      break;
    } else {
      buffer = (char*)buffer + ret;
      int samplesRead = ret / ((BitDepth / 8) * ChannelCount);
      totalSamplesRead += samplesRead;
      samples -= samplesRead;
    }
  }
  ReadPosition += totalSamplesRead;
  return totalSamplesRead;
}

void VorbisAudioStream::Seek(int samples) {
  ov_pcm_seek(&Vf, samples);
  ReadPosition = ov_pcm_tell(&Vf);
}

bool VorbisAudioStream::_registered =
    AudioStream::AddAudioStreamCreator(&VorbisAudioStream::Create);

}  // namespace Audio
}  // namespace Impacto