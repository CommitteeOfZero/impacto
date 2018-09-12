#pragma once

#include <GL/glew.h>

#include "model.h"

namespace Impacto {

class Character3D {
 public:
  static void Init();
  static Character3D* Load(uint32_t modelId);

  void Submit();

  void Render();

  ~Character3D();

  Model* StaticModel = 0;

  bool IsSubmitted = false;

 private:
  GLuint VAOs[ModelMaxMeshesPerModel];
  GLuint VBOs[ModelMaxMeshesPerModel];
  GLuint IBOs[ModelMaxMeshesPerModel];
};

}  // namespace Impacto