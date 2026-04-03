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
  WavePos.resize((size_t)BGWaveGridSize.x * (size_t)BGWaveGridSize.y);
  Vertices.resize((size_t)BGWaveGridSize.x * (size_t)BGWaveGridSize.y);
  Indices.resize(
      ((size_t)BGWaveGridSize.x * 2 + 1) * ((size_t)BGWaveGridSize.y - 1) - 1);

  // triangle strips with primitive restart index
  constexpr uint16_t primitiveRestart = 0xFFFF;
  size_t i = 0;
  for (uint16_t y = 0; y < BGWaveGridSize.y - 1; y++) {
    for (uint16_t x = 0; x < BGWaveGridSize.x; x++) {
      const uint16_t firstIndex = y * BGWaveGridSize.x + x;
      Indices[i++] = firstIndex;
      Indices[i++] = firstIndex + BGWaveGridSize.x;
    }

    if (y != BGWaveGridSize.y - 2) {
      Indices[i++] = primitiveRestart;
    }
  }
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

void BGWave::CalcPos(int startPhase, std::optional<uint32_t> alpha) {
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
    const bool topFlag = flags & 4;
    const bool bottomFlag = flags & 8;
    const bool leftFlag = flags & 1;
    const bool rightFlag = flags & 2;

    for (size_t j = 0; j < maxGridDimension; j++) {
      currentPhase = (currentPhase + frequency) & 0xFFFF;
      const float ampedSin =
          amplitude * std::sin(ScrWorkAngleToRad(currentPhase));
      if (j < BGWaveGridSize.y) {
        if (leftFlag) {
          left[j] += ampedSin - amplitude;
        }

        if (rightFlag) {
          right[j] += ampedSin + amplitude;
        }
      }

      // top and bottom are switched since original engine has an upside-down
      // mesh
      if (j < BGWaveGridSize.x) {
        if (topFlag) {
          top[j] += ampedSin - amplitude;
        }

        if (bottomFlag) {
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

  const float alphaFloat = static_cast<float>(alpha.value_or(255)) / 255.0f;
  const glm::vec4 tint = glm::vec4(1.0f, 1.0f, 1.0f, alphaFloat);
  // update vertices
  for (size_t y = 0; y < BGWaveGridSize.y; y++) {
    float yUV =
        static_cast<float>(y) / static_cast<float>(BGWaveGridSize.y - 1);
    size_t row_start = y * BGWaveGridSize.x;
    for (size_t x = 0; x < BGWaveGridSize.x; x++) {
      float xUV =
          static_cast<float>(x) / static_cast<float>(BGWaveGridSize.x - 1);

      size_t index = row_start + x;
      Vertices[index] = VertexBufferSprites{
          .Position = WavePos[index], .UV = {xUV, yUV}, .Tint = tint};
    }
  }
}

void EFFWave::Render(const Sprite& mask) {
  PrimitiveData primitives = GetPrimitives();
  Renderer->DrawPrimitives(
      mask.Sheet, nullptr, ShaderProgramType::Sprite, primitives.Vertices,
      primitives.Indices, glm::mat4(1.0f), glm::mat4(1.0f), false,
      TopologyMode::TriangleStrips, SpecialFBO::MaskEffectFrameBuffer);
}

// TODO: Not implemented
void CHAWave::CalcPos(int startPhase, std::optional<uint32_t> alpha) {}
}  // namespace Effects
}  // namespace Impacto