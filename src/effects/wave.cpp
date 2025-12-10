#include <vector>

#include "wave.h"

#include "../profile/data/bgeff.h"
#include "../profile/game.h"

namespace Impacto {
namespace Effects {

using namespace Impacto::Profile::BgEff;

void Wave::Init() {
  WavePos = std::vector<float>(WaveGridSize.x * WaveGridSize.y * 2);
}

void Wave::InitWave() { Size = 0; }

void Wave::SetWave(int flags, int amplitude, int phaseDelta, int initialPhase,
                   int frequency) {
  WaveData[Size++] = {flags, amplitude, phaseDelta, initialPhase, frequency};
}

void Wave::ResetWave(int index, int flags, int amplitude, int phaseDelta,
                     int initialPhase, int frequency) {
  if (flags > -1) {
    WaveData[index].Flags = flags;
  }
  if (amplitude > -1) {
    WaveData[index].Amplitude = amplitude;
  }
  if (phaseDelta > -1) {
    WaveData[index].PhaseDelta = phaseDelta;
  }
  if (initialPhase > -1) {
    WaveData[index].Phase = initialPhase;
  }
  if (frequency > -1) {
    WaveData[index].Frequency = frequency;
  }
}

void BGWave::SetPos(int startPhase) {
  std::vector<float> widths =
      std::vector(WaveGridSize.x, (float)Profile::ResolutionWidth);
  std::vector<float> heights =
      std::vector(WaveGridSize.x, (float)Profile::ResolutionHeight);
  std::vector<float> zeroes1 = std::vector(WaveGridSize.x, 0.0f);
  std::vector<float> zeroes2 = std::vector(WaveGridSize.x, 0.0f);

  for (size_t i = 0; i < Size; i++) {
    int resPhase = WaveData[i].Phase + startPhase;
    float amplitude = static_cast<float>(WaveData[i].Amplitude);
    int flags = WaveData[i].Flags;

    for (size_t j = 0; j < WaveGridSize.x; j++) {
      resPhase += WaveData[i].Frequency & 0xFFFF;

      float scaledSin = amplitude * (float)std::sin(resPhase) / 0x10000;
      if (j < WaveGridSize.y) {
        if (flags & 1) {
          zeroes2[j] += scaledSin - amplitude;
        }
        if (flags & 2) {
          widths[j] += scaledSin + amplitude;
        }
      }

      if (flags & 4) {
        zeroes1[j] += scaledSin - amplitude;
      }
      if (flags & 8) {
        heights[j] += scaledSin + amplitude;
      }
    }
  }

  std::vector<float> resX = std::vector(WaveGridSize.x, 0.0f);
  std::vector<float> resY = std::vector(WaveGridSize.x, 0.0f);
  const float HorizontalIntervalCountInverted = 1.0f / (WaveGridSize.x - 1.0f);
  const float VerticalIntervalCountInverted = 1.0f / (WaveGridSize.y - 1.0f);

  for (size_t i = 0; i < WaveGridSize.x; i++) {
    resX[i] = (widths[i] - zeroes2[i]) * HorizontalIntervalCountInverted;
    resY[i] = (heights[i] - zeroes1[i]) * VerticalIntervalCountInverted;
  }

  for (size_t i = 0; i < WaveGridSize.y; i++) {
    float curVal = zeroes2[i];
    const float delta = resX[i];
    size_t rowStart = WaveGridSize.x * i * 2;
    for (size_t j = 0; j < WaveGridSize.x; j++) {
      WavePos[rowStart + j * 2] = curVal;
      curVal += delta;
    }
  }

  for (size_t i = 0; i < WaveGridSize.x; i++) {
    float curVal = zeroes1[i];
    const float delta = resY[i];
    size_t rowStart = i * 2 + 1;
    for (size_t j = 0; j < WaveGridSize.y; j++) {
      WavePos[rowStart + j * WaveGridSize.x * 2] = curVal;
      curVal += delta;
    }
  }
}

void EFFWave::SetPos(int startPhase) {
  // TODO: not implemented
}

void CHAWave::SetPos(int startPhase) {
  // TODO: not implemented
}

void WaveLoad(int* data) {
  size_t dataSize = 0;
  for (size_t i = 0; i < 20; i++) {
    WaveBg.WaveData[i].Flags = data[dataSize++];
    WaveBg.WaveData[i].Amplitude = data[dataSize++];
    WaveBg.WaveData[i].PhaseDelta = data[dataSize++];
    WaveBg.WaveData[i].Phase = data[dataSize++];
    WaveBg.WaveData[i].Frequency = data[dataSize++];
  }
  for (size_t i = 0; i < 20; i++) {
    WaveEFF.WaveData[i].Flags = data[dataSize++];
    WaveEFF.WaveData[i].Amplitude = data[dataSize++];
    WaveEFF.WaveData[i].PhaseDelta = data[dataSize++];
    WaveEFF.WaveData[i].Phase = data[dataSize++];
    WaveEFF.WaveData[i].Frequency = data[dataSize++];
  }
  for (size_t i = 0; i < 20; i++) {
    WaveCHA.WaveData[i].Flags = data[dataSize++];
    WaveCHA.WaveData[i].Amplitude = data[dataSize++];
    WaveCHA.WaveData[i].PhaseDelta = data[dataSize++];
    WaveCHA.WaveData[i].Phase = data[dataSize++];
    WaveCHA.WaveData[i].Frequency = data[dataSize++];
  }

  WaveBg.Size = data[dataSize++];
  WaveEFF.Size = data[dataSize++];
  WaveCHA.Size = data[dataSize++];
}

void WaveSave(int* data) {
  size_t dataSize = 0;
  for (size_t i = 0; i < 20; i++) {
    data[dataSize++] = WaveBg.WaveData[i].Flags;
    data[dataSize++] = WaveBg.WaveData[i].Amplitude;
    data[dataSize++] = WaveBg.WaveData[i].PhaseDelta;
    data[dataSize++] = WaveBg.WaveData[i].Phase;
    data[dataSize++] = WaveBg.WaveData[i].Frequency;
  }
  for (size_t i = 0; i < 20; i++) {
    data[dataSize++] = WaveEFF.WaveData[i].Flags;
    data[dataSize++] = WaveEFF.WaveData[i].Amplitude;
    data[dataSize++] = WaveEFF.WaveData[i].PhaseDelta;
    data[dataSize++] = WaveEFF.WaveData[i].Phase;
    data[dataSize++] = WaveEFF.WaveData[i].Frequency;
  }
  for (size_t i = 0; i < 20; i++) {
    data[dataSize++] = WaveCHA.WaveData[i].Flags;
    data[dataSize++] = WaveCHA.WaveData[i].Amplitude;
    data[dataSize++] = WaveCHA.WaveData[i].PhaseDelta;
    data[dataSize++] = WaveCHA.WaveData[i].Phase;
    data[dataSize++] = WaveCHA.WaveData[i].Frequency;
  }

  data[dataSize++] = WaveBg.Size;
  data[dataSize++] = WaveEFF.Size;
  data[dataSize++] = WaveCHA.Size;
}

void Init() {
  WaveBg.Init();
  WaveEFF.Init();
  WaveCHA.Init();
}

}  // namespace Effects
}  // namespace Impacto