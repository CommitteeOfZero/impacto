#pragma once

#include <glad/glad.h>

#include "model.h"
#include "../loadable.h"

namespace Impacto {

class Camera;

class Background3D : public Loadable<Background3D> {
  friend class Loadable<Background3D>;

 public:
  static void Init();
  // Background render updates affecting all backgrounds
  static void UpdateGpu(Camera* camera);

  void Render();

  Model* StaticModel = 0;

  bool IsUsed = false;
  bool IsSubmitted = false;
  bool IsVisible = false;

 protected:
  bool LoadSync(uint32_t modelId);
  void UnloadSync();
  void MainThreadOnLoad();

 private:
  GLuint VAOs[ModelMaxMeshesPerModel];
  GLuint VBOs[ModelMaxMeshesPerModel];
  GLuint IBOs[ModelMaxMeshesPerModel];

  GLuint TexBuffers[ModelMaxTexturesPerModel];
};

}  // namespace Impacto