#pragma once

#include "audiostream.h"
#include <opus/opus.h>
#include "../impacto.h"

namespace Impacto {
namespace Audio {

struct LopusFrameTableEntry {
  uint32_t PacketOffset;
  uint32_t PacketSize;
  uint64_t SampleOffset;
};
class LopusAudioStream : public AudioStream {
 public:
  ~LopusAudioStream();

  int Read(void* out, int samples) override;
  void Seek(int samples) override;

 private:
  static AudioStream* Create(Io::Stream* stream);
  LopusAudioStream() {}
  void InitWithInfo();

  uint32_t DataOffset = 0;
  uint32_t StartOffset = 0;
  uint32_t DataSize = 0;
  uint32_t PreSkipSampleCount = 0;
  uint64_t TotalSampleCount = 0;
  uint64_t CurrentSample = 0;
  std::optional<uint16_t> FrameSize = std::nullopt;
  std::vector<LopusFrameTableEntry> FrameTable;
  int CurrentFrameIndex = 0;

  OpusDecoder* Decoder = 0;

  std::vector<int16_t> PcmLeftover;
  int PcmLeftoverOffset = 0;
  int SkipRemaining = 0;
  bool Loop = false;

  static bool _registered;
};

}  // namespace Audio
}  // namespace Impacto