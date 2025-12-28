#include <vector>
#include "../renderer/renderer.h"
#include "../profile/data/waveeffects.h"

namespace Impacto {
namespace Effects {

struct WaveParams {
  int Flags;
  int Amplitude;
  int TemporalFrequency;
  int Phase;
  int SpatialFrequency;
};

class Wave {
 public:
  std::vector<WaveParams> WaveData{};
  std::vector<glm::vec2> WavePos{};
  std::vector<VertexBufferSprites> Vertices{};
  std::vector<uint16_t> Indices{};
  uint32_t WaveCount = 0;

  void ClearWaves();
  void AddWave(WaveParams params);
  void SetWave(int index, WaveParams params);

  inline void Update(float dt) {
    // temporal frequency update was meant to be called every frame on capped
    // 60fps
    const float coefficient = dt * 60.0f;
    for (size_t i = 0; i < WaveCount; i++) {
      WaveData[i].Phase +=
          static_cast<int>(WaveData[i].TemporalFrequency * coefficient);
    }
  }

  virtual void Init() { WaveData.resize(Profile::WaveEffects::WaveMaxCount); };
  virtual void CalcPos(int startPhase) = 0;
  virtual PrimitiveData GetPrimitives() = 0;
};

class BGWave : public Wave {
 public:
  void Init() override;
  void CalcPos(int startPhase) override;
  PrimitiveData GetPrimitives() override;
};

class EFFWave : public Wave {
 public:
  void CalcPos(int startPhase) override;
  PrimitiveData GetPrimitives() override;
};

class CHAWave : public Wave {
 public:
  // maybe
  inline void Init() override {
    WaveData.resize(Profile::WaveEffects::WaveMaxCount);
    WavePos.resize(28 * 20);
  }
  void CalcPos(int startPhase) override;
  PrimitiveData GetPrimitives() override;
};

inline BGWave WaveBG = BGWave();
inline EFFWave WaveEFF = EFFWave();
inline CHAWave WaveCHA = CHAWave();

void Init();

}  // namespace Effects
}  // namespace Impacto