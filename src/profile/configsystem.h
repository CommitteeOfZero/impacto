#pragma once

#include "../audio/audiocommon.h"
#include <glm/glm.hpp>

namespace Impacto {
namespace Profile {
namespace ConfigSystem {

constexpr int VoiceCount = 33;

namespace Default {
inline bool ShowTipsNotification = true;
inline bool AdvanceTextOnDirectionalInput = false;
inline bool DirectionalInputForTrigger = false;
inline bool TriggerStopSkip = true;

inline float TextSpeed = 768.0f / 60.0f;
inline float AutoSpeed = 768.0f / 60.0f;

inline bool SkipRead = true;
inline bool SyncVoice = true;
inline bool SkipVoice = false;

inline bool VoiceMuted[VoiceCount];
inline float VoiceVolume[VoiceCount];
inline float GroupVolumes[Audio::ACG_Count];
}  // namespace Default

// Add new tips to the tips notification rendering queue
inline bool ShowTipsNotification = Default::ShowTipsNotification;

// Advance text on L/R stick, arrow keys, etc.
inline bool AdvanceTextOnDirectionalInput =
    Default::AdvanceTextOnDirectionalInput;

// Interact with trigger using left and right input in addition
// to their regular counterparts
inline bool DirectionalInputForTrigger = Default::DirectionalInputForTrigger;

// Stop skip mode when reaching a trigger
// (e.g. delusion trigger, phone trigger, etc.)
inline bool TriggerStopSkip = Default::TriggerStopSkip;

// Typewriter animation speed
inline float TextSpeed = Default::TextSpeed;
inline glm::vec2 TextSpeedBounds = glm::vec2(256.0f, 4096.0f) / 60.0f;

// Speed to skip in auto mode (MessWaitSpeed)
inline float AutoSpeed = Default::AutoSpeed;
// Menu is essentially auto *time* as opposed to auto *speed*
inline glm::vec2 AutoSpeedBounds = glm::vec2(2048.0f, 256.0f) / 60.0f;

// Only skip read text
inline bool SkipRead = Default::SkipRead;

// Sync text speed with voice line duration
inline bool SyncVoice = Default::SyncVoice;

// Stop voice line after dialogue progression
inline bool SkipVoice = Default::SkipVoice;

// Individual character mute/volume settings
inline bool VoiceMuted[VoiceCount];
inline float VoiceVolume[VoiceCount];

void Configure();
void ResetToDefault();

}  // namespace ConfigSystem
}  // namespace Profile
}  // namespace Impacto