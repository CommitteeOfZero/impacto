#pragma once

#include <glm/glm.hpp>

#include "renderable3d.h"
#include "camera.h"

namespace Impacto {
namespace Scene3D {

int const MaxRenderables = 8;

void Init();
void Shutdown();
void Update(float dt);
void Render();

extern Camera MainCamera;

extern Renderable3D Renderables[MaxRenderables];

extern glm::vec3 LightPosition;
extern glm::vec4 Tint;
extern bool DarkMode;

}  // namespace Scene3D
}  // namespace Impacto