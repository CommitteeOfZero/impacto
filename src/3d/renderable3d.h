#pragma once

#include <glad/glad.h>

#include "model.h"
#include "modelanimator.h"
#include "../loadable.h"

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

class Renderable3D : public Loadable<Renderable3D> {
  friend class Loadable<Renderable3D>;

 public:
  static void Init();
  // Character render updates affecting all characters
  static void LoadSceneUniforms(Camera* camera);

  void MakePlane();

  void Update(float dt);
  void Render();

  void ReloadDefaultBoneTransforms();
  void InitMeshAnimStatus();
  void ReloadDefaultMeshAnimStatus();

  void SwitchAnimation(int16_t animId, float transitionTime);

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
  bool LoadSync(uint32_t modelId);
  void UnloadSync();
  void MainThreadOnLoad();

 private:
  void Pose();
  void PoseBone(int16_t id);

  void CalculateMorphedVertices(int id);

  void UseMesh(int id);
  void LoadModelUniforms();
  void LoadMeshUniforms(int id);
  void SetTextures(int id, int const* textureUnits, int count);
  void DrawCharacterMesh(int id);
  void DrawSimpleMesh(int id);

  GLuint UBOModel;

  GLuint VAOs[ModelMaxMeshesPerModel];
  GLuint VBOs[ModelMaxMeshesPerModel];
  GLuint MorphVBOs[ModelMaxMeshesPerModel];
  GLuint IBOs[ModelMaxMeshesPerModel];
  GLuint UBOs[ModelMaxMeshesPerModel];

  GLuint TexBuffers[ModelMaxTexturesPerModel];

  bool VAOsUpdated[ModelMaxMeshesPerModel];
  bool UniformsUpdated[ModelMaxMeshesPerModel];

  Transform PrevBoneTransforms[ModelMaxBonesPerModel];
  AnimatedMesh PrevMeshAnimStatus[ModelMaxMeshesPerModel];
  float PrevPoseWeight;
  float AnimationTransitionTime;
};

}  // namespace Impacto