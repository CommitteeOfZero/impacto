#pragma once

#include "../audio/audiocommon.h"
#include <glm/glm.hpp>

namespace Impacto {
namespace Profile {
namespace ConfigSystem {

constexpr int VoiceCount = 33;

BETTER_ENUM(AutoQuickSaveType, uint8_t, Never = 0, OnTrigger = (1 << 0),
            OnScene = (1 << 1))

namespace Default {
inline bool ShowTipsNotification = true;
inline uint8_t AutoQuickSave =
    AutoQuickSaveType::OnTrigger | AutoQuickSaveType::OnScene;
inline uint8_t ControllerType = 0;
inline bool AdvanceTextOnDirectionalInput = false;
inline bool DirectionalInputForTrigger = false;
inline bool TriggerStopSkip = true;

inline float TextSpeed = 768.0f / 60.0f;
inline float AutoSpeed = 768.0f / 60.0f;

inline bool SkipRead = true;
inline bool SyncVoice = true;
inline bool SkipVoice = false;

inline std::array<bool, VoiceCount> VoiceMuted;
inline std::array<float, VoiceCount> VoiceVolume;
inline std::array<float, Audio::ACG_Count> GroupVolumes;

inline float ImageSize = 1.0f;
}  // namespace Default

// Add new tips to the tips notification rendering queue
inline bool ShowTipsNotification = Default::ShowTipsNotification;

// When should the game automatically quick save
inline uint8_t AutoQuickSave = Default::AutoQuickSave;

// What controller scheme to use
inline uint8_t ControllerType = Default::ControllerType;

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
inline std::array<bool, VoiceCount> VoiceMuted;
inline std::array<float, VoiceCount> VoiceVolume;

// Scalar for the the viewport to render onto the window
inline float ImageSize;

void Configure();
void ResetToDefault();

}  // namespace ConfigSystem
}  // namespace Profile
}  // namespace Impacto