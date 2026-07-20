#pragma once

#include "../game.h"

#include <cstdint>
#include <optional>
#include <string>

namespace Impacto {
namespace Profile {
namespace Game {

inline RendererType ActiveRenderer = RendererType::OpenGL;
inline VideoPlayerType VideoPlayer = VideoPlayerType::FFmpeg;
inline AudioBackendType ActiveAudioBackend = AudioBackendType::OpenAL;

inline SubtitleAssBackendType SubtitleAssBackend =
    SubtitleAssBackendType::LibAss;
inline SubtitleTextBackendType SubtitleTextBackend =
    SubtitleTextBackendType::None;
inline SubtitleBmpBackendType SubtitleBmpBackend = SubtitleBmpBackendType::None;

inline uint32_t LayerCount;
inline GameFeature GameFeatures;

inline char const* WindowName;
inline std::optional<std::string> WindowIconPath;
inline std::optional<std::string> CursorArrowPath;
inline std::optional<std::string> CursorPointerPath;

inline bool LayFileBigEndian;
inline bool CharaIsMvl;

inline size_t ScreenCaptureCount = 0;
inline bool UseMoviePriority = false;
inline bool UseBgChaEffects = false;
inline bool UseBgFrameEffects = false;
inline bool UseWaveEffects = false;

inline float LayFileTexXMultiplier;
inline float LayFileTexYMultiplier;

// The design coordinate system is: x,y from 0,0 to width,height,
// origin is top left
inline float DesignWidth;
inline float DesignHeight;

inline int PlatformId = 0;

void Configure();
}  // namespace Game
}  // namespace Profile
}  // namespace Impacto