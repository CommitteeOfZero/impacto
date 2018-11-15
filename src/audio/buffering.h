#pragma once

#include <algorithm>
#include "../impacto.h"

namespace Impacto {
namespace Audio {

template <typename T, typename SampleType>
class Buffering {
 protected:
  bool DecodeBuffer();
  SampleType* DecodedBuffer = 0;
  uint8_t* EncodedBuffer = 0;

  int DecodedSamplesAvailable = 0;
  int DecodedSamplesConsumed = 0;
  int SamplesPerBuffer = 0;
  int EncodedBytesPerBuffer = 0;
  int StreamDataOffset;

  int ReadBuffered(SampleType* out, int samples) {
    T* stream = static_cast<T*>(this);
    int read = 0;
    while (samples) {
      if (DecodedSamplesAvailable) {
        // write out decoded samples
        int samplesWrittenNow = std::min(samples, DecodedSamplesAvailable);
        if (out) {
          // null pointer => discard-seek
          memcpy(out,
                 stream->DecodedBuffer +
                     DecodedSamplesConsumed * stream->ChannelCount,
                 samplesWrittenNow * stream->ChannelCount * sizeof(SampleType));
          out += samplesWrittenNow * stream->ChannelCount;
        }
        samples -= samplesWrittenNow;
        read += samplesWrittenNow;
        stream->ReadPosition += samplesWrittenNow;
        DecodedSamplesAvailable -= samplesWrittenNow;
        DecodedSamplesConsumed += samplesWrittenNow;
      } else {
        // decode more data
        int samplesLeft = stream->Duration - stream->ReadPosition;
        if (samplesLeft == 0)
          //
          return read;

        DecodedSamplesAvailable = SamplesPerBuffer;
        DecodedSamplesConsumed = 0;
        stream->BaseStream->Read(stream->EncodedBuffer, EncodedBytesPerBuffer);
        bool decodeSuccess = stream->DecodeBuffer();

        DecodedSamplesAvailable =
            std::min(DecodedSamplesAvailable, samplesLeft);
        if (!decodeSuccess) return read;
      }
    }
    return read;
  }

  void SeekBuffered(int samples) {
    T* stream = static_cast<T*>(this);
    int buffer = samples / SamplesPerBuffer;
    int offset = samples % SamplesPerBuffer;
    int currentBuffer = stream->ReadPosition / SamplesPerBuffer;
    if (currentBuffer != buffer) {
      // seek base stream
      stream->BaseStream->Seek(
          StreamDataOffset + buffer * EncodedBytesPerBuffer, RW_SEEK_SET);
      stream->ReadPosition = buffer * SamplesPerBuffer;
      DecodedSamplesAvailable = 0;
      DecodedSamplesConsumed = 0;
    }
    // ensure available
    if (!DecodedSamplesAvailable) {
      ReadBuffered(NULL, 1);
    }
    // skip
    stream->ReadPosition = samples;
    DecodedSamplesAvailable = SamplesPerBuffer - offset;
    DecodedSamplesConsumed = offset;
  }
};

}  // namespace Audio
}  // namespace Impacto