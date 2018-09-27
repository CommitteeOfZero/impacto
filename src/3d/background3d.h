#pragma once

#include <GL/glew.h>

#include "model.h"

namespace Impacto {

class Scene;
class Camera;

class Background3D {
 public:
  bool Load(uint32_t modelId);
  void Unload();

  void Submit();

  void Render();

  Model* StaticModel = 0;

  bool IsUsed = false;
  bool IsSubmitted = false;

 private:
  GLuint VAOs[ModelMaxMeshesPerModel];
  GLuint VBOs[ModelMaxMeshesPerModel];
  GLuint IBOs[ModelMaxMeshesPerModel];

  GLuint TexBuffers[ModelMaxTexturesPerModel];
};

void Background3DInit();
// Background render updates affecting all backgrounds
void Background3DUpdateGpu(Scene* scene, Camera* camera);

}  // namespace Impacto