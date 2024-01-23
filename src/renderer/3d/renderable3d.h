#pragma once

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

class IRenderable3D {
 public:
  virtual void MakePlane() = 0;

  virtual void Update(float dt) = 0;
  virtual void Render() = 0;

  virtual void ReloadDefaultBoneTransforms() = 0;
  virtual void InitMeshAnimStatus() = 0;
  virtual void ReloadDefaultMeshAnimStatus() = 0;

  virtual void SwitchAnimation(int16_t animId, float transitionTime) = 0;

  LoadStatus Status = LS_Unloaded;

  bool LoadAsync(uint32_t id) {
    if (Status == LS_Loading) {
      // cannot currently cancel a load
      return false;
    }
    Unload();
    NextLoadId = id;
    Status = LS_Loading;
    WorkQueue::Push(this, &LoadWorker, &OnLoaded);
    return true;
  }

  void Unload() {
    if (Status == LS_Loaded) {
      UnloadSync();
      Status = LS_Unloaded;
    }
  }

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

  uint32_t NextLoadId;

  static void LoadWorker(void* ptr) {
    auto renderable = static_cast<IRenderable3D*>(ptr);
    renderable->LoadSync(renderable->NextLoadId);
  }

  static void OnLoaded(void* ptr) {
    auto renderable = static_cast<IRenderable3D*>(ptr);
    renderable->MainThreadOnLoad();
    renderable->Status = LS_Loaded;
  }
};

}  // namespace Impacto