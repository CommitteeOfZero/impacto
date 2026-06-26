#include "lopusaudiostream.h"
#include "../log.h"

using namespace Impacto::Io;

namespace Impacto {
namespace Audio {

AudioStream* LopusAudioStream::Create(Stream* stream) {
  LopusAudioStream* result = 0;
  uint32_t dataSize = 0;
  uint32_t dataOffset, frameDataOffset, contextOffset, preSkipSampleCount;

  if (Io::ReadLE<uint32_t>(stream) != 0x80000001) {
    goto fail;
  }

  result = new LopusAudioStream;
  result->BaseStream = stream;
  result->BitDepth = 16;

  Io::ReadLE<uint32_t>(result->BaseStream);  // chunk size
  Io::ReadLE<uint8_t>(result->BaseStream);   // version
  result->ChannelCount = Io::ReadLE<uint8_t>(result->BaseStream);
  {
    uint16_t frameSize =
        Io::ReadLE<uint16_t>(result->BaseStream);  // frame size, 0 if VBR
    if (frameSize) {
      result->FrameSize = frameSize;
    }
  }
  result->SampleRate = Io::ReadLE<uint32_t>(result->BaseStream);
  if (result->SampleRate != 48000) {
    // non standard sample rate
    result->SampleRate = 48000;
  }
  dataOffset = Io::ReadLE<uint32_t>(result->BaseStream);
  frameDataOffset = Io::ReadLE<uint32_t>(result->BaseStream);
  contextOffset = Io::ReadLE<uint32_t>(result->BaseStream);
  preSkipSampleCount = Io::ReadLE<uint16_t>(result->BaseStream);

  result->BaseStream->Seek(2, RW_SEEK_CUR);  // padding

  // context info chunk
  if (contextOffset && result->BaseStream->Seek(contextOffset, RW_SEEK_SET) &&
      Io::ReadLE<uint32_t>(result->BaseStream) == 0x80000003) {
    result->BaseStream->Seek(1, RW_SEEK_CUR);  // unk

    result->Loop = Io::ReadU8(result->BaseStream) != 0;
    result->Duration =
        Io::ReadLE<int32_t>(result->BaseStream);  // number of samples
    result->LoopStart = Io::ReadLE<int32_t>(result->BaseStream);
    result->LoopEnd = Io::ReadLE<int32_t>(result->BaseStream);
  }
  // TODO: multistream info chunk, possible but vgstream comments say it's rare

  // data info chunk
  result->BaseStream->Seek(dataOffset, RW_SEEK_SET);
  if (Io::ReadLE<uint32_t>(result->BaseStream) != 0x80000004) {
    goto fail;
  }

  dataSize = Io::ReadLE<uint32_t>(result->BaseStream);

  result->DataOffset = dataOffset;
  result->DataSize = dataSize;
  result->StartOffset = dataOffset + 8;
  result->PreSkipSampleCount = preSkipSampleCount;
  result->SkipRemaining = (int)preSkipSampleCount;

  {
    int opusErr = 0;
    result->Decoder =
        opus_decoder_create(result->SampleRate, result->ChannelCount, &opusErr);
    if (opusErr != OPUS_OK || !result->Decoder) {
      goto fail;
    }
  }

  // theoretical slop
  {
    uint32_t pos = result->StartOffset;
    uint32_t end = result->StartOffset + result->DataSize;
    uint64_t runningSamples = 0;
    LopusFrameTableEntry entry;

    while (pos < end) {
      result->BaseStream->Seek(pos, RW_SEEK_SET);
      uint32_t frameSize = ReadBE<uint32_t>(result->BaseStream);
      result->BaseStream->Seek(4, RW_SEEK_CUR);

      uint32_t packetOffset = pos + 8;
      if (frameSize == 0 || (uint64_t)packetOffset + frameSize > end) {
        break;
      }

      uint8_t peek[4] = {0, 0, 0, 0};
      uint32_t peekLen = std::min(frameSize, 4U);
      result->BaseStream->Read(peek, (int)peekLen);

      int frameSamples = opus_packet_get_nb_samples(peek, (int32_t)peekLen,
                                                    result->SampleRate);
      frameSamples = std::max(frameSamples, 0);

      entry.PacketOffset = packetOffset;
      entry.PacketSize = frameSize;
      entry.SampleOffset = runningSamples;
      result->FrameTable.push_back(entry);

      runningSamples += (uint64_t)frameSamples;
      pos = packetOffset + frameSize;
    }

    result->TotalSampleCount = runningSamples > result->PreSkipSampleCount
                                   ? runningSamples - result->PreSkipSampleCount
                                   : 0;
  }

  return result;
fail:
  if (result) {
    if (result->Decoder) opus_decoder_destroy(result->Decoder);
    delete result;
  }
  stream->Seek(0, RW_SEEK_SET);
  return 0;
}

void LopusAudioStream::InitWithInfo() {}

LopusAudioStream::~LopusAudioStream() {
  if (Decoder) {
    opus_decoder_destroy(Decoder);
    Decoder = 0;
  }
}

// todo
int LopusAudioStream::Read(void* out, int samples) { return 0; }

// todo
void LopusAudioStream::Seek(int samples) { return; }

bool LopusAudioStream::_registered =
    AudioStream::AddAudioStreamCreator(&LopusAudioStream::Create);

}  // namespace Audio
}  // namespace Impacto