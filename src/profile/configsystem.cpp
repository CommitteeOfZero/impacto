#include "configsystem.h"
#include "profile_internal.h"
#include "../audio/audiosystem.h"

namespace Impacto {
namespace Profile {
namespace ConfigSystem {

void Configure() {
  std::fill_n(Default::VoiceMuted, VoiceCount, false);
  std::fill_n(Default::VoiceVolume, VoiceCount, 1.0f);
  std::fill_n(Default::GroupVolumes, Audio::ACG_Count, 0.5f);

  Default::GroupVolumes[Audio::ACG_BGM] = 0.25f;

  if (TryPushMember("ConfigSystem")) {
    AssertIs(LUA_TTABLE);

    Default::ShowTipsNotification =
        TryGetMemberBool("ShowTipsNotification")
            .value_or(Default::ShowTipsNotification);
    Default::AdvanceTextOnDirectionalInput =
        TryGetMemberBool("AdvanceTextOnDirectionalInput")
            .value_or(Default::AdvanceTextOnDirectionalInput);
    Default::DirectionalInputForTrigger =
        TryGetMemberBool("DirectionalInputForTrigger")
            .value_or(Default::DirectionalInputForTrigger);
    Default::TriggerStopSkip =
        TryGetMemberBool("TriggerStopSkip").value_or(Default::TriggerStopSkip);

    Default::TextSpeed =
        TryGetMemberFloat("TextSpeed").value_or(Default::TextSpeed);
    TextSpeedBounds =
        TryGetMemberVec2("TextSpeedBounds").value_or(TextSpeedBounds);
    Default::AutoSpeed =
        TryGetMemberFloat("AutoSpeed").value_or(Default::AutoSpeed);
    AutoSpeedBounds =
        TryGetMemberVec2("AutoSpeedBounds").value_or(AutoSpeedBounds);

    Default::SkipRead =
        TryGetMemberBool("SkipRead").value_or(Default::SkipRead);
    Default::SyncVoice =
        TryGetMemberBool("SyncVoice").value_or(Default::SyncVoice);
    Default::SkipVoice =
        TryGetMemberBool("SkipVoice").value_or(Default::SkipVoice);

    std::optional<Uint32> optionalVoiceCount = TryGetMemberUint("VoiceCount");
    if (optionalVoiceCount) {
      Uint32 voiceCount = optionalVoiceCount.value();
      assert(voiceCount <= VoiceCount);

      GetMemberBoolArray(Default::VoiceMuted, voiceCount, "VoiceMuted");
      GetMemberFloatArray(Default::VoiceVolume, voiceCount, "VoiceVolume");
    }

    Pop();
  }

  ResetToDefault();
}

void ResetToDefault() {
  ShowTipsNotification = Default::ShowTipsNotification;
  AdvanceTextOnDirectionalInput = Default::AdvanceTextOnDirectionalInput;
  DirectionalInputForTrigger = Default::DirectionalInputForTrigger;
  TriggerStopSkip = Default::TriggerStopSkip;

  TextSpeed = Default::TextSpeed;
  AutoSpeed = Default::AutoSpeed;

  SkipRead = Default::SkipRead;
  SyncVoice = Default::SyncVoice;
  SkipVoice = Default::SkipVoice;

  std::copy(std::begin(Default::VoiceMuted), std::end(Default::VoiceMuted),
            VoiceMuted);
  std::copy(std::begin(Default::VoiceVolume), std::end(Default::VoiceVolume),
            VoiceVolume);
  std::copy(std::begin(Default::GroupVolumes), std::end(Default::GroupVolumes),
            Audio::GroupVolumes);
}

}  // namespace ConfigSystem
}  // namespace Profile
}  // namespace Impacto