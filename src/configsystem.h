#include <algorithm>
#include <glm/glm.hpp>

namespace Impacto {
namespace ConfigSystem {

void Init();

// Add new tips to the tips notification rendering queue
inline bool ShowTipsNotification = true;

// Advance text on L/R stick, arrow keys, etc.
inline bool AdvanceTextOnDirectionalInput = false;

// Interact with trigger using left and right input in addition
// to their regular counterparts
inline bool DirectionalInputForTrigger = false;

// Stop skip mode when reaching a trigger
// (e.g. delusion trigger, phone trigger, etc.)
inline bool TriggerStopSkip = true;

// Typewriter animation speed
inline float TextSpeed = 768.0f / 60.0f;
constexpr inline glm::vec2 TextSpeedBounds = glm::vec2(256.0f, 4096.0f) / 60.0f;

// Speed to skip in auto mode (MessWaitSpeed)
inline float AutoSpeed = 768.0f / 60.0f;
// Menu is essentially auto *time* as opposed to auto *speed*
constexpr inline glm::vec2 AutoSpeedBounds = glm::vec2(2048.0f, 256.0f) / 60.0f;

// Only skip read text
inline bool SkipRead = true;

// Sync text speed with voice line duration
inline bool SyncVoice = true;

// Stop voice line after dialogue progression
inline bool SkipVoice = false;

// Individual character mute/volume settings
constexpr int VoiceCount = 33;
inline bool VoiceMuted[VoiceCount];
inline float VoiceVolume[VoiceCount];

}  // namespace ConfigSystem
}  // namespace Impacto