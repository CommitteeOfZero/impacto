#pragma once

#include <glad/glad.h>

#include "model.h"
#include "modelanimator.h"
#include "../../loadable.h"
#include "../3d/scene.h"

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
class Camera;

class IRenderable3D : public Loadable<IRenderable3D> {
  friend class Loadable<IRenderable3D>;

 public:
  virtual void MakePlane() = 0;

  virtual void Update(float dt) = 0;
  virtual void Render() = 0;

  virtual void ReloadDefaultBoneTransforms() = 0;
  virtual void InitMeshAnimStatus() = 0;
  virtual void ReloadDefaultMeshAnimStatus() = 0;

  virtual void SwitchAnimation(int16_t animId, float transitionTime) = 0;

  Model* StaticModel = 0;

  // Per-frame results of vertex animation
  MorphVertexBuffer* CurrentMorphedVertices = 0;
  AnimatedMesh MeshAnimStatus[ModelMaxMeshesPerModel];
  PosedBone CurrentPose[ModelMaxBonesPerModel];
  Transform ModelTransform;
  ModelAnimator Animator;

  bool IsUsed = false;
  bool IsSubmitted = false;
  bool IsVisible = false;

 protected:
  virtual bool LoadSync(uint32_t modelId) = 0;
  virtual void UnloadSync() = 0;
  virtual void MainThreadOnLoad() = 0;
};

}  // namespace Impacto