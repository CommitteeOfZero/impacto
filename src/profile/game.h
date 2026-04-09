#pragma once

#include "../game.h"

#include <cstdint>
#include <optional>
#include <string>

namespace Impacto {
namespace Profile {

enum class DateFormatType : uint8_t {
  DMY,
  MDY,
  YMD,
};

enum class SubtitleConfigType : uint8_t {
  None = 0,
  Karaoke = 1 << 0,
  Translation = 1 << 1,
  All = 0xFF,
};

struct DateFormatDef {
  DateFormatDef(DateFormatType sel) : Sel(sel) {}
  std::string_view FormattedString() const {
    switch (Sel) {
      case DateFormatType::DMY:
        return "{:%d/%m/%y}";
      case DateFormatType::MDY:
        return "{:%m/%d/%y}";
      case DateFormatType::YMD:
      default:
        return "{:%y/%m/%d}";
    }
  }
  DateFormatType Sel;
};

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

// This is for user configuration with realboot
inline char const* Language;
inline int ResolutionWidth;
inline int ResolutionHeight;
inline bool Fullscreen;
// TODO Move to "Patch" logic
inline SubtitleConfigType SubtitleConfig = SubtitleConfigType::None;
inline bool CloseBacklogWhenReachedEnd = true;
inline DateFormatDef DateFormat = DateFormatType::YMD;
inline bool HasScriptedExitLogic = false;

inline int PlatformId = 0;

void LoadGameFromLua();

}  // namespace Profile
}  // namespace Impacto

template <>
struct magic_enum::customize::enum_range<Impacto::Profile::SubtitleConfigType> {
  static constexpr bool is_flags = true;
};