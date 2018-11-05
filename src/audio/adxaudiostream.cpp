#include "adxaudiostream.h"

#include "../util.h"
#include "../log.h"

#include <algorithm>

namespace Impacto {
namespace Audio {

struct AdxHeaderInfo {
  int ChannelCount;
  int SampleRate;
  int SampleCount;
  bool HasLoop;
  int LoopStart;
  int LoopEnd;

  uint16_t Highpass;
  uint8_t FrameSize;
  int StreamDataOffset;

  int16_t Hist1_L;
  int16_t Hist1_R;
  int16_t Hist2_L;
  int16_t Hist2_R;
};

// Thanks https://github.com/kode54/vgmstream/blob/master/src/util.h

static int nibble_to_int[16] = {0,  1,  2,  3,  4,  5,  6,  7,
                                -8, -7, -6, -5, -4, -3, -2, -1};

static inline int get_high_nibble_signed(uint8_t n) {
  return nibble_to_int[n >> 4];
}

static inline int get_low_nibble_signed(uint8_t n) {
  return nibble_to_int[n & 0xf];
}

static inline int clamp16(int32_t val) {
  if (val > 32767) return 32767;
  if (val < -32768) return -32768;
  return val;
}

void AdxAudioStream::SetCoefficients(double cutoff, double sample_rate) {
  // https://wiki.multimedia.cx/index.php/CRI_ADX_ADPCM#Coefficients

  static int16_t const Coef1_44k_500hz = 0x7298;
  static int16_t const Coef2_44k_500hz = 0x3350;
  if (cutoff == 500.0) {
    if (sample_rate == 44100.0) {
      Coef1 = Coef1_44k_500hz;
      Coef2 = Coef2_44k_500hz;
      return;
    }
  }

  /* temps to keep the calculation simple */
  double z, a, b, c;

  z = cos(2.0 * M_PI * cutoff / sample_rate);

  a = M_SQRT2 - z;
  b = M_SQRT2 - 1.0;
  c = (a - sqrt((a + b) * (a - b))) / b;

  /* compute the coefficients as fixed point values, with 12 fractional bits */
  Coef1 = (int16_t)floor(c * 8192);
  Coef2 = (int16_t)floor(c * c * -4096);
}

// TODO make this compile-time
bool AdxAudioStream::DecodeBuffer() {
  if (ChannelCount == 2) return DecodeBufferStereo();
  return DecodeBufferMono();
}

bool AdxAudioStream::DecodeBufferMono() {
  /* the +1 becomes important on quiet ADXs */
  int scale = SDL_SwapBE16(*(uint16_t*)EncodedBuffer) + 1;

  int16_t* output = DecodedBuffer;
  uint8_t* input = EncodedBuffer + 2;
  for (int i = 0; i < (EncodedBytesPerBuffer - 2); i++) {
    /* this byte contains nibbles for two samples */
    int sample_byte = *input;

    output[0] = clamp16(get_low_nibble_signed(sample_byte) * scale +
                        (Coef1 * Hist1_L >> 12) + (Coef2 * Hist2_L >> 12));
    output[1] =
        clamp16(get_high_nibble_signed(sample_byte) * scale +
                (Coef1 * (int32_t)output[0] >> 12) + (Coef2 * Hist1_L >> 12));
    Hist2_L = output[0];
    Hist1_L = output[1];

    input++;
    output += 2;
  }

  return true;
}

bool AdxAudioStream::DecodeBufferStereo() {
  /* the +1 becomes important on quiet ADXs */
  int scale = SDL_SwapBE16(*(uint16_t*)EncodedBuffer) + 1;

  int16_t* output = DecodedBuffer;
  uint8_t* input = EncodedBuffer + 2;
  for (int i = 0; i < (EncodedBytesPerBuffer - 2) / 2; i++) {
    /* this byte contains nibbles for two samples */
    int sample_byte_L = input[0];
    int sample_byte_R = input[1];

    output[0] = clamp16(get_low_nibble_signed(sample_byte_L) * scale +
                        (Coef1 * Hist1_L >> 12) + (Coef2 * Hist2_L >> 12));
    output[1] = clamp16(get_low_nibble_signed(sample_byte_R) * scale +
                        (Coef1 * Hist1_R >> 12) + (Coef2 * Hist2_R >> 12));
    output[2] =
        clamp16(get_high_nibble_signed(sample_byte_L) * scale +
                (Coef1 * (int32_t)output[0] >> 12) + (Coef2 * Hist1_L >> 12));
    output[3] =
        clamp16(get_high_nibble_signed(sample_byte_R) * scale +
                (Coef1 * (int32_t)output[1] >> 12) + (Coef2 * Hist1_R >> 12));
    Hist2_L = output[0];
    Hist2_R = output[1];
    Hist1_L = output[2];
    Hist1_R = output[3];

    input += 2;
    output += 4;
  }

  return true;
}

static bool ParseAdxHeader(SDL_RWops* stream, AdxHeaderInfo* info) {
  uint8_t header[0x34];
  SDL_RWread(stream, header, 0x34, 1);

  // first magic
  if (SDL_SwapBE16(*(uint16_t*)header) != 0x8000) return false;

  // second magic
  SDL_RWseek(stream, info->StreamDataOffset - 6, RW_SEEK_CUR);
  char const magic[] = "(c)CRI";
  char fileMagic[6];
  SDL_RWread(stream, fileMagic, 6, 1);
  if (memcmp(magic, fileMagic, 6) != 0) {
    return false;
  }

  info->StreamDataOffset = SDL_SwapBE16(*(uint16_t*)(header + 2)) + 4;

  if (header[4] != 3) {
    ImpLog(LL_Error, LC_Audio,
           "Encountered ADX file with unsupported encoding type format %d\n",
           header[4]);
    return false;
  }

  info->FrameSize = header[5];

  if (header[6] != 4) {
    ImpLog(LL_Error, LC_Audio,
           "Encountered ADX file with unknown bits per sample %d\n", header[6]);
    return false;
  }

  info->ChannelCount = header[7];
  if (info->ChannelCount != 1 && info->ChannelCount != 2) {
    ImpLog(LL_Error, LC_Audio,
           "Encountered ADX file with unsupported channel count %d\n",
           info->ChannelCount);
    return false;
  }

  info->SampleRate = SDL_SwapBE32(*(uint32_t*)(header + 8));

  info->SampleCount = SDL_SwapBE32(*(uint32_t*)(header + 12));

  info->Highpass = SDL_SwapBE16(*(uint16_t*)(header + 16));

  if (header[18] != 4) {
    ImpLog(LL_Error, LC_Audio,
           "Encountered ADX file with unsupported header version %d\n",
           header[18]);
    return false;
  }

  if (header[19] != 0) {
    ImpLog(LL_Error, LC_Audio, "Encountered encrypted ADX file, type %d\n",
           header[19]);
    return false;
  }

  info->Hist1_L = SDL_SwapBE16(*(uint16_t*)(header + 0x18));
  info->Hist2_L = SDL_SwapBE16(*(uint16_t*)(header + 0x1A));
  info->Hist1_R = SDL_SwapBE16(*(uint16_t*)(header + 0x1C));
  info->Hist2_R = SDL_SwapBE16(*(uint16_t*)(header + 0x1E));

  // TODO loop support
  info->HasLoop = false;
}

bool AudioIsAdx(SDL_RWops* stream) {
  AdxHeaderInfo info = {0};
  bool result = ParseAdxHeader(stream, &info);
  SDL_RWseek(stream, 0, RW_SEEK_SET);
  return result;
}
AdxAudioStream::AdxAudioStream(SDL_RWops* stream) : AudioStream(stream) {
  AdxHeaderInfo info = {0};
  if (!ParseAdxHeader(stream, &info)) return;

  ChannelCount = info.ChannelCount;
  SampleRate = info.SampleRate;
  Duration = info.SampleCount;
  EncodedBytesPerBuffer = info.FrameSize;
  LoopStart = info.HasLoop ? info.LoopStart : 0;
  LoopEnd = info.HasLoop ? info.LoopEnd : Duration;
  StreamDataOffset = info.StreamDataOffset;
  Hist1_L = info.Hist1_L;
  Hist1_R = info.Hist1_R;
  Hist2_L = info.Hist2_L;
  Hist2_R = info.Hist2_R;
  SamplesPerBuffer = ((EncodedBytesPerBuffer - 2) / 2) / ChannelCount;
  SetCoefficients(info.Highpass, SampleRate);

  BitDepth = 16;
}
AdxAudioStream::~AdxAudioStream() {}

int AdxAudioStream::Read(void* buffer, int samples) {
  return ReadBuffered((int16_t*)buffer, samples);
}

void AdxAudioStream::Seek(int samples) { SeekBuffered(samples); }

}  // namespace Audio
}  // namespace Impacto