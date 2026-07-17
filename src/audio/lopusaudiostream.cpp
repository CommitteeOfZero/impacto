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

    result->LoopStart =
        result->LoopStart > result->Duration ? 0 : result->LoopStart;
    // Loop end can be negative, that's might be the workaround
    result->LoopEnd = result->LoopEnd < 0 ? result->Duration : result->LoopEnd;
    if (result->Duration == 0) {
      result->Duration = result->LoopEnd;
    }
    result->BaseStream->Seek(dataOffset, RW_SEEK_SET);
  }
  // TODO: multistream info chunk, possible but vgstream comments say it's rare

  // data info chunk
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

constexpr int kMaxFrameSamples = 5760;

// todo slop
int LopusAudioStream::Read(void* out, int samples) {
  if (!Decoder || samples <= 0) return 0;

  int16_t* outPtr = reinterpret_cast<int16_t*>(out);
  int written = 0;

  while (written < samples) {
    int leftoverAvailable =
        (int)(PcmLeftover.size() / ChannelCount) - PcmLeftoverOffset;

    if (leftoverAvailable > 0) {
      int toCopy = std::min(leftoverAvailable, samples - written);
      std::memcpy(outPtr + (size_t)written * ChannelCount,
                  PcmLeftover.data() + (size_t)PcmLeftoverOffset * ChannelCount,
                  (size_t)toCopy * ChannelCount * sizeof(int16_t));
      PcmLeftoverOffset += toCopy;
      written += toCopy;
      continue;
    }

    if (CurrentFrameIndex >= (int)FrameTable.size()) {
      break;
    }

    const LopusFrameTableEntry& entry = FrameTable[CurrentFrameIndex];
    CurrentFrameIndex++;

    std::vector<uint8_t> packetBuf(entry.PacketSize);
    BaseStream->Seek(entry.PacketOffset, RW_SEEK_SET);
    BaseStream->Read(packetBuf.data(), (int)entry.PacketSize);

    PcmLeftover.assign((size_t)kMaxFrameSamples * ChannelCount, 0);
    int decodedSamples =
        opus_decode(Decoder, packetBuf.data(), (int32_t)entry.PacketSize,
                    PcmLeftover.data(), kMaxFrameSamples, 0);
    if (decodedSamples < 0) {
      PcmLeftover.clear();
      PcmLeftoverOffset = 0;
      continue;
    }
    PcmLeftover.resize((size_t)decodedSamples * ChannelCount);
    PcmLeftoverOffset = 0;
    if (SkipRemaining > 0) {
      int skip = std::min(decodedSamples, SkipRemaining);
      PcmLeftoverOffset = skip;
      SkipRemaining -= skip;
    }
  }

  CurrentSample += (uint64_t)written;
  return written;
}

// todo slop
void LopusAudioStream::Seek(int samples) {
  if (!Decoder || FrameTable.empty()) return;

  if (samples < 0) samples = 0;
  uint64_t targetRaw = (uint64_t)samples + PreSkipSampleCount;

  auto it =
      std::upper_bound(FrameTable.begin(), FrameTable.end(), targetRaw,
                       [](uint64_t value, const LopusFrameTableEntry& entry) {
                         return value < entry.SampleOffset;
                       });
  int frameIndex = (int)(it - FrameTable.begin()) - 1;
  if (frameIndex < 0) frameIndex = 0;

  const LopusFrameTableEntry& entry = FrameTable[frameIndex];

  std::vector<uint8_t> packetBuf(entry.PacketSize);
  BaseStream->Seek(entry.PacketOffset, RW_SEEK_SET);
  BaseStream->Read(packetBuf.data(), (int)entry.PacketSize);

  opus_decoder_ctl(Decoder, OPUS_RESET_STATE);

  PcmLeftover.assign((size_t)kMaxFrameSamples * ChannelCount, 0);
  int decodedSamples =
      opus_decode(Decoder, packetBuf.data(), (int32_t)entry.PacketSize,
                  PcmLeftover.data(), kMaxFrameSamples, 0);
  if (decodedSamples < 0) decodedSamples = 0;
  PcmLeftover.resize((size_t)decodedSamples * ChannelCount);

  uint64_t intraFrameSkip =
      targetRaw > entry.SampleOffset ? targetRaw - entry.SampleOffset : 0;
  PcmLeftoverOffset =
      (int)std::min<uint64_t>(intraFrameSkip, (uint64_t)decodedSamples);

  SkipRemaining = 0;
  CurrentFrameIndex = frameIndex + 1;
  CurrentSample = (uint64_t)samples;
}

bool LopusAudioStream::_registered =
    AudioStream::AddAudioStreamCreator(&LopusAudioStream::Create);

}  // namespace Audio
}  // namespace Impacto