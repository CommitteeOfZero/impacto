#include "hcaaudiostream.h"

#include "../util.h"
#include "../log.h"

using namespace Impacto::Io;

namespace Impacto {
namespace Audio {

AudioStream* HcaAudioStream::Create(InputStream* stream) {
  clHCA* Decoder = 0;
  HcaAudioStream* result = 0;

  uint8_t headerStart[8];
  stream->Read(headerStart, 8);
  int headerSize = clHCA_isOurFile(headerStart, 8);
  if (headerSize < 0) {
    goto fail;
  }
  uint8_t* header = (uint8_t*)ImpStackAlloc(headerSize);
  memcpy(header, headerStart, 8);
  stream->Read(header + 8, headerSize - 8);

  Decoder = clHCA_new();
  int err = clHCA_DecodeHeader(Decoder, header, headerSize);
  ImpStackFree(header);
  if (err) {
    ImpLog(LL_Error, LC_Audio, "clHCA_DecodeHeader failed with code %d\n", err);
    goto fail;
  }

  clHCA_stInfo info;
  clHCA_getInfo(Decoder, &info);

  if (info.channelCount != 1 && info.channelCount != 2) {
    ImpLog(LL_Error, LC_Audio, "Unsupported channel count %d in HCA\n",
           info.channelCount);
    goto fail;
  }

  result = new HcaAudioStream;
  result->BaseStream = stream;
  result->Decoder = Decoder;
  result->InitWithInfo(&info);

  return result;

fail:
  if (Decoder) clHCA_delete(Decoder);
  if (result) {
    result->Decoder = 0;
    result->BaseStream = 0;
    delete result;
  }
  stream->Seek(0, RW_SEEK_SET);
  return 0;
}

void HcaAudioStream::InitWithInfo(clHCA_stInfo* info) {
  SampleRate = info->samplingRate;
  ChannelCount = info->channelCount;

  Duration = info->blockCount * info->samplesPerBlock;
  LoopStart = info->loopEnabled ? info->loopStartBlock * info->samplesPerBlock +
                                      info->loopStartDelay
                                : info->encoderDelay;
  LoopEnd = info->loopEnabled
                ? info->loopEndBlock * info->samplesPerBlock +
                      (info->samplesPerBlock - info->loopEndPadding)
                : Duration;
  EncodedBytesPerBuffer = info->blockSize;
  StreamDataOffset = info->headerSize;
  SamplesPerBuffer = info->samplesPerBlock;

  BitDepth = 16;

  EncodedBuffer = (uint8_t*)malloc(EncodedBytesPerBuffer);
  DecodedBuffer =
      (int16_t*)malloc(sizeof(int16_t) * SamplesPerBuffer * ChannelCount);

  clHCA_SetKey(Decoder, 0xCF222F1FE0748978u);  // default key

  clHCA_DecodeReset(Decoder);
  Seek(info->encoderDelay);
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

bool HcaAudioStream::_registered =
    AudioStream::AddAudioStreamCreator(&HcaAudioStream::Create);

}  // namespace Audio
}  // namespace Impacto