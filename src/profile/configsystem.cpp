#include "configsystem.h"
#include "profile_internal.h"
#include "../audio/audiosystem.h"

namespace Impacto {
namespace Profile {
namespace ConfigSystem {

void Configure() {
  std::fill(Default::VoiceMuted.begin(), Default::VoiceMuted.end(), false);
  std::fill(Default::VoiceVolume.begin(), Default::VoiceVolume.end(), 1.0f);
  std::fill(Default::GroupVolumes.begin(), Default::GroupVolumes.end(), 0.5f);

  Default::GroupVolumes[Audio::ACG_BGM] = 0.25f;

  if (TryPushMember("ConfigSystem")) {
    AssertIs(LUA_TTABLE);

    Default::ShowTipsNotification =
        TryGetMember<bool>("ShowTipsNotification")
            .value_or(Default::ShowTipsNotification);
    Default::AutoQuickSave =
        TryGetMember<uint8_t>("AutoQuickSave").value_or(Default::AutoQuickSave);
    Default::ControllerType = TryGetMember<uint8_t>("ControllerType")
                                  .value_or(Default::ControllerType);
    Default::AdvanceTextOnDirectionalInput =
        TryGetMember<bool>("AdvanceTextOnDirectionalInput")
            .value_or(Default::AdvanceTextOnDirectionalInput);
    Default::DirectionalInputForTrigger =
        TryGetMember<bool>("DirectionalInputForTrigger")
            .value_or(Default::DirectionalInputForTrigger);
    Default::TriggerStopSkip = TryGetMember<bool>("TriggerStopSkip")
                                   .value_or(Default::TriggerStopSkip);

    Default::TextSpeed =
        TryGetMember<float>("TextSpeed").value_or(Default::TextSpeed);
    TextSpeedBounds =
        TryGetMember<glm::vec2>("TextSpeedBounds").value_or(TextSpeedBounds);
    Default::AutoSpeed =
        TryGetMember<float>("AutoSpeed").value_or(Default::AutoSpeed);
    AutoSpeedBounds =
        TryGetMember<glm::vec2>("AutoSpeedBounds").value_or(AutoSpeedBounds);

    Default::SkipRead =
        TryGetMember<bool>("SkipRead").value_or(Default::SkipRead);
    Default::SyncVoice =
        TryGetMember<bool>("SyncVoice").value_or(Default::SyncVoice);
    Default::SkipVoice =
        TryGetMember<bool>("SkipVoice").value_or(Default::SkipVoice);

    std::optional<uint32_t> optionalVoiceCount =
        TryGetMember<uint32_t>("VoiceCount");
    if (optionalVoiceCount) {
      uint32_t voiceCount = optionalVoiceCount.value();
      assert(voiceCount <= VoiceCount);

      GetMemberArray(Default::VoiceMuted.data(), voiceCount, "VoiceMuted");
      GetMemberArray(Default::VoiceVolume.data(), voiceCount, "VoiceVolume");
    }

    Default::ImageSize =
        TryGetMember<float>("ImageSize").value_or(Default::ImageSize);

    Pop();
  }

  ResetToDefault();
}

void ResetToDefault() {
  ShowTipsNotification = Default::ShowTipsNotification;
  AutoQuickSave = Default::AutoQuickSave;
  ControllerType = Default::ControllerType;
  AdvanceTextOnDirectionalInput = Default::AdvanceTextOnDirectionalInput;
  DirectionalInputForTrigger = Default::DirectionalInputForTrigger;
  TriggerStopSkip = Default::TriggerStopSkip;

  TextSpeed = Default::TextSpeed;
  AutoSpeed = Default::AutoSpeed;

  SkipRead = Default::SkipRead;
  SyncVoice = Default::SyncVoice;
  SkipVoice = Default::SkipVoice;

  std::ranges::copy(Default::VoiceMuted, VoiceMuted.begin());
  std::ranges::copy(Default::VoiceVolume, VoiceVolume.begin());
  std::ranges::copy(Default::GroupVolumes, Audio::GroupVolumes.begin());

  ImageSize = Default::ImageSize;
}

}  // namespace ConfigSystem
}  // namespace Profile
}  // namespace Impacto