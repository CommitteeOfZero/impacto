#pragma once

#include <cstdint>

namespace Impacto {
namespace Audio {
namespace ControllerAudio {

void Init();
void Update(bool enabled);
void Shutdown();

bool IsAvailable();

bool QueuePcm(void const* data, int sampleCount, int sampleRate,
              int channelCount, int bitDepth);
void FinishQueue();
void StopPlayback();
void SetPaused(bool paused);
void SetGain(float gain);

int GetQueuedDurationMs();
uint64_t GetPlayedSamples(int sampleRate);
bool IsFinished();

}  // namespace ControllerAudio
}  // namespace Audio
}  // namespace Impacto
