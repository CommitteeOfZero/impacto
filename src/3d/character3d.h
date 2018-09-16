#pragma once

#include <GL/glew.h>

#include "model.h"

namespace Impacto {

struct PosedBone {
  // Affine transform in relation to parent
  Transform LocalTransform;
  // Order: T * Rz * Ry * Rx * S

  // Affine transform in relation to world
  glm::mat4 World;
  // Affine transform in relation to bindpose
  glm::mat4 Offset;
};

class Character3D {
 public:
  bool Load(uint32_t modelId);
  void Unload();
  void MakePlane();

  void Submit();

  void Update(float dt);
  void Render();

  Model* StaticModel = 0;

  PosedBone* CurrentPose = 0;
  Transform ModelTransform;

  bool IsUsed = false;
  bool IsSubmitted = false;

 private:
  void ReloadDefaultBoneTransforms();
  void Pose();
  void PoseBone(int16_t id);

  GLuint VAOs[ModelMaxMeshesPerModel];
  GLuint VBOs[ModelMaxMeshesPerModel];
  GLuint IBOs[ModelMaxMeshesPerModel];

  GLuint TexBuffers[ModelMaxTexturesPerModel];
};

void Character3DInit();

}  // namespace Impacto