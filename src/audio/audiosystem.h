#pragma once

#include "../impacto.h"
#include "../log.h"
#include "audiochannel.h"
#include "audiobackend.h"
#include <memory>

namespace Impacto {
namespace Audio {

void AudioInit();
void AudioUpdate(float dt);
void AudioShutdown();

inline AudioBackend* Backend = nullptr;

inline float MasterVolume = 1.0f;
inline std::array<float, ACG_Count> GroupVolumes;
inline std::array<std::unique_ptr<AudioChannel>, AC_Count> Channels;

template <AudioChannelId... Ids>
struct ChannelGroupDef {
  static constexpr std::array<AudioChannelId, sizeof...(Ids)> ChannelIds{
      Ids...};
  size_t NextIndex = 0;
};

inline auto ChannelGroups = std::tuple{
    ChannelGroupDef<AC_BGM0, AC_BGM1, AC_BGM2>{},
    ChannelGroupDef<AC_SSE0, AC_SSE1, AC_SSE2, AC_SSE3>{},
    ChannelGroupDef<AC_VOICE0, AC_VOICE1, AC_VOICE2, AC_REV>{},
    ChannelGroupDef<>{},
};

AudioChannel* GetNextChannelInGroup(AudioChannelGroup group);
void StopChannelGroup(AudioChannelGroup group, float fadeOut);
void PauseChannelGroup(AudioChannelGroup group);
void ResumeChannelGroup(AudioChannelGroup group);

void PlayInGroup(AudioChannelGroup group, std::string const& mountpoint,
                 uint32_t fileId, bool loop, float fadeIn);
void PlayInGroup(AudioChannelGroup group, std::string const& mountpoint,
                 std::string const& fileName, bool loop, float fadeIn);

}  // namespace Audio
}  // namespace Impacto