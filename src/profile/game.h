#pragma once

#include "../game.h"

namespace Impacto {
namespace Profile {

extern RendererType ActiveRenderer;
extern VideoPlayerType VideoPlayer;
extern AudioBackendType ActiveAudioBackend;

extern int LayerCount;
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

void LoadGameFromJson();

}  // namespace Profile
}  // namespace Impacto