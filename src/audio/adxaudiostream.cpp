#include "adxaudiostream.h"

#include "../util.h"
#include "../log.h"

#include <algorithm>

using namespace Impacto::Io;

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

// https://github.com/kode54/vgmstream/blob/master/src/coding/adx_decoder.c
bool AdxAudioStream::DecodeBuffer() {
  int16_t* output = DecodedBuffer;
  uint8_t* input = EncodedBuffer;

  for (int c = 0; c < ChannelCount; c++) {
    /* the +1 becomes important on quiet ADXs */
    int scale = SDL_SwapBE16(*(uint16_t*)input) + 1;
    input += 2;

    for (int i = 0; i < SamplesPerBuffer; i++) {
      /* this byte contains nibbles for two samples */
      int sample_byte = input[i / 2];

      output[i * ChannelCount + c] =
          clamp16((i & 1 ? get_low_nibble_signed(sample_byte)
                         : get_high_nibble_signed(sample_byte)) *
                      scale +
                  (Coef1 * Hist1[c] >> 12) + (Coef2 * Hist2[c] >> 12));

      Hist2[c] = Hist1[c];
      Hist1[c] = output[i * ChannelCount + c];
    }
    input += SamplesPerBuffer / 2;
  }

  return true;
}

static bool ParseAdxHeader(Stream* stream, AdxHeaderInfo* info) {
  uint8_t header[0x34];
  stream->Read(header, 0x34);

  // first magic
  if (SDL_SwapBE16(*(uint16_t*)header) != 0x8000) return false;

  info->StreamDataOffset = SDL_SwapBE16(*(uint16_t*)(header + 2)) + 4;

  // second magic
  stream->Seek(info->StreamDataOffset - 6, RW_SEEK_SET);
  char const magic[] = "(c)CRI";
  char fileMagic[6];
  stream->Read(fileMagic, 6);
  if (memcmp(magic, fileMagic, 6) != 0) {
    return false;
  }

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

  int HistOffset = 0x18;
  int HistSize = std::max(8, 4 * info->ChannelCount);

  info->Hist1_L = SDL_SwapBE16(*(uint16_t*)(header + HistOffset));
  info->Hist2_L = SDL_SwapBE16(*(uint16_t*)(header + HistOffset + 2));
  info->Hist1_R = SDL_SwapBE16(*(uint16_t*)(header + HistOffset + 4));
  info->Hist2_R = SDL_SwapBE16(*(uint16_t*)(header + HistOffset + 6));

  int LoopOffset = HistOffset + HistSize;
  int LoopSize = 0x18;
  if (LoopOffset + LoopSize <= info->StreamDataOffset) {
    info->HasLoop = true;
    info->LoopStart = SDL_SwapBE32(*(uint32_t*)(header + LoopOffset + 8));
    info->LoopEnd = SDL_SwapBE32(*(uint32_t*)(header + LoopOffset + 16));
  } else {
    info->HasLoop = false;
  }

  return true;
}

AudioStream* AdxAudioStream::Create(Stream* stream) {
  AdxAudioStream* result = 0;

  AdxHeaderInfo info = {0, 0, 0, false, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  if (!ParseAdxHeader(stream, &info)) goto fail;

  result = new AdxAudioStream;
  result->BaseStream = stream;
  result->InitWithInfo(&info);

  return result;

fail:
  if (result) {
    result->BaseStream = 0;
    delete result;
  }
  stream->Seek(0, RW_SEEK_SET);
  return 0;
}
AdxAudioStream::~AdxAudioStream() {}

void AdxAudioStream::InitWithInfo(AdxHeaderInfo* info) {
  ChannelCount = info->ChannelCount;
  SampleRate = info->SampleRate;
  EncodedBytesPerBuffer = info->FrameSize * ChannelCount;
  StreamDataOffset = info->StreamDataOffset;
  Hist1[0] = info->Hist1_L;
  Hist1[1] = info->Hist1_R;
  Hist2[0] = info->Hist2_L;
  Hist2[1] = info->Hist2_R;
  SamplesPerBuffer = (info->FrameSize - 2) * 2;
  Duration = info->SampleCount;
  LoopStart = info->HasLoop ? info->LoopStart : 0;
  LoopEnd = info->HasLoop ? info->LoopEnd : Duration;
  SetCoefficients(info->Highpass, SampleRate);

  BitDepth = 16;

  BaseStream->Seek(StreamDataOffset, RW_SEEK_SET);
}

int AdxAudioStream::Read(void* buffer, int samples) {
  return ReadBuffered((int16_t*)buffer, samples);
}

void AdxAudioStream::Seek(int samples) { SeekBuffered(samples); }

bool AdxAudioStream::_registered =
    AudioStream::AddAudioStreamCreator(&AdxAudioStream::Create);

}  // namespace Audio
}  // namespace Impacto