
namespace Impacto {
namespace Effects {

struct WaveParams {
  int Flags;
  int Amplitude;
  int PhaseDelta;
  int Phase;
  int Frequency;
};

class Wave {
 public:
  std::array<WaveParams, 20> WaveData;
  std::vector<float> WavePos;
  uint32_t Size;

  void InitWave();
  void SetWave(int flags, int amplitude, int nextPhase, int initialPhase,
               int frequency);
  void ResetWave(int index, int flags, int amplitude, int nextPhase,
                 int initialPhase, int frequency);

  inline void Update(float dt) {
    // TODO: figure out actual coefficient
    float coefficient = dt * 60;
    for (size_t i = 0; i < Size; i++) {
      WaveData[i].Phase +=
          static_cast<int>(WaveData[i].PhaseDelta * coefficient);
    }
  }
  virtual void Init();
  virtual void SetPos(int startPhase) = 0;
};

class BGWave : public Wave {
 public:
  void SetPos(int startPhase) override;
};

class EFFWave : public Wave {
 public:
  void SetPos(int startPhase) override;
};

class CHAWave : public Wave {
 public:
  // maybe
  inline void Init() override { WavePos = std::vector<float>(68 * 56); }
  void SetPos(int startPhase) override;
};

inline BGWave WaveBg = BGWave();
inline BGWave WaveEFF = BGWave();
inline CHAWave WaveCHA = CHAWave();

void Init();

void WaveSave(int* data);
void WaveLoad(int* data);

}  // namespace Effects
}  // namespace Impacto