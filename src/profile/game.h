#pragma once

#include "../game.h"

#include <cstdint>

namespace Impacto {
namespace Profile {

extern RendererType ActiveRenderer;
extern VideoPlayerType VideoPlayer;
extern AudioBackendType ActiveAudioBackend;

extern uint32_t LayerCount;
extern int GameFeatures;

extern char const* WindowName;

extern bool LayFileBigEndian;
extern bool CharaIsMvl;
extern bool UseScreenCapEffects;
extern bool UseMoviePriority;
extern float LayFileTexXMultiplier;
extern float LayFileTexYMultiplier;

// The design coordinate system is: x,y from 0,0 to width,height,
// origin is top left
extern float DesignWidth;
extern float DesignHeight;

// This is for user configuration with realboot
extern char const* Language;
extern int ResolutionWidth;
extern int ResolutionHeight;
extern bool Fullscreen;
extern char const* Subtitles;

void LoadGameFromLua();

}  // namespace Profile
}  // namespace Impacto