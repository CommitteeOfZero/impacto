#pragma once

#include "../utils.h"
#include "../pipeline.h"
#include "../window.h"
#include "../../3d/renderable3d.h"

namespace Impacto {
namespace Vulkan {

enum RenderPass { RP_Outline = 0, RP_First = 1, RP_Second = 2, RP_Count };

struct PipelinePushConstants {
  VkBool32 IsDash;
};

struct SceneUniformBufferType {
  alignas(16) glm::mat4 ViewProjection;
  alignas(16) glm::vec4 Tint;
  alignas(16) glm::vec3 WorldLightPosition;
  alignas(16) glm::vec3 WorldEyePosition;
  alignas(16) VkBool32 DarkMode;
};

struct ModelUniformBufferType {
  alignas(16) glm::mat4 Model;
};

struct MeshUniformBufferType {
  alignas(16) glm::mat4 Bones[ModelMaxBonesPerMesh];
  alignas(16) float ModelOpacity;
  alignas(16) VkBool32 HasShadowColorMap;
};

struct BgMVPUniformBufferType {
  alignas(16) glm::mat4 MVP;
};

class Renderable3D : public IRenderable3D {
 public:
  static void Init(VulkanWindow* window, VkDevice device,
                   VkRenderPass renderPass, VkCommandBuffer* commandBuffers);
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

  AllocatedBuffer ModelUniformBuffers[MAX_FRAMES_IN_FLIGHT];
  uint8_t* ModelUniformBuffersMapped[MAX_FRAMES_IN_FLIGHT];
  AllocatedBuffer MeshUniformBuffers[ModelMaxMeshesPerModel]
                                    [MAX_FRAMES_IN_FLIGHT];
  uint8_t* MeshUniformBuffersMapped[ModelMaxMeshesPerModel]
                                   [MAX_FRAMES_IN_FLIGHT];
  AllocatedBuffer MeshVertexBuffers[ModelMaxMeshesPerModel]
                                   [MAX_FRAMES_IN_FLIGHT];
  AllocatedBuffer MeshIndexBuffers[ModelMaxMeshesPerModel]
                                  [MAX_FRAMES_IN_FLIGHT];

  AllocatedBuffer BackgroundMvpBuffers[ModelMaxMeshesPerModel]
                                      [MAX_FRAMES_IN_FLIGHT];
  uint8_t* BackgroundMvpMapped[ModelMaxMeshesPerModel][MAX_FRAMES_IN_FLIGHT];

  int CurrentTextureBufferInfo = 0;
  VkDescriptorImageInfo TextureBufferInfo[4];

  int CurrentWriteDescriptorSet = 0;
  VkWriteDescriptorSet WriteDescriptorSet[4];

  AllocatedBuffer CurrentMorphedVerticesVk[MAX_FRAMES_IN_FLIGHT];
  void* CurrentMorphedVerticesVkMapped[MAX_FRAMES_IN_FLIGHT];

  uint32_t TexBuffers[ModelMaxTexturesPerModel];

  bool UniformsUpdated[ModelMaxMeshesPerModel];

  Transform PrevBoneTransforms[ModelMaxBonesPerModel];
  AnimatedMesh PrevMeshAnimStatus[ModelMaxMeshesPerModel];
  float PrevPoseWeight;
  float AnimationTransitionTime;
};

}  // namespace Vulkan
}  // namespace Impacto