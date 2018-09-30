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
  // TODO: Visibility not blended during animation transitions - is this okay?
  float Visible;
  float MorphInfluences[ModelMaxMorphTargetsPerMesh];
  int MorphedVerticesOffset;
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

  void SwitchAnimation(uint32_t animId, float transitionTime);

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

  void UpdateVAO(int id);
  void SetMeshUniforms(int id);
  void DrawMesh(int id);
  void DrawOutline(int id);

  GLuint VAOs[ModelMaxMeshesPerModel];
  GLuint VBOs[ModelMaxMeshesPerModel];
  GLuint MorphVBOs[ModelMaxMeshesPerModel];
  GLuint IBOs[ModelMaxMeshesPerModel];

  GLuint TexBuffers[ModelMaxTexturesPerModel];

  bool VAOsUpdated[ModelMaxMeshesPerModel];

  Transform PrevBoneTransforms[ModelMaxBonesPerModel];
  AnimatedMesh PrevMeshAnimStatus[ModelMaxMeshesPerModel];
  float PrevPoseWeight;
  float AnimationTransitionTime;
};

void Character3DInit();
// Character render updates affecting all characters
void Character3DUpdateGpu(Scene* scene, Camera* camera);

}  // namespace Impacto