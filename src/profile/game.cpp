#include "game.h"
#include "profile.h"
#include "profile_internal.h"

namespace Impacto {
namespace Profile {

RendererType ActiveRenderer{RendererType::OpenGL};
VideoPlayerType VideoPlayer{VideoPlayerType::FFmpeg};
AudioBackendType ActiveAudioBackend{AudioBackendType::OpenAL};

int LayerCount;
int GameFeatures;

char const* WindowName;

bool LayFileBigEndian;
bool CharaIsMvl;
bool UseScreenCapEffects;
float LayFileTexXMultiplier;
float LayFileTexYMultiplier;

float DesignWidth;
float DesignHeight;

void LoadGameFromJson() {
  AssertIs(LUA_TTABLE);

  ActiveRenderer = RendererType::_from_integral_unchecked(
      EnsureGetMemberInt("ActiveRenderer"));

  LayerCount = EnsureGetMemberInt("LayerCount");
  GameFeatures = EnsureGetMemberInt("GameFeatures");
  WindowName = EnsureGetMemberString("WindowName");
  DesignWidth = EnsureGetMemberFloat("DesignWidth");
  DesignHeight = EnsureGetMemberFloat("DesignHeight");

  bool res = TryGetMemberBool("LayFileBigEndian", LayFileBigEndian);
  if (!res) LayFileBigEndian = false;
  res = TryGetMemberBool("CharaIsMvl", CharaIsMvl);
  if (!res) CharaIsMvl = false;
  res = TryGetMemberFloat("LayFileTexXMultiplier", LayFileTexXMultiplier);
  if (!res) LayFileTexXMultiplier = 1.0f;
  res = TryGetMemberFloat("LayFileTexYMultiplier", LayFileTexYMultiplier);
  if (!res) LayFileTexYMultiplier = 1.0f;
  res = TryGetMemberBool("UseScreenCapEffects", UseScreenCapEffects);
  if (!res) UseScreenCapEffects = true;
  int audioBackendType = -1;
  res = TryGetMemberInt("AudioBackendType", audioBackendType);
  if (!res)
    ActiveAudioBackend = AudioBackendType::OpenAL;
  else
    ActiveAudioBackend =
        AudioBackendType::_from_integral_unchecked(audioBackendType);
  int videoPlayerType = -1;
  res = TryGetMemberInt("VideoPlayerType", videoPlayerType);
  if (!res)
    VideoPlayer = VideoPlayerType::FFmpeg;
  else
    VideoPlayer = VideoPlayerType::_from_integral_unchecked(videoPlayerType);
}

}  // namespace Profile
}  // namespace Impacto