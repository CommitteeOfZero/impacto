#include "lopusaudiostream.h"
#include "../log.h"

using namespace Impacto::Io;

namespace Impacto {
namespace Audio {

AudioStream* LopusAudioStream::Create(Stream* stream) {
  LopusAudioStream* result = new LopusAudioStream();
  uint32_t contextOffset;

  if (Io::ReadLE<uint32_t>(stream) != 0x80000001) {
    goto fail;
  }

  result->BaseStream = stream;
  result->BitDepth = 16;

  Io::ReadLE<uint32_t>(result->BaseStream);  // chunk size
  Io::ReadLE<uint8_t>(result->BaseStream);   // version
  result->ChannelCount = Io::ReadLE<uint8_t>(result->BaseStream);
  {
    const uint16_t frameSize =
        Io::ReadLE<uint16_t>(result->BaseStream);  // frame size, 0 if VBR
    if (frameSize) {
      result->FrameSize = frameSize;
    }
  }
  result->SampleRate = Io::ReadLE<uint32_t>(result->BaseStream);

  // non standard sample rate case
  if (result->SampleRate != 48000) {
    result->SampleRate = 48000;
  }
  result->DataOffset = Io::ReadLE<uint32_t>(result->BaseStream);
  result->StartOffset = result->DataOffset + 8;
  result->BaseStream->Seek(4, IoSeek::Cur);
  contextOffset = Io::ReadLE<uint32_t>(result->BaseStream);
  result->PreSkipSampleCount = Io::ReadLE<uint16_t>(result->BaseStream);

  result->BaseStream->Seek(2, IoSeek::Cur);  // padding

  // context info chunk
  if (contextOffset && result->BaseStream->Seek(contextOffset, IoSeek::Set) &&
      Io::ReadLE<uint32_t>(result->BaseStream) == 0x80000003) {
    result->BaseStream->Seek(5, IoSeek::Cur);  // unk

    result->Loop = Io::ReadU8(result->BaseStream) != 0;
    result->BaseStream->Seek(2, IoSeek::Cur);  // zeroes padding?

    result->Duration =
        Io::ReadLE<int32_t>(result->BaseStream);  // number of samples
    result->LoopStart = Io::ReadLE<int32_t>(result->BaseStream);
    result->LoopEnd = Io::ReadLE<int32_t>(result->BaseStream);
    result->BaseStream->Seek(0x28, IoSeek::Cur);  // zeroes padding
  }
  // TODO: multistream info chunk, possible but vgstream comment says it's rare
  if (Io::ReadLE<uint32_t>(result->BaseStream) != 0x80000005) {
    ImpLog(LogLevel::Debug, LogChannel::Audio,
           "Multistream info chunk encountered, but not handled");
  }

  // data info chunk
  result->BaseStream->Seek(result->DataOffset, IoSeek::Set);
  if (Io::ReadLE<uint32_t>(result->BaseStream) != 0x80000004) {
    goto fail;
  }

  result->DataSize = Io::ReadLE<uint32_t>(result->BaseStream);
  result->SkipRemaining = (int)result->PreSkipSampleCount;

  {
    int opusErr = 0;
    result->Decoder =
        opus_decoder_create(result->SampleRate, result->ChannelCount, &opusErr);
    if (opusErr != OPUS_OK || !result->Decoder) {
      goto fail;
    }
  }

  // build frame table for seek
  {
    uint32_t currentPos = result->StartOffset;
    uint32_t end = result->StartOffset + result->DataSize;
    uint64_t runningSamples = 0;

    while (currentPos < end) {
      result->BaseStream->Seek(currentPos, IoSeek::Set);
      const uint32_t frameSize = ReadBE<uint32_t>(result->BaseStream);
      result->BaseStream->Seek(4, IoSeek::Cur);

      uint32_t packetOffset = currentPos + 8;
      if (frameSize == 0 ||
          static_cast<uint64_t>(packetOffset) + frameSize > end) {
        break;
      }

      uint8_t peek[4] = {0, 0, 0, 0};
      const uint32_t peekLen = std::min(frameSize, 4U);
      result->BaseStream->Read(peek, (int)peekLen);

      int frameSamples = opus_packet_get_nb_samples(peek, (int32_t)peekLen,
                                                    result->SampleRate);

      LopusFrameTableEntry entry = {
          .SampleOffset = runningSamples,
          .PacketOffset = packetOffset,
          .PacketSize = frameSize,
      };
      result->FrameTable.push_back(entry);

      runningSamples += static_cast<uint64_t>(frameSamples);
      currentPos = packetOffset + frameSize;
    }
  }

  return result;
fail:
  if (result) {
    delete result;
  }
  stream->Seek(0, IoSeek::Set);
  return nullptr;
}

LopusAudioStream::~LopusAudioStream() {
  if (Decoder) {
    opus_decoder_destroy(Decoder);
    Decoder = nullptr;
  }
}

constexpr int kMaxFrameSamples = 5760;

int LopusAudioStream::DecodeFrame(int frameIndex) {
  const LopusFrameTableEntry& entry = FrameTable[frameIndex];

  std::vector<uint8_t> packetBuf(entry.PacketSize);
  BaseStream->Seek(entry.PacketOffset, IoSeek::Set);
  BaseStream->Read(packetBuf.data(), (int)entry.PacketSize);

  PcmLeftover.assign((size_t)kMaxFrameSamples * ChannelCount, 0);
  return opus_decode(Decoder, packetBuf.data(), (int32_t)entry.PacketSize,
                     PcmLeftover.data(), kMaxFrameSamples, 0);
}

int LopusAudioStream::Read(void* out, int samples) {
  if (!Decoder || samples <= 0) return 0;

  int16_t* outPtr = reinterpret_cast<int16_t*>(out);
  int written = 0;

  while (written < samples) {
    int leftoverAvailable =
        (int)(PcmLeftover.size() / ChannelCount) - PcmLeftoverOffset;

    if (leftoverAvailable > 0) {
      int samplesToCopy = std::min(leftoverAvailable, samples - written);
      std::memcpy(outPtr + (size_t)written * ChannelCount,
                  PcmLeftover.data() + (size_t)PcmLeftoverOffset * ChannelCount,
                  (size_t)samplesToCopy * ChannelCount * sizeof(int16_t));
      PcmLeftoverOffset += samplesToCopy;
      written += samplesToCopy;
      continue;
    }

    if (PendingSeekFrameIndex.has_value()) {
      const int frameIndex = *PendingSeekFrameIndex;
      PendingSeekFrameIndex = std::nullopt;

      opus_decoder_ctl(Decoder, OPUS_RESET_STATE);
      int decodedSamples = std::max(DecodeFrame(frameIndex), 0);

      if (decodedSamples == 0) {
        PcmLeftover.clear();
      } else {
        PcmLeftover.resize((size_t)decodedSamples * ChannelCount);
      }
      PcmLeftoverOffset = (int)std::min<uint64_t>(PendingIntraFrameSkip,
                                                  (uint64_t)decodedSamples);
      PendingIntraFrameSkip = 0;
      continue;
    }

    if (CurrentFrameIndex >= (int)FrameTable.size()) {
      break;
    }

    int decodedSamples = DecodeFrame(CurrentFrameIndex);
    CurrentFrameIndex++;
    if (decodedSamples <= 0) {
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

  ReadPosition += written;
  return written;
}

void LopusAudioStream::Seek(int samples) {
  if (!Decoder || FrameTable.empty()) return;

  samples = std::max(0, samples);
  ReadPosition = samples;
  const uint64_t targetRaw =
      static_cast<uint64_t>(samples) + PreSkipSampleCount;

  const auto it =
      std::upper_bound(FrameTable.begin(), FrameTable.end(), targetRaw,
                       [](uint64_t value, const LopusFrameTableEntry& entry) {
                         return value < entry.SampleOffset;
                       });
  const int frameIndex =
      std::max(static_cast<int>(it - FrameTable.begin()) - 1, 0);

  const LopusFrameTableEntry& entry = FrameTable[frameIndex];

  PcmLeftover.clear();
  PcmLeftoverOffset = 0;
  SkipRemaining = 0;

  PendingSeekFrameIndex = frameIndex;
  PendingIntraFrameSkip =
      targetRaw > entry.SampleOffset ? targetRaw - entry.SampleOffset : 0;

  CurrentFrameIndex = frameIndex + 1;
}

bool LopusAudioStream::_registered =
    AudioStream::AddAudioStreamCreator(&LopusAudioStream::Create);

}  // namespace Audio
}  // namespace Impacto