#pragma once

#include <glad/glad.h>

#include "../shader.h"
#include "../window.h"
#include "../../3d/renderable3d.h"

namespace Impacto {
namespace DirectX9 {

enum RenderPass { RP_Outline = 0, RP_First = 1, RP_Second = 2, RP_Count };

struct SceneUniformBufferType {
  glm::mat4 ViewProjection;
  glm::vec4 Tint;
  glm::vec3 WorldLightPosition;
  glm::vec3 WorldEyePosition;
  bool DarkMode;
};

struct ModelUniformBufferType {
  glm::mat4 Model;
};

struct MeshUniformBufferType {
  glm::mat4 Bones[ModelMaxBonesPerMesh];
  float ModelOpacity;
  bool HasShadowColorMap;
};

struct BgMVPUniformBufferType {
  glm::mat4 MVP;
};

class Renderable3D : public IRenderable3D {
 public:
  static void Init(DirectX9Window* window, IDirect3DDevice9* device);
  static void BeginFrame(IScene3D* scene, Camera* camera);

  void MakePlane() override;

  void Update(float dt) override;
  void Render() override;

  void ReloadDefaultBoneTransforms() override;
  void InitMeshAnimStatus() override;
  void ReloadDefaultMeshAnimStatus() override;

  void SwitchAnimation(int16_t animId, float transitionTime) override;

 protected:
  bool LoadSync(uint32_t modelId) override;
  void UnloadSync() override;
  void MainThreadOnLoad() override;

 private:
  void Pose();
  void PoseBone(int16_t id);

  void CalculateMorphedVertices(int id);

  void UseMaterial(MaterialType type);
  void UseMesh(int id);
  void LoadModelUniforms();
  void LoadMeshUniforms(int id);
  void SetTextures(int id, int const* textureUnits, int count);
  void DrawMesh(int id, RenderPass pass);

  void* CurrentMorphedVerticesDx;

  uint32_t TexBuffers[ModelMaxTexturesPerModel];

  bool UniformsUpdated[ModelMaxMeshesPerModel];

  Transform PrevBoneTransforms[ModelMaxBonesPerModel];
  AnimatedMesh PrevMeshAnimStatus[ModelMaxMeshesPerModel];
  float PrevPoseWeight;
  float AnimationTransitionTime;
};

}  // namespace DirectX9
}  // namespace Impacto