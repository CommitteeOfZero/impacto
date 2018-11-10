#pragma once

#include "impacto.h"

#include <glm/glm.hpp>

namespace Impacto {

class Game;

namespace ModelViewer {

void Init(Game* game);
void Update(float dt);

}  // namespace ModelViewer
}  // namespace Impacto