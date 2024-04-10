#pragma once

#include "impacto.h"

#include <glm/glm.hpp>

namespace Impacto {
namespace ModelViewer {

extern uint32_t CurrentModel;
extern uint32_t CurrentBackground;

void Init();
void Update(float dt);

}  // namespace ModelViewer
}  // namespace Impacto