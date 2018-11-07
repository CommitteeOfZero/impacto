#include "hcaaudiostream.h"

#include "../util.h"
#include "../log.h"

namespace Impacto {
namespace Audio {

bool AudioIsHca(SDL_RWops* stream) {
  uint8_t header[8];
  SDL_RWread(stream, header, 8, 1);
  SDL_RWseek(stream, 0, RW_SEEK_SET);
  return clHCA_isOurFile(header, 8) >= 0;
  // TODO detect unsupported HCAs here (e.g. encrypted and we don't have the
  // key)
}

HcaAudioStream::HcaAudioStream(SDL_RWops* stream) : AudioStream(stream) {
  Decoder = clHCA_new();

  uint8_t headerStart[8];
  SDL_RWread(stream, headerStart, 8, 1);
  int headerSize = clHCA_isOurFile(headerStart, 8);
  uint8_t* header = (uint8_t*)ImpStackAlloc(headerSize);
  memcpy(header, headerStart, 8);
  SDL_RWread(stream, header + 8, headerSize - 8, 1);

  int err = clHCA_DecodeHeader(Decoder, header, headerSize);
  ImpStackFree(header);
  if (err) {
    ImpLog(LL_Error, LC_Audio, "clHCA_DecodeHeader failed with code %d\n", err);
    return;
  }

  clHCA_stInfo info;
  clHCA_getInfo(Decoder, &info);

  SampleRate = info.samplingRate;
  ChannelCount = info.channelCount;
  if (ChannelCount != 1 && ChannelCount != 2) {
    ImpLog(LL_Error, LC_Audio, "Unsupported channel count %d in HCA\n",
           ChannelCount);
    return;
  }
  Duration = info.blockCount * info.samplesPerBlock;
  LoopStart = info.loopEnabled ? info.loopStartBlock * info.samplesPerBlock +
                                     info.loopStartDelay
                               : info.encoderDelay;
  LoopEnd = info.loopEnabled ? info.loopEndBlock * info.samplesPerBlock +
                                   (info.samplesPerBlock - info.loopEndPadding)
                             : Duration;
  EncodedBytesPerBuffer = info.blockSize;
  StreamDataOffset = info.headerSize;
  SamplesPerBuffer = info.samplesPerBlock;

  BitDepth = 16;

  EncodedBuffer = (uint8_t*)malloc(EncodedBytesPerBuffer);
  DecodedBuffer =
      (int16_t*)malloc(sizeof(int16_t) * SamplesPerBuffer * ChannelCount);

  clHCA_SetKey(Decoder, 0xCF222F1FE0748978u);  // default key

  clHCA_DecodeReset(Decoder);
  Seek(info.encoderDelay);
}

HcaAudioStream::~HcaAudioStream() {
  if (EncodedBuffer) free(EncodedBuffer);
  if (DecodedBuffer) free(DecodedBuffer);
  if (Decoder) clHCA_delete(Decoder);
}

int HcaAudioStream::Read(void* buffer, int samples) {
  return ReadBuffered((int16_t*)buffer, samples);
}

void HcaAudioStream::Seek(int samples) { SeekBuffered(samples); }

bool HcaAudioStream::DecodeBuffer() {
  // TODO clHCA_DecodeReset after seek?
  int err = clHCA_DecodeBlock(Decoder, EncodedBuffer, EncodedBytesPerBuffer);
  if (err < 0) return false;
  clHCA_ReadSamples16(Decoder, DecodedBuffer);
  return true;
}

}  // namespace Audio
}  // namespace Impacto