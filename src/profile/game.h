#pragma once

#include "../game.h"

#include <cstdint>

namespace Impacto {
namespace Profile {

inline RendererType ActiveRenderer = RendererType::OpenGL;
inline VideoPlayerType VideoPlayer = VideoPlayerType::FFmpeg;
inline AudioBackendType ActiveAudioBackend = AudioBackendType::OpenAL;

inline uint32_t LayerCount;
inline int GameFeatures;

inline char const* WindowName;

inline bool LayFileBigEndian;
inline bool CharaIsMvl;
inline bool UseScreenCapEffects;
inline bool UseMoviePriority = false;
inline float LayFileTexXMultiplier;
inline float LayFileTexYMultiplier;

// The design coordinate system is: x,y from 0,0 to width,height,
// origin is top left
inline float DesignWidth;
inline float DesignHeight;

// This is for user configuration with realboot
inline char const* Language;
inline int ResolutionWidth;
inline int ResolutionHeight;
inline bool Fullscreen;
inline char const* Subtitles;

inline int PlatformId = 0;

void LoadGameFromLua();

}  // namespace Profile
}  // namespace Impacto