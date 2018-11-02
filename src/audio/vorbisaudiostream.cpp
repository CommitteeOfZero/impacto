#include "vorbisaudiostream.h"
#include "../log.h"
#include <algorithm>

namespace Impacto {
namespace Audio {

static size_t OvRwRead(void* ptr, size_t size, size_t nmemb, void* datasource) {
  return SDL_RWread((SDL_RWops*)datasource, ptr, size, nmemb);
}
static int OvRwSeek(void* datasource, ogg_int64_t offset, int whence) {
  return SDL_RWseek((SDL_RWops*)datasource, offset, whence);
}
static long OvRwTell(void* datasource) {
  return SDL_RWtell((SDL_RWops*)datasource);
}
ov_callbacks OvRwCallbacks = {OvRwRead, OvRwSeek, NULL, OvRwTell};

bool AudioIsVorbis(SDL_RWops* stream) {
  OggVorbis_File vf;
  bool result = ov_test_callbacks(stream, &vf, NULL, 0, OvRwCallbacks) == 0;
  SDL_RWseek(stream, 0, RW_SEEK_SET);
  return result;
}

VorbisAudioStream::VorbisAudioStream(SDL_RWops* stream) : AudioStream(stream) {
  ImpLog(LL_Info, LC_Audio, "Creating Vorbis stream\n");
  int err = ov_open_callbacks(BaseStream, &Vf, NULL, 0, OvRwCallbacks);
  if (err < 0) {
    ImpLog(LL_Error, LC_Audio, "ov_open_callbacks failed with code %d\n", err);
    return;
  }
  VfOpen = true;

  Duration = ov_pcm_total(&Vf, -1);
  LoopEnd = Duration;

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

  vorbis_info* info = ov_info(&Vf, -1);
  ChannelCount = info->channels;
  assert(ChannelCount == 1 || ChannelCount == 2);
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

}  // namespace Audio
}  // namespace Impacto