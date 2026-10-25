#include "audiosystem.h"
#include "controlleraudio.h"
#include "../userconfig.h"
#include "../log.h"
#include "../profile/game.h"
#include "../profile/scriptvars.h"
#include "../profile/configsystem.h"
#include "../text/dialoguepage.h"
#include "../video/videosystem.h"

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

static std::string ControllerOutputDevice;

static bool IsPlayedOnController(AudioChannelId id) {
  auto const& config = UserConfig::CommonSettings.ControllerAudio;
  switch (id) {
    case AC_SE0:
    case AC_SE1:
    case AC_SE2:
      return config.SE;
    case AC_VOICE0:
    case AC_VOICE1:
    case AC_VOICE2:
    case AC_REV:
      return config.Voice;
    case AC_BGM0:
    case AC_BGM1:
    case AC_BGM2:
      return config.BGM;
    case AC_SSE0:
    case AC_SSE1:
    case AC_SSE2:
    case AC_SSE3:
      return config.SSE;
    case AC_Count:
      break;
  }
  return false;
}

static bool IsControllerAudioUsed() {
  bool used = UserConfig::CommonSettings.ControllerAudio.Movie;
  for (int i = 0; i < AC_Count; i++) {
    used |= IsPlayedOnController((AudioChannelId)i);
  }
  return used;
}

static void SetMovieOutput(AudioOutputId output) {
  if (+Profile::Game::GameFeatures & +GameFeature::Video) {
    Video::SetAudioOutput(output);
  }
}

static void CloseControllerOutput() {
  for (int i = 0; i < AC_Count; i++) {
    if (Channels[i]->GetOutput() == AO_Controller) {
      Channels[i]->SetOutput(AO_Main);
    }
  }
  SetMovieOutput(AO_Main);
  Backend->CloseOutput(AO_Controller);

  ImpLog(LogLevel::Info, LogChannel::Audio,
         "Stopped playing controller audio on {:s}\n", ControllerOutputDevice);
  ControllerOutputDevice.clear();
}

static void UpdateControllerOutput(bool deviceListChanged) {
  std::string const& device = ControllerAudio::GetDevice();

  if (device == ControllerOutputDevice) {
    const bool reopen = deviceListChanged && !device.empty() &&
                        !Backend->IsOutputOpen(AO_Controller);
    if (!reopen) return;
  }

  if (!ControllerOutputDevice.empty()) CloseControllerOutput();
  ControllerOutputDevice = device;
  if (!ControllerOutputDevice.empty() &&
      Backend->OpenOutput(AO_Controller, ControllerOutputDevice)) {
    ImpLog(LogLevel::Info, LogChannel::Audio,
           "Playing controller audio on {:s}\n", ControllerOutputDevice);
  }
}

static void UpdateChannelOutputs() {
  const AudioOutputId controllerOutput =
      Backend->IsOutputOpen(AO_Controller) ? AO_Controller : AO_Main;
  for (int i = 0; i < AC_Count; i++) {
    const AudioOutputId output =
        IsPlayedOnController((AudioChannelId)i) ? controllerOutput : AO_Main;
    if (Channels[i]->GetOutput() != output) Channels[i]->SetOutput(output);
  }
  SetMovieOutput(UserConfig::CommonSettings.ControllerAudio.Movie
                     ? controllerOutput
                     : AO_Main);
}

void AudioShutdown() {
  for (int i = 0; i < AC_Count; i++) {
    Channels[i] = nullptr;
  }
  IsInit = false;
  SetMovieOutput(AO_Main);
  ControllerAudio::Shutdown();
  Backend->Shutdown();
}

void AudioInit() {
  assert(IsInit == false);
  ImpLog(LogLevel::Info, LogChannel::Audio, "Initialising audio system\n");

  switch (UserConfig::AdvancedSettings.ActiveAudioBackend) {
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
  ControllerOutputDevice.clear();
  ControllerAudio::Init();
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

void AudioReinit() {
  if (Backend->Reinit()) {
    ImpLog(LogLevel::Info, LogChannel::Audio, "Reinitializing audio system\n");
    if (Backend->ReopenSupported()) return;
  }

  for (int i = 0; i < AC_Count; i++) {
    if (Channels[i]->GetOutput() == AO_Main) Channels[i]->Reinit();
  }
}

void AudioSubtitlesStart(AudioChannel* channel) {
  using Profile::Subtitle::SubtitleMappings;
  std::optional<Subtitle::SubtitlePlayer> player;

  if (channel->GetGroup() != AudioChannelGroup::ACG_BGM) return;
  AudioStream const* bgmStream = channel->GetStream();

  auto bgmSubtitleMapItr = SubtitleMappings.find("bgm");
  if (bgmSubtitleMapItr == SubtitleMappings.end()) return;
  auto mappingsItr =
      bgmSubtitleMapItr->second.find(bgmStream->GetBaseStream()->Meta.FileName);
  if (mappingsItr == bgmSubtitleMapItr->second.end()) {
    mappingsItr =
        bgmSubtitleMapItr->second.find(bgmStream->GetBaseStream()->Meta.Id);
  }
  if (mappingsItr == bgmSubtitleMapItr->second.end()) return;

  SubtitlePlayers[channel->GetId() - AC_BGM0].emplace(
      Profile::Game::DesignWidth, Profile::Game::DesignHeight);
  int trackId = 0;
  for (auto const& subFile : mappingsItr->second) {
    if (!subFile.Path) continue;
    SubtitlePlayers[channel->GetId() - AC_BGM0]->AddTrackFile(
        trackId++, subFile.Type, *subFile.Path, subFile.Config);
  }
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
  const bool deviceListChanged = Backend->DidDeviceListChanged();
  ControllerAudio::Update(dt, IsControllerAudioUsed(), deviceListChanged);
  UpdateControllerOutput(deviceListChanged);
  UpdateChannelOutputs();

  // Set voice modifier for each voice channel
  for (int i = AC_VOICE0; i <= AC_VOICE2; i++) {
    const int charId = DialoguePages[i - AC_VOICE0].CurrentVoiceCharacterId;
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

void AudioSubtitlesUpdate() {
  for (int i = 0; i < ssize(SubtitlePlayers); i++) {
    if (!SubtitlePlayers[i]) continue;
    auto const* currentChannel = Channels[i + AC_BGM0].get();
    switch (currentChannel->GetState()) {
      case ACS_Stopped:
        SubtitlePlayers[i].reset();
        break;
      case ACS_Paused:
        break;
      case ACS_Playing:
      case ACS_FadingIn:
      case ACS_FadingOut: {
        auto durationSec =
            std::chrono::duration<float>{currentChannel->PositionInSeconds()};
        SubtitlePlayers[i]->UpdateElapsedTime(
            std::chrono::duration_cast<Video::Clock::Microseconds>(
                durationSec));
      }
    }
  }
}

void AudioSubtitlesRender() {
  for (auto& subtitlePlayer : SubtitlePlayers) {
    if (subtitlePlayer) subtitlePlayer->Render();
  }
}

}  // namespace Audio
}  // namespace Impacto