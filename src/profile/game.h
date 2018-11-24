#pragma once

namespace Impacto {
namespace Profile {

extern int LayerCount;
extern int GameFeatures;

// The design coordinate system is: x,y from 0,0 to width,height,
// origin is top left
extern float DesignWidth;
extern float DesignHeight;

void LoadFromJson();

}  // namespace Profile
}  // namespace Impacto