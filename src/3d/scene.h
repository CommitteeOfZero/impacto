#pragma once

#include <glm/glm.hpp>

#include "character3d.h"
#include "background3d.h"
#include "camera.h"

namespace Impacto {
namespace Scene3D {

int const MaxBackgrounds = 8;
int const MaxCharacters = 8;

void Init();
void Shutdown();
void Update(float dt);
void Render();

extern Camera MainCamera;

extern Background3D Backgrounds[MaxBackgrounds];
extern Character3D Characters[MaxCharacters];

extern glm::vec3 LightPosition;
extern glm::vec4 Tint;
extern bool DarkMode;

extern uint32_t CharacterToLoadId;

}  // namespace Scene3D
}  // namespace Impacto