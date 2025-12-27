#include "wave.h"

#include "../profile/game.h"
#include "../util.h"

namespace Impacto {
namespace Effects {

using namespace Impacto::Profile::WaveEffects;

void Init() {
  WaveBG.Init();
  WaveEFF.Init();
  WaveCHA.Init();
}

void BGWave::Init() {
  WaveData.resize(WaveMaxCount);
  WavePos.resize(BGWaveGridSize.x * BGWaveGridSize.y);
  Vertices.resize(BGWaveGridSize.x * BGWaveGridSize.y);
  Indices.resize((BGWaveGridSize.x * 2 + 1) * (BGWaveGridSize.y - 1) - 1);
}

void Wave::ClearWaves() { WaveCount = 0; }

void Wave::AddWave(WaveParams params) { WaveData[WaveCount++] = params; }

void Wave::SetWave(int index, WaveParams params) {
  if (params.Flags > -1) {
    WaveData[index].Flags = params.Flags;
  }
  if (params.Amplitude > -1) {
    WaveData[index].Amplitude = params.Amplitude;
  }
  if (params.TemporalFrequency > -1) {
    WaveData[index].TemporalFrequency = params.TemporalFrequency;
  }
  if (params.Phase > -1) {
    WaveData[index].Phase = params.Phase;
  }
  if (params.SpatialFrequency > -1) {
    WaveData[index].SpatialFrequency = params.SpatialFrequency;
  }
}

void BGWave::CalcPos(int startPhase) {
  std::vector<float> left = std::vector(BGWaveGridSize.y, 0.0f);
  std::vector<float> right =
      std::vector(BGWaveGridSize.y, (float)Profile::ResolutionWidth);

  std::vector<float> top = std::vector(BGWaveGridSize.x, 0.0f);
  std::vector<float> bottom =
      std::vector(BGWaveGridSize.x, (float)Profile::ResolutionHeight);

  const uint16_t maxGridDimension =
      std::max(BGWaveGridSize.x, BGWaveGridSize.y);

  for (size_t i = 0; i < WaveCount; i++) {
    int currentPhase = WaveData[i].Phase + startPhase;
    const float amplitude = static_cast<float>(WaveData[i].Amplitude);
    const int frequency = WaveData[i].SpatialFrequency;
    const int flags = WaveData[i].Flags;

    for (size_t j = 0; j < maxGridDimension; j++) {
      currentPhase = (currentPhase + frequency) & 0xFFFF;
      const float ampedSin =
          amplitude * std::sin(ScrWorkAngleToRad(currentPhase));
      if (j < BGWaveGridSize.y) {
        if (flags & 1) {
          left[j] += ampedSin - amplitude;
        }

        if (flags & 2) {
          right[j] += ampedSin + amplitude;
        }
      }

      // top and bottom are switched since original engine has an upside-down
      // mesh
      if (j < BGWaveGridSize.x) {
        if (flags & 4) {
          top[j] += ampedSin - amplitude;
        }

        if (flags & 8) {
          bottom[j] += ampedSin + amplitude;
        }
      }
    }
  }

  std::vector<float> lerpStepX = std::vector(BGWaveGridSize.y, 0.0f);
  std::vector<float> lerpStepY = std::vector(BGWaveGridSize.x, 0.0f);
  const float HorzIntervalCountInverted = 1.0f / (BGWaveGridSize.x - 1.0f);
  const float VertIntervalCountInverted = 1.0f / (BGWaveGridSize.y - 1.0f);

  for (size_t i = 0; i < BGWaveGridSize.x; i++) {
    lerpStepY[i] = (bottom[i] - top[i]) * VertIntervalCountInverted;
  }

  for (size_t i = 0; i < BGWaveGridSize.y; i++) {
    lerpStepX[i] = (right[i] - left[i]) * HorzIntervalCountInverted;
  }

  for (size_t i = 0; i < BGWaveGridSize.y; i++) {
    float currentVal = left[i];
    const float step = lerpStepX[i];
    size_t rowStart = i * BGWaveGridSize.x;

    for (size_t j = 0; j < BGWaveGridSize.x; j++) {
      WavePos[rowStart + j].x = currentVal;
      currentVal += step;
    }
  }

  for (size_t j = 0; j < BGWaveGridSize.x; j++) {
    float currentVal = top[j];
    const float step = lerpStepY[j];

    for (size_t i = 0; i < BGWaveGridSize.y; i++) {
      WavePos[i * BGWaveGridSize.x + j].y = currentVal;
      currentVal += step;
    }
  }
}

// TODO: not implemented
void EFFWave::CalcPos(int startPhase) {}

// TODO: not implemented
void CHAWave::CalcPos(int startPhase) {}

PrimitiveData BGWave::GetPrimitives() {
  for (size_t y = 0; y < BGWaveGridSize.y; y++) {
    float yUV =
        static_cast<float>(y) / static_cast<float>(BGWaveGridSize.y - 1);
    size_t row_start = y * BGWaveGridSize.x;
    for (size_t x = 0; x < BGWaveGridSize.x; x++) {
      float xUV =
          static_cast<float>(x) / static_cast<float>(BGWaveGridSize.x - 1);

      size_t index = row_start + x;
      Vertices[index] =
          VertexBufferSprites{.Position = WavePos[index], .UV = {xUV, yUV}};
    }
  }

  constexpr uint16_t PRIMITIVE_RESTART = 0xFFFF;

  // triangle strips with primitive restart index
  size_t i = 0;
  for (uint16_t y = 0; y < BGWaveGridSize.y - 1; y++) {
    for (uint16_t x = 0; x < BGWaveGridSize.x; x++) {
      const uint16_t firstIndex = y * BGWaveGridSize.x + x;
      Indices[i++] = firstIndex;
      Indices[i++] = firstIndex + BGWaveGridSize.x;
    }

    if (y != BGWaveGridSize.y - 2) {
      Indices[i++] = PRIMITIVE_RESTART;
    }
  }

  return {std::span(Vertices), std::span(Indices)};
}

// TODO: Not implemented
PrimitiveData EFFWave::GetPrimitives() {
  return {std::span(Vertices), std::span(Indices)};
}

// TODO: Not implemented
PrimitiveData CHAWave::GetPrimitives() {
  return {std::span(Vertices), std::span(Indices)};
}

}  // namespace Effects
}  // namespace Impacto