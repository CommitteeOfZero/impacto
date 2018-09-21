#pragma once

#include <GL/glew.h>

#include "model.h"
#include "modelanimator.h"

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

struct AnimatedMesh {
  float Visible;
  float MorphInfluences[AnimMaxMorphTargetsPerTrack];
  int MorphedVerticesOffset;

  uint16_t UsedMorphTargetCount;
  uint16_t UsedMorphTargetIds[AnimMaxMorphTargetsPerTrack];
};

class ModelAnimator;
class Scene;
class Camera;

class Character3D {
 public:
  bool Load(uint32_t modelId);
  void Unload();
  void MakePlane();

  void Submit();

  void Update(float dt);
  void Render();

  void CalculateMorphedVertices();

  void ReloadDefaultBoneTransforms();
  void InitMeshAnimStatus();
  void ReloadDefaultMeshAnimStatus();

  Model* StaticModel = 0;

  // Per-frame results of vertex animation
  MorphVertexBuffer* CurrentMorphedVertices = 0;
  AnimatedMesh MeshAnimStatus[ModelMaxMeshesPerModel];
  PosedBone CurrentPose[ModelMaxBonesPerModel];
  Transform ModelTransform;
  ModelAnimator Animator;

  bool IsUsed = false;
  bool IsSubmitted = false;

 private:
  void Pose();
  void PoseBone(int16_t id);

  void DrawMesh(int id, bool outline);

  GLuint VAOs[ModelMaxMeshesPerModel];
  GLuint VBOs[ModelMaxMeshesPerModel];
  GLuint MorphVBOs[ModelMaxMeshesPerModel];
  GLuint IBOs[ModelMaxMeshesPerModel];

  GLuint TexBuffers[ModelMaxTexturesPerModel];
};

void Character3DInit();
// Character render updates affecting all characters
void Character3DUpdateGpu(Scene* scene, Camera* camera);

}  // namespace Impacto