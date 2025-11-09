#include "audiosystem.h"
#include "../log.h"
#include "../profile/game.h"
#include "../profile/scriptvars.h"
#include "../profile/configsystem.h"

#ifndef IMPACTO_DISABLE_OPENAL
#include "openal/audiobackend.h"
#endif

#include <utility>

namespace Impacto {
namespace Audio {

using namespace Impacto::Profile::ScriptVars;

static bool IsInit = false;

template <typename GroupDef>
static AudioChannel* GetNextChannelInGroupImpl(GroupDef& groupDef) {
  if constexpr (GroupDef::ChannelIds.size() == 0) {
    return nullptr;
  } else {
    for (AudioChannelId id : GroupDef::ChannelIds) {
      if (Channels[id] && Channels[id]->GetState() == ACS_Stopped) {
        return Channels[id].get();
      }
    }

    AudioChannelId selectedId = GroupDef::ChannelIds[groupDef.NextIndex];
    groupDef.NextIndex = (groupDef.NextIndex + 1) % GroupDef::ChannelIds.size();
    return Channels[selectedId].get();
  }
}

AudioChannel* GetNextChannelInGroup(AudioChannelGroup group) {
  switch (group) {
    case ACG_BGM:
      return GetNextChannelInGroupImpl(std::get<ACG_BGM>(ChannelGroups));
    case ACG_SE:
      return GetNextChannelInGroupImpl(std::get<ACG_SE>(ChannelGroups));
    case ACG_Voice:
      return GetNextChannelInGroupImpl(std::get<ACG_Voice>(ChannelGroups));
    case ACG_Movie:
      return GetNextChannelInGroupImpl(std::get<ACG_Movie>(ChannelGroups));
    case ACG_Count:
      return nullptr;
  }
  return nullptr;
}

template <typename GroupDef>
static void StopChannelGroupImpl(float fadeOut) {
  for (AudioChannelId id : GroupDef::ChannelIds) {
    if (Channels[id]) {
      Channels[id]->Stop(fadeOut);
    }
  }
}

void StopChannelGroup(AudioChannelGroup group, float fadeOut) {
  switch (group) {
    case ACG_BGM:
      StopChannelGroupImpl<
          std::tuple_element_t<ACG_BGM, decltype(ChannelGroups)>>(fadeOut);
      break;
    case ACG_SE:
      StopChannelGroupImpl<
          std::tuple_element_t<ACG_SE, decltype(ChannelGroups)>>(fadeOut);
      break;
    case ACG_Voice:
      StopChannelGroupImpl<
          std::tuple_element_t<ACG_Voice, decltype(ChannelGroups)>>(fadeOut);
      break;
    case ACG_Movie:
      StopChannelGroupImpl<
          std::tuple_element_t<ACG_Movie, decltype(ChannelGroups)>>(fadeOut);
      break;
    case ACG_Count:
      break;
  }
}

template <typename GroupDef>
static void PauseChannelGroupImpl() {
  for (AudioChannelId id : GroupDef::ChannelIds) {
    if (Channels[id]) {
      Channels[id]->Pause();
    }
  }
}

void PauseChannelGroup(AudioChannelGroup group) {
  switch (group) {
    case ACG_BGM:
      PauseChannelGroupImpl<
          std::tuple_element_t<ACG_BGM, decltype(ChannelGroups)>>();
      break;
    case ACG_SE:
      PauseChannelGroupImpl<
          std::tuple_element_t<ACG_SE, decltype(ChannelGroups)>>();
      break;
    case ACG_Voice:
      PauseChannelGroupImpl<
          std::tuple_element_t<ACG_Voice, decltype(ChannelGroups)>>();
      break;
    case ACG_Movie:
      PauseChannelGroupImpl<
          std::tuple_element_t<ACG_Movie, decltype(ChannelGroups)>>();
      break;
    case ACG_Count:
      break;
  }
}

template <typename GroupDef>
static void ResumeChannelGroupImpl() {
  for (AudioChannelId id : GroupDef::ChannelIds) {
    if (Channels[id]) {
      Channels[id]->Resume();
    }
  }
}

void ResumeChannelGroup(AudioChannelGroup group) {
  switch (group) {
    case ACG_BGM:
      ResumeChannelGroupImpl<
          std::tuple_element_t<ACG_BGM, decltype(ChannelGroups)>>();
      break;
    case ACG_SE:
      ResumeChannelGroupImpl<
          std::tuple_element_t<ACG_SE, decltype(ChannelGroups)>>();
      break;
    case ACG_Voice:
      ResumeChannelGroupImpl<
          std::tuple_element_t<ACG_Voice, decltype(ChannelGroups)>>();
      break;
    case ACG_Movie:
      ResumeChannelGroupImpl<
          std::tuple_element_t<ACG_Movie, decltype(ChannelGroups)>>();
      break;
    case ACG_Count:
      break;
  }
}

void AudioShutdown() {
  for (int i = 0; i < AC_Count; i++) {
    Channels[i] = nullptr;
  }
  IsInit = false;
  Backend->Shutdown();
}

void AudioInit() {
  assert(IsInit == false);
  ImpLog(LogLevel::Info, LogChannel::Audio, "Initialising audio system\n");

  switch (Profile::ActiveAudioBackend) {
#ifndef IMPACTO_DISABLE_OPENAL
    case AudioBackendType::OpenAL: {
      Backend = new OpenAL::AudioBackend();
    } break;
#endif
    case AudioBackendType::None:
    default: {
      ImpLog(LogLevel::Warning, LogChannel::Audio,
             "Unknown or unsupported audio backend selected! You will not hear "
             "audio.\n");
      Backend = new AudioBackend();
    }
  }

  if (!Backend->Init()) return;
  for (int i = AC_SE0; i <= AC_SE2; i++)
    Channels[i] = AudioChannel::Create((AudioChannelId)i, ACG_SE);
  for (int i = AC_VOICE0; i <= AC_REV; i++)
    Channels[i] = AudioChannel::Create((AudioChannelId)i, ACG_Voice);
  for (int i = AC_BGM0; i <= AC_BGM2; i++)
    Channels[i] = AudioChannel::Create((AudioChannelId)i, ACG_BGM);

  Channels[AC_SSE0] = AudioChannel::Create(AC_SSE0, ACG_SE);
  Channels[AC_SSE1] = AudioChannel::Create(AC_SSE1, ACG_SE);
  Channels[AC_SSE2] = AudioChannel::Create(AC_SSE2, ACG_SE);
  Channels[AC_SSE3] = AudioChannel::Create(AC_SSE3, ACG_SE);

  IsInit = true;
}

void PlayInGroup(AudioChannelGroup group, std::string const& mountpoint,
                 uint32_t fileId, bool loop, float fadeIn) {
  AudioChannel* channel = GetNextChannelInGroup(group);
  if (channel) {
    channel->Play(mountpoint, fileId, loop, fadeIn);
  }
}

void PlayInGroup(AudioChannelGroup group, std::string const& mountpoint,
                 std::string const& fileName, bool loop, float fadeIn) {
  AudioChannel* channel = GetNextChannelInGroup(group);
  if (channel) {
    channel->Play(mountpoint, fileName, loop, fadeIn);
  }
}

void AudioUpdate(float dt) {
  // Set voice modifier for each voice channel
  for (int i = AC_VOICE0; i <= AC_VOICE2; i++) {
    const int charId = DialoguePages[i - AC_VOICE0].AnimationId;
    const int mappedCharId = ScrWork[SW_CHARACTERIDMAPPING + charId];
    const float voiceVolumeModifier =
        Profile::ConfigSystem::VoiceMuted[mappedCharId]
            ? 0.0f
            : Profile::ConfigSystem::VoiceVolume[mappedCharId];
    Channels[i]->SetVolume(voiceVolumeModifier);
  }

  for (int i = 0; i < AC_Count; i++) {
    Channels[i]->Update(dt);
  }
}

}  // namespace Audio
}  // namespace Impacto