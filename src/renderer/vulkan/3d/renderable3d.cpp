#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "renderable3d.h"

#include "../../3d/camera.h"
#include "../../3d/modelanimator.h"
#include "../../../log.h"
#include "../../../profile/scene3d.h"

namespace Impacto {
namespace Vulkan {

static uint32_t TextureDummy = 0;

// character
static Pipeline *PipelineMain = 0, *PipelineOutline = 0, *PipelineEye = 0;
static Pipeline *PipelineMainNoDepthWrite = 0, *PipelineEyeNoDepthWrite = 0;

// background
static Pipeline* PipelineBackground = 0;

static AllocatedBuffer SceneUniformBuffers[MAX_FRAMES_IN_FLIGHT];
static uint8_t* SceneUniformBuffersMapped[MAX_FRAMES_IN_FLIGHT];

static glm::mat4 ViewProjection;

static bool IsInit = false;

static MaterialType CurrentMaterial = MT_None;
static bool CurrentMaterialIsDepthWrite = false;

static VulkanWindow* Window;
static VkDevice Device;
static VkCommandBuffer* CommandBuffers;
static VkDescriptorSetLayout ModelDescriptorSetLayout;
static VkDescriptorSetLayout BgTextureSetLayout;

static uint32_t StagingBufferSize = 4096 * 4096;

void Renderable3D::Init(VulkanWindow* window, VkDevice device,
                        VkRenderPass renderPass,
                        VkCommandBuffer* commandBuffers) {
  assert(IsInit == false);
  ImpLog(LL_Info, LC_Renderable3D, "Initializing Renderable3D system\n");
  IsInit = true;

  Window = window;
  Device = device;
  CommandBuffers = commandBuffers;

  // Character shaders
  VkDescriptorSetLayoutBinding uniformBinds[4] = {};
  uniformBinds[0].binding = 0;
  uniformBinds[0].descriptorCount = 1;
  uniformBinds[0].descriptorType =
      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;  // Scene uniform
  uniformBinds[0].pImmutableSamplers = nullptr;
  uniformBinds[0].stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
  uniformBinds[1].binding = 1;
  uniformBinds[1].descriptorCount = 1;
  uniformBinds[1].descriptorType =
      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;  // Model uniform
  uniformBinds[1].pImmutableSamplers = nullptr;
  uniformBinds[1].stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
  uniformBinds[2].binding = 2;
  uniformBinds[2].descriptorCount = 1;
  uniformBinds[2].descriptorType =
      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;  // Mesh uniform
  uniformBinds[2].pImmutableSamplers = nullptr;
  uniformBinds[2].stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
  uniformBinds[3].binding = 3;
  uniformBinds[3].descriptorCount = 4;
  uniformBinds[3].descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;  // Textures
  uniformBinds[3].pImmutableSamplers = nullptr;
  uniformBinds[3].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutCreateInfo modelSetLayoutInfo = {};
  modelSetLayoutInfo.bindingCount = 4;
  modelSetLayoutInfo.flags =
      VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR;
  modelSetLayoutInfo.pNext = nullptr;
  modelSetLayoutInfo.sType =
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  modelSetLayoutInfo.pBindings = uniformBinds;

  vkCreateDescriptorSetLayout(Device, &modelSetLayoutInfo, nullptr,
                              &ModelDescriptorSetLayout);

  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    SceneUniformBuffers[i] = CreateBuffer(sizeof(SceneUniformBufferType),
                                          VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                          VMA_MEMORY_USAGE_CPU_ONLY);
    void* data;
    vmaMapMemory(Allocator, SceneUniformBuffers[i].Allocation, &data);
    SceneUniformBuffersMapped[i] = (uint8_t*)data;
  }

  VkVertexInputBindingDescription modelBindingDescription{};
  modelBindingDescription.binding = 0;
  modelBindingDescription.stride =
      Profile::Scene3D::Version == +LKMVersion::DaSH ? sizeof(VertexBufferDaSH)
                                                     : sizeof(VertexBuffer);
  modelBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  VkVertexInputAttributeDescription modelAttributeDescriptions[5] = {};
  if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
    modelAttributeDescriptions[0].binding = 0;
    modelAttributeDescriptions[0].location = 0;
    modelAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    modelAttributeDescriptions[0].offset = offsetof(VertexBufferDaSH, Position);
    modelAttributeDescriptions[1].binding = 0;
    modelAttributeDescriptions[1].location = 1;
    modelAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    modelAttributeDescriptions[1].offset = offsetof(VertexBufferDaSH, Normal);
    modelAttributeDescriptions[2].binding = 0;
    modelAttributeDescriptions[2].location = 2;
    modelAttributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    modelAttributeDescriptions[2].offset = offsetof(VertexBufferDaSH, UV);
    modelAttributeDescriptions[3].binding = 0;
    modelAttributeDescriptions[3].location = 3;
    modelAttributeDescriptions[3].format = VK_FORMAT_R8G8B8A8_SINT;
    modelAttributeDescriptions[3].offset =
        offsetof(VertexBufferDaSH, BoneIndices);
    modelAttributeDescriptions[4].binding = 0;
    modelAttributeDescriptions[4].location = 4;
    modelAttributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    modelAttributeDescriptions[4].offset =
        offsetof(VertexBufferDaSH, BoneWeights);

  } else {
    modelAttributeDescriptions[0].binding = 0;
    modelAttributeDescriptions[0].location = 0;
    modelAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    modelAttributeDescriptions[0].offset = offsetof(VertexBuffer, Position);
    modelAttributeDescriptions[1].binding = 0;
    modelAttributeDescriptions[1].location = 1;
    modelAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    modelAttributeDescriptions[1].offset = offsetof(VertexBuffer, Normal);
    modelAttributeDescriptions[2].binding = 0;
    modelAttributeDescriptions[2].location = 2;
    modelAttributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    modelAttributeDescriptions[2].offset = offsetof(VertexBuffer, UV);
    modelAttributeDescriptions[3].binding = 0;
    modelAttributeDescriptions[3].location = 3;
    modelAttributeDescriptions[3].format = VK_FORMAT_R8G8B8A8_SINT;
    modelAttributeDescriptions[3].offset = offsetof(VertexBuffer, BoneIndices);
    modelAttributeDescriptions[4].binding = 0;
    modelAttributeDescriptions[4].location = 4;
    modelAttributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    modelAttributeDescriptions[4].offset = offsetof(VertexBuffer, BoneWeights);
  }

  VkPipelineDepthStencilStateCreateInfo depthStencil{};
  depthStencil.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = VK_TRUE;
  depthStencil.depthWriteEnable = VK_TRUE;
  depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
  depthStencil.depthBoundsTestEnable = VK_FALSE;
  depthStencil.stencilTestEnable = VK_FALSE;

  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  VkPushConstantRange pushConstant;
  pushConstant.offset = 0;
  pushConstant.size = sizeof(PipelinePushConstants);
  pushConstant.stageFlags =
      VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT;

  PipelineMain = new Pipeline(Device, renderPass);
  PipelineMain->SetPushConstants(&pushConstant, 1);
  PipelineMain->SetDepthStencilInfo(depthStencil);
  PipelineMain->CreateWithShader(
      "Renderable3D_Character", modelBindingDescription,
      modelAttributeDescriptions, 5, ModelDescriptorSetLayout);

  PipelineEye = new Pipeline(Device, renderPass);
  PipelineEye->SetPushConstants(&pushConstant, 1);
  PipelineEye->SetDepthStencilInfo(depthStencil);
  PipelineEye->CreateWithShader("Renderable3D_Eye", modelBindingDescription,
                                modelAttributeDescriptions, 5,
                                ModelDescriptorSetLayout);

  depthStencil.depthWriteEnable = VK_FALSE;
  PipelineOutline = new Pipeline(Device, renderPass);
  PipelineOutline->SetPushConstants(&pushConstant, 1);
  PipelineOutline->SetDepthStencilInfo(depthStencil);
  PipelineOutline->SetRasterizerInfo(rasterizer);
  PipelineOutline->CreateWithShader(
      "Renderable3D_Outline", modelBindingDescription,
      modelAttributeDescriptions, 5, ModelDescriptorSetLayout);

  PipelineMainNoDepthWrite = new Pipeline(Device, renderPass);
  PipelineMainNoDepthWrite->SetPushConstants(&pushConstant, 1);
  PipelineMainNoDepthWrite->SetDepthStencilInfo(depthStencil);
  PipelineMainNoDepthWrite->CreateWithShader(
      "Renderable3D_Character", modelBindingDescription,
      modelAttributeDescriptions, 5, ModelDescriptorSetLayout);

  PipelineEyeNoDepthWrite = new Pipeline(Device, renderPass);
  PipelineEyeNoDepthWrite->SetPushConstants(&pushConstant, 1);
  PipelineEyeNoDepthWrite->SetDepthStencilInfo(depthStencil);
  PipelineEyeNoDepthWrite->CreateWithShader(
      "Renderable3D_Eye", modelBindingDescription, modelAttributeDescriptions,
      5, ModelDescriptorSetLayout);

  // BG shaders
  VkDescriptorSetLayoutBinding bgBinds[2] = {};
  bgBinds[0].binding = 0;
  bgBinds[0].descriptorCount = 1;
  bgBinds[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;  // MVP uniform
  bgBinds[0].pImmutableSamplers = nullptr;
  bgBinds[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  bgBinds[1].binding = 3;
  bgBinds[1].descriptorCount = 1;
  bgBinds[1].descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;  // texture
  bgBinds[1].pImmutableSamplers = nullptr;
  bgBinds[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutCreateInfo bgBindInfo = {};
  bgBindInfo.bindingCount = 2;
  bgBindInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR;
  bgBindInfo.pNext = nullptr;
  bgBindInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  bgBindInfo.pBindings = bgBinds;

  vkCreateDescriptorSetLayout(Device, &bgBindInfo, nullptr,
                              &BgTextureSetLayout);

  VkVertexInputBindingDescription backgroundBindingDescription{};
  backgroundBindingDescription.binding = 0;
  backgroundBindingDescription.stride = sizeof(BgVertexBuffer);
  backgroundBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  VkVertexInputAttributeDescription backgroundAttributeDescriptions[2] = {};
  backgroundAttributeDescriptions[0].binding = 0;
  backgroundAttributeDescriptions[0].location = 0;
  backgroundAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  backgroundAttributeDescriptions[0].offset =
      offsetof(BgVertexBuffer, Position);
  backgroundAttributeDescriptions[1].binding = 0;
  backgroundAttributeDescriptions[1].location = 1;
  backgroundAttributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
  backgroundAttributeDescriptions[1].offset = offsetof(BgVertexBuffer, UV);
  PipelineBackground = new Pipeline(Device, renderPass);
  PipelineBackground->SetPushConstants(&pushConstant, 1);
  PipelineBackground->CreateWithShader(
      "Renderable3D_Background", backgroundBindingDescription,
      backgroundAttributeDescriptions, 2, BgTextureSetLayout);

  Texture texDummy;
  texDummy.Load1x1();
  TextureDummy = texDummy.Submit();
}

void Renderable3D::BeginFrame(IScene3D* scene, Camera* camera) {
  CurrentMaterial = MT_None;

  SceneUniformBufferType entry{};
  entry.ViewProjection = camera->ViewProjection;
  entry.Tint = scene->Tint;
  entry.WorldLightPosition = scene->LightPosition;
  entry.WorldEyePosition = camera->CameraTransform.Position;
  entry.DarkMode = scene->DarkMode;
  memcpy(SceneUniformBuffersMapped[CurrentFrameIndex], &entry,
         sizeof(SceneUniformBufferType));

  ViewProjection = camera->ViewProjection;
}

bool Renderable3D::LoadSync(uint32_t modelId) {
  assert(IsUsed == false);

  ImpLog(LL_Info, LC_Renderable3D, "Creating renderable (model ID %d)\n",
         modelId);

  StaticModel = Model::Load(modelId);
  ModelTransform = Transform();
  PrevPoseWeight = 0.0f;

  if (!StaticModel) {
    ImpLog(LL_Error, LC_Renderable3D,
           "Model loading failed for character with model ID %d\n");
    return false;
  }

  InitMeshAnimStatus();
  ReloadDefaultBoneTransforms();

  Animator.Character = this;
  SwitchAnimation(StaticModel->IdleAnimation, 0.0f);

  IsUsed = true;

  return true;
}

void Renderable3D::MakePlane() {
  assert(IsUsed == false);

  StaticModel = Model::MakePlane();
  ModelTransform = Transform();
  PrevPoseWeight = 0.0f;

  Animator.Character = this;

  InitMeshAnimStatus();
  ReloadDefaultBoneTransforms();

  IsUsed = true;
}

void Renderable3D::InitMeshAnimStatus() {
  int totalMorphedVertices = 0;
  for (uint32_t i = 0; i < StaticModel->MeshCount; i++) {
    MeshAnimStatus[i].MorphedVerticesOffset = totalMorphedVertices;
    if (StaticModel->Meshes[i].MorphTargetCount > 0) {
      totalMorphedVertices += StaticModel->Meshes[i].VertexCount;
    }
  }
  if (totalMorphedVertices > 0) {
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
        CurrentMorphedVerticesVk[i] = CreateBuffer(
            totalMorphedVertices * sizeof(VertexBufferDaSH),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
      } else {
        CurrentMorphedVerticesVk[i] = CreateBuffer(
            totalMorphedVertices * sizeof(VertexBuffer),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
      }
      vmaMapMemory(Allocator, CurrentMorphedVerticesVk[i].Allocation,
                   &CurrentMorphedVerticesVkMapped[i]);
    }
  }
  ReloadDefaultMeshAnimStatus();
}

void Renderable3D::ReloadDefaultMeshAnimStatus() {
  /*
  void* currentMorphedVertex;
  if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
    currentMorphedVertex =
        ((VertexBufferDaSH*)CurrentMorphedVerticesVkMapped[CurrentFrameIndex]);
  } else {
    currentMorphedVertex =
        ((VertexBuffer*)CurrentMorphedVerticesVkMapped[CurrentFrameIndex]);
  }
  VertexBuffer* currentMorphedVertexRNE =
  (VertexBuffer*)currentMorphedVertex; VertexBufferDaSH*
  currentMorphedVertexDaSH =
      (VertexBufferDaSH*)currentMorphedVertex;
  */

  for (uint32_t i = 0; i < StaticModel->MeshCount; i++) {
    MeshAnimStatus[i].Visible = 1.0f;
    if (StaticModel->Meshes[i].MorphTargetCount > 0) {
      for (int j = 0; j < StaticModel->Meshes[i].MorphTargetCount; j++) {
        MeshAnimStatus[i].MorphInfluences[j] = 0.0f;
      }
    }
  }
}

void Renderable3D::SwitchAnimation(int16_t animId, float transitionTime) {
  if (Animator.CurrentAnimation != 0 && transitionTime > 0.0f) {
    PrevPoseWeight = 1.0f;
    for (uint32_t i = 0; i < StaticModel->BoneCount; i++) {
      PrevBoneTransforms[i] = CurrentPose[i].LocalTransform;
    }
    memcpy(PrevMeshAnimStatus, MeshAnimStatus, sizeof(MeshAnimStatus));
    AnimationTransitionTime = transitionTime;
  } else {
    PrevPoseWeight = 0.0f;
  }
  Animator.Start(animId);
}

void Renderable3D::ReloadDefaultBoneTransforms() {
  for (uint32_t i = 0; i < StaticModel->BoneCount; i++) {
    CurrentPose[i].LocalTransform = StaticModel->Bones[i].BaseTransform;
  }
}

void Renderable3D::CalculateMorphedVertices(int id) {
  Mesh* mesh = &StaticModel->Meshes[id];
  AnimatedMesh* animStatus = &MeshAnimStatus[id];
  AnimatedMesh* prevAnimStatus = &PrevMeshAnimStatus[id];
  if (mesh->MorphTargetCount == 0) return;

  void* currentMorphedVertex;
  if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
    currentMorphedVertex =
        ((VertexBufferDaSH*)CurrentMorphedVerticesVkMapped[CurrentFrameIndex]) +
        animStatus->MorphedVerticesOffset;
  } else {
    currentMorphedVertex =
        ((VertexBuffer*)CurrentMorphedVerticesVkMapped[CurrentFrameIndex]) +
        animStatus->MorphedVerticesOffset;
  }
  VertexBuffer* currentMorphedVertexRNE = (VertexBuffer*)currentMorphedVertex;
  VertexBufferDaSH* currentMorphedVertexDaSH =
      (VertexBufferDaSH*)currentMorphedVertex;

  void* currentVertex;
  if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
    currentVertex =
        ((VertexBufferDaSH*)StaticModel->VertexBuffers) + mesh->VertexOffset;
  } else {
    currentVertex =
        ((VertexBuffer*)StaticModel->VertexBuffers) + mesh->VertexOffset;
  }

  VertexBuffer* currentVertexRNE = (VertexBuffer*)currentVertex;
  VertexBufferDaSH* currentVertexDaSH = (VertexBufferDaSH*)currentVertex;

  for (uint32_t j = 0; j < mesh->VertexCount; j++) {
    if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
      memcpy(currentMorphedVertexDaSH, currentVertexDaSH,
             sizeof(VertexBufferDaSH));
      currentMorphedVertexDaSH++;
      currentVertexDaSH++;
    } else {
      memcpy(currentMorphedVertexRNE, currentVertexRNE, sizeof(VertexBuffer));
      currentMorphedVertexRNE++;
      currentVertexRNE++;
    }
  }

  for (int k = 0; k < mesh->MorphTargetCount; k++) {
    float influence;
    if (PrevPoseWeight > 0.0f) {
      influence = glm::mix(prevAnimStatus->MorphInfluences[k],
                           animStatus->MorphInfluences[k],
                           glm::smoothstep(0.0f, 1.0f, 1.0f - PrevPoseWeight));
    } else {
      influence = animStatus->MorphInfluences[k];
    }

    if (influence == 0.0f) continue;

    if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
      currentMorphedVertex =
          ((VertexBufferDaSH*)
               CurrentMorphedVerticesVkMapped[CurrentFrameIndex]) +
          animStatus->MorphedVerticesOffset;
    } else {
      currentMorphedVertex =
          ((VertexBuffer*)CurrentMorphedVerticesVkMapped[CurrentFrameIndex]) +
          animStatus->MorphedVerticesOffset;
    }
    currentMorphedVertexRNE = (VertexBuffer*)currentMorphedVertex;
    currentMorphedVertexDaSH = (VertexBufferDaSH*)currentMorphedVertex;

    if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
      currentVertex =
          ((VertexBufferDaSH*)StaticModel->VertexBuffers) + mesh->VertexOffset;
    } else {
      currentVertex =
          ((VertexBuffer*)StaticModel->VertexBuffers) + mesh->VertexOffset;
    }

    currentVertexRNE = (VertexBuffer*)currentVertex;
    currentVertexDaSH = (VertexBufferDaSH*)currentVertex;

    MorphVertexBuffer* currentMorphTargetVbo =
        StaticModel->MorphVertexBuffers +
        StaticModel->MorphTargets[mesh->MorphTargetIds[k]].VertexOffset;

    for (uint32_t j = 0; j < mesh->VertexCount; j++) {
      if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
        currentMorphedVertexDaSH->Position +=
            (currentMorphTargetVbo->Position - currentVertexDaSH->Position) *
            influence;
        currentMorphedVertexDaSH->Normal +=
            (currentMorphTargetVbo->Normal - currentVertexDaSH->Normal) *
            influence;
        currentVertexDaSH++;
        currentMorphedVertexDaSH++;
      } else {
        currentMorphedVertexRNE->Position +=
            (currentMorphTargetVbo->Position - currentVertexRNE->Position) *
            influence;
        currentMorphedVertexRNE->Normal +=
            (currentMorphTargetVbo->Normal - currentVertexRNE->Normal) *
            influence;
        currentVertexRNE++;
        currentMorphedVertexRNE++;
      }
      currentMorphTargetVbo++;
    }
  }
}

void Renderable3D::Pose() {
  for (int i = 0; i < StaticModel->RootBoneCount; i++) {
    PoseBone(StaticModel->RootBones[i]);
  }
}

void Renderable3D::PoseBone(int16_t id) {
  StaticBone* bone = &StaticModel->Bones[id];
  PosedBone* transformed = &CurrentPose[id];

  glm::mat4 parentWorld;
  if (bone->Parent < 0) {
    parentWorld = glm::mat4(1.0);
  } else {
    parentWorld = CurrentPose[bone->Parent].World;
  }

  Transform transform;
  if (PrevPoseWeight > 0.0f) {
    transform = PrevBoneTransforms[id].Interpolate(
        transformed->LocalTransform,
        glm::smoothstep(0.0f, 1.0f, 1.0f - PrevPoseWeight));
  } else {
    transform = transformed->LocalTransform;
  }

  glm::mat4 local = transform.Matrix();

  transformed->World = parentWorld * local;

  for (int i = 0; i < bone->ChildrenCount; i++) {
    PoseBone(bone->Children[i]);
  }

  transformed->Offset = transformed->World * bone->BindInverse;
}

void Renderable3D::Update(float dt) {
  if (!IsUsed) return;
  if (Animator.CurrentAnimation) {
    if (!Animator.IsPlaying) {
      // oneshot ended
      SwitchAnimation(StaticModel->IdleAnimation, AnimationTransitionTime);
    } else {
      Animator.Update(dt);
    }
  }
  Pose();
  if (PrevPoseWeight > 0.0f) {
    PrevPoseWeight -= dt / AnimationTransitionTime;
    if (PrevPoseWeight < 0.0f) PrevPoseWeight = 0.0f;
  }

  for (uint32_t i = 0; i < StaticModel->MeshCount; i++) {
    CalculateMorphedVertices(i);
  }
}

void Renderable3D::DrawMesh(int id, RenderPass pass) {
  if (pass == RP_Outline && StaticModel->Type == ModelType_Background) return;

  if (!MeshAnimStatus[id].Visible) return;

  Mesh& mesh = StaticModel->Meshes[id];

  if (pass == RP_First && mesh.Flags & MeshFlag_Later) return;
  if (pass == RP_Second && !(mesh.Flags & MeshFlag_Later)) return;

  if (pass == RP_Outline) {
    UseMaterial(MT_Outline);
  } else {
    UseMaterial(mesh.Material);
  }
  // Because of the above, we use CurrentMaterial later, not mesh.Material

  UseMesh(id);

  VkDescriptorBufferInfo bgMvpBufferInfo{};
  VkDescriptorBufferInfo sceneBufferInfo{};
  VkDescriptorBufferInfo modelBufferInfo{};
  VkDescriptorBufferInfo meshBufferInfo{};
  switch (CurrentMaterial) {
    case MT_Background: {
      bgMvpBufferInfo.buffer =
          BackgroundMvpBuffers[id][CurrentFrameIndex].Buffer;
      bgMvpBufferInfo.offset = 0;
      bgMvpBufferInfo.range = sizeof(BgMVPUniformBufferType);

      WriteDescriptorSet[CurrentWriteDescriptorSet].sType =
          VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      WriteDescriptorSet[CurrentWriteDescriptorSet].dstSet = 0;
      WriteDescriptorSet[CurrentWriteDescriptorSet].dstBinding = 0;
      WriteDescriptorSet[CurrentWriteDescriptorSet].descriptorCount = 1;
      WriteDescriptorSet[CurrentWriteDescriptorSet].descriptorType =
          VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      WriteDescriptorSet[CurrentWriteDescriptorSet].pBufferInfo =
          &bgMvpBufferInfo;
      CurrentWriteDescriptorSet += 1;
    } break;
    case MT_Generic:
    case MT_Eye:
    case MT_Outline:
    case MT_DaSH_Generic:
    case MT_DaSH_Face:
    case MT_DaSH_Skin:
    case MT_DaSH_Eye: {
      sceneBufferInfo.buffer = SceneUniformBuffers[CurrentFrameIndex].Buffer;
      sceneBufferInfo.offset = 0;
      sceneBufferInfo.range = sizeof(SceneUniformBufferType);

      WriteDescriptorSet[CurrentWriteDescriptorSet].sType =
          VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      WriteDescriptorSet[CurrentWriteDescriptorSet].dstSet = 0;
      WriteDescriptorSet[CurrentWriteDescriptorSet].dstBinding = 0;
      WriteDescriptorSet[CurrentWriteDescriptorSet].descriptorCount = 1;
      WriteDescriptorSet[CurrentWriteDescriptorSet].descriptorType =
          VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      WriteDescriptorSet[CurrentWriteDescriptorSet].pBufferInfo =
          &sceneBufferInfo;
      CurrentWriteDescriptorSet += 1;

      modelBufferInfo.buffer = ModelUniformBuffers[CurrentFrameIndex].Buffer;
      modelBufferInfo.offset = 0;
      modelBufferInfo.range = sizeof(ModelUniformBufferType);

      WriteDescriptorSet[CurrentWriteDescriptorSet].sType =
          VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      WriteDescriptorSet[CurrentWriteDescriptorSet].dstSet = 0;
      WriteDescriptorSet[CurrentWriteDescriptorSet].dstBinding = 1;
      WriteDescriptorSet[CurrentWriteDescriptorSet].descriptorCount = 1;
      WriteDescriptorSet[CurrentWriteDescriptorSet].descriptorType =
          VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      WriteDescriptorSet[CurrentWriteDescriptorSet].pBufferInfo =
          &modelBufferInfo;
      CurrentWriteDescriptorSet += 1;

      meshBufferInfo.buffer = MeshUniformBuffers[id][CurrentFrameIndex].Buffer;
      meshBufferInfo.offset = 0;
      meshBufferInfo.range = sizeof(MeshUniformBufferType);

      WriteDescriptorSet[CurrentWriteDescriptorSet].sType =
          VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      WriteDescriptorSet[CurrentWriteDescriptorSet].dstSet = 0;
      WriteDescriptorSet[CurrentWriteDescriptorSet].dstBinding = 2;
      WriteDescriptorSet[CurrentWriteDescriptorSet].descriptorCount = 1;
      WriteDescriptorSet[CurrentWriteDescriptorSet].descriptorType =
          VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      WriteDescriptorSet[CurrentWriteDescriptorSet].pBufferInfo =
          &meshBufferInfo;
      CurrentWriteDescriptorSet += 1;

    } break;
    default:
      ImpLog(LL_Warning, LC_Renderable3D, "Unknown material!\n");
      break;
  }

  int textureCount = 0;

  switch (CurrentMaterial) {
    case MT_Background: {
      int const backgroundTextureTypes[] = {TT_ColorMap};
      textureCount = 1;
      SetTextures(id, backgroundTextureTypes, 1);
      break;
    }
    case MT_Outline: {
      if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
        int const outlineTextureTypes[] = {TT_DaSH_ColorMap, TT_DaSH_NoiseMap};
        textureCount = 2;
        SetTextures(id, outlineTextureTypes, 2);
      } else {
        int const outlineTextureTypes[] = {TT_ColorMap};
        textureCount = 1;
        SetTextures(id, outlineTextureTypes, 1);
      }
      break;
    }
    case MT_Generic: {
      int const genericTextureTypes[] = {TT_ColorMap, TT_GradientMaskMap,
                                         TT_SpecularColorMap};
      textureCount = 3;
      SetTextures(id, genericTextureTypes, 3);
      break;
    }
    case MT_Eye: {
      int const eyeTextureTypes[] = {TT_Eye_IrisColorMap, TT_Eye_WhiteColorMap,
                                     TT_Eye_HighlightColorMap,
                                     TT_Eye_IrisSpecularColorMap};
      textureCount = 4;
      SetTextures(id, eyeTextureTypes, 4);
      break;
    }
    case MT_DaSH_Generic: {
      int const dashGenericTextureTypes[] = {
          TT_DaSH_ColorMap, TT_DaSH_GradientMaskMap, TT_DaSH_SpecularColorMap,
          TT_DaSH_ShadowColorMap};
      textureCount = 4;
      SetTextures(id, dashGenericTextureTypes, 4);
      break;
    }
    case MT_DaSH_Face:
    case MT_DaSH_Skin: {
      int const dashGenericTextureTypes[] = {
          TT_DaSH_ColorMap, TT_DaSH_GradientMaskMap, TT_DaSH_SpecularColorMap};
      textureCount = 3;
      SetTextures(id, dashGenericTextureTypes, 3);
      break;
    }
    case MT_DaSH_Eye: {
      int const dashEyeTextureTypes[] = {TT_DaSH_Eye_IrisColorMap,
                                         TT_DaSH_Eye_WhiteColorMap,
                                         TT_DaSH_Eye_HighlightColorMap};
      textureCount = 3;
      SetTextures(id, dashEyeTextureTypes, 3);
      break;
    }
    default:
      ImpLog(LL_Warning, LC_Renderable3D, "Unknown material!\n");
      break;
  }

  CurrentTextureBufferInfo = 0;

  if (mesh.Opacity < 0.9 && CurrentMaterialIsDepthWrite) {
    switch (CurrentMaterial) {
      case MT_Generic:
      case MT_DaSH_Generic:
      case MT_DaSH_Face:
      case MT_DaSH_Skin: {
        vkCmdBindPipeline(CommandBuffers[CurrentFrameIndex],
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          PipelineMainNoDepthWrite->GraphicsPipeline);
        CurrentPipeline = PipelineMainNoDepthWrite;
      } break;
      case MT_Eye:
      case MT_DaSH_Eye: {
        vkCmdBindPipeline(CommandBuffers[CurrentFrameIndex],
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          PipelineEyeNoDepthWrite->GraphicsPipeline);
        CurrentPipeline = PipelineEyeNoDepthWrite;
        break;
      }
      default:
        ImpLog(LL_Warning, LC_Renderable3D, "Unknown material!\n");
        break;
    }
  }

  WriteDescriptorSet[CurrentWriteDescriptorSet].sType =
      VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  WriteDescriptorSet[CurrentWriteDescriptorSet].dstSet = 0;
  WriteDescriptorSet[CurrentWriteDescriptorSet].dstBinding = 3;
  WriteDescriptorSet[CurrentWriteDescriptorSet].descriptorCount = textureCount;
  WriteDescriptorSet[CurrentWriteDescriptorSet].descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  WriteDescriptorSet[CurrentWriteDescriptorSet].pImageInfo = TextureBufferInfo;
  CurrentWriteDescriptorSet += 1;

  vkCmdPushDescriptorSetKHR(CommandBuffers[CurrentFrameIndex],
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            CurrentPipeline->PipelineLayout, 0,
                            CurrentWriteDescriptorSet, WriteDescriptorSet);
  CurrentWriteDescriptorSet = 0;

  PipelinePushConstants pushConstants{Profile::Scene3D::Version ==
                                      +LKMVersion::DaSH};
  vkCmdPushConstants(CommandBuffers[CurrentFrameIndex],
                     CurrentPipeline->PipelineLayout,
                     VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT,
                     0, sizeof(PipelinePushConstants), &pushConstants);

  if (StaticModel->Meshes[id].MorphTargetCount > 0) {
    VkBuffer vertexBuffers[] = {
        CurrentMorphedVerticesVk[CurrentFrameIndex].Buffer};
    size_t stride = Profile::Scene3D::Version == +LKMVersion::DaSH
                        ? sizeof(VertexBufferDaSH)
                        : sizeof(VertexBuffer);
    VkDeviceSize offsets[] = {
        (VkDeviceSize)(MeshAnimStatus[id].MorphedVerticesOffset * stride)};
    vkCmdBindVertexBuffers(CommandBuffers[CurrentFrameIndex], 0, 1,
                           vertexBuffers, offsets);
  } else {
    VkBuffer vertexBuffers[] = {
        MeshVertexBuffers[id][CurrentFrameIndex].Buffer};
    VkDeviceSize offsets[] = {(VkDeviceSize)0};
    vkCmdBindVertexBuffers(CommandBuffers[CurrentFrameIndex], 0, 1,
                           vertexBuffers, offsets);
  }
  vkCmdBindIndexBuffer(CommandBuffers[CurrentFrameIndex],
                       MeshIndexBuffers[id][CurrentFrameIndex].Buffer, 0,
                       VK_INDEX_TYPE_UINT16);
  vkCmdDrawIndexed(CommandBuffers[CurrentFrameIndex], mesh.IndexCount, 1, 0, 0,
                   0);

  if (mesh.Opacity < 0.9 && CurrentMaterialIsDepthWrite) {
    switch (CurrentMaterial) {
      case MT_Generic:
      case MT_DaSH_Generic:
      case MT_DaSH_Face:
      case MT_DaSH_Skin: {
        vkCmdBindPipeline(CommandBuffers[CurrentFrameIndex],
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          PipelineMain->GraphicsPipeline);
        CurrentPipeline = PipelineMain;
      } break;
      case MT_Eye:
      case MT_DaSH_Eye: {
        vkCmdBindPipeline(CommandBuffers[CurrentFrameIndex],
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          PipelineEye->GraphicsPipeline);
        CurrentPipeline = PipelineEye;
        break;
      }
      default:
        ImpLog(LL_Warning, LC_Renderable3D, "Unknown material!\n");
        break;
    }
  }

  if (pass != RP_Outline) DrawMesh(id, RP_Outline);
}

void Renderable3D::Render() {
  if (!IsUsed || !IsVisible) return;

  LoadModelUniforms();

  memset(UniformsUpdated, 0, sizeof(UniformsUpdated));
  CurrentTextureBufferInfo = 0;
  CurrentWriteDescriptorSet = 0;

  for (int i = RP_First; i < RP_Count; i++) {
    for (size_t j = 0; j < StaticModel->MeshCount; j++) {
      DrawMesh((int)j, (RenderPass)i);
    }
  }

  if (CurrentMaterial == MT_Outline) {
    // Clean up some state
    UseMaterial(MT_Background);
  }
}

void Renderable3D::SetTextures(int id, int const* textureTypes, int count) {
  for (int i = 0; i < count; i++) {
    int t = textureTypes[i];
    if (StaticModel->Meshes[id].Maps[t] >= 0) {
      TextureBufferInfo[CurrentTextureBufferInfo].sampler = Sampler;
      TextureBufferInfo[CurrentTextureBufferInfo].imageView =
          Textures[TexBuffers[StaticModel->Meshes[id].Maps[t]]].ImageView;
      TextureBufferInfo[CurrentTextureBufferInfo].imageLayout =
          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    } else {
      TextureBufferInfo[CurrentTextureBufferInfo].sampler = Sampler;
      TextureBufferInfo[CurrentTextureBufferInfo].imageView =
          Textures[TextureDummy].ImageView;
      TextureBufferInfo[CurrentTextureBufferInfo].imageLayout =
          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }
    CurrentTextureBufferInfo += 1;
  }
}

void Renderable3D::LoadModelUniforms() {
  if (StaticModel->Type == ModelType_Character) {
    ModelUniformBufferType entry{};
    entry.Model = ModelTransform.Matrix();
    memcpy(ModelUniformBuffersMapped[CurrentFrameIndex], &entry,
           sizeof(ModelUniformBufferType));
  }
}

void Renderable3D::UseMaterial(MaterialType type) {
  if (CurrentMaterial == type) return;

  switch (type) {
    case MT_Outline: {
      vkCmdBindPipeline(CommandBuffers[CurrentFrameIndex],
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        PipelineOutline->GraphicsPipeline);
      CurrentPipeline = PipelineOutline;
      break;
    }
    case MT_Background: {
      vkCmdBindPipeline(CommandBuffers[CurrentFrameIndex],
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        PipelineBackground->GraphicsPipeline);
      CurrentPipeline = PipelineBackground;
      break;
    }
    case MT_Generic:
    case MT_DaSH_Generic:
    case MT_DaSH_Face:
    case MT_DaSH_Skin: {
      vkCmdBindPipeline(CommandBuffers[CurrentFrameIndex],
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        PipelineMain->GraphicsPipeline);
      CurrentPipeline = PipelineMain;
      break;
    }
    case MT_Eye:
    case MT_DaSH_Eye: {
      vkCmdBindPipeline(CommandBuffers[CurrentFrameIndex],
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        PipelineEye->GraphicsPipeline);
      CurrentPipeline = PipelineEye;
      break;
    }
    default:
      ImpLog(LL_Warning, LC_Renderable3D, "Unknown material!\n");
      break;
  }

  if (type == MT_Outline) {
    CurrentMaterialIsDepthWrite = false;
  } else {
    CurrentMaterialIsDepthWrite = true;
  }

  CurrentMaterial = type;
}

void Renderable3D::UseMesh(int id) { LoadMeshUniforms(id); }

void Renderable3D::LoadMeshUniforms(int id) {
  Mesh& mesh = StaticModel->Meshes[id];

  if (StaticModel->Type == ModelType_Character) {
    MeshUniformBufferType entry{};
    if (!UniformsUpdated[id]) {
      if (mesh.UsedBones > 0) {
        glm::mat4* outBone = entry.Bones;
        for (uint32_t j = 0; j < mesh.UsedBones; j++) {
          memcpy(outBone, glm::value_ptr(CurrentPose[mesh.BoneMap[j]].Offset),
                 sizeof(glm::mat4));
          outBone++;
        }
      } else {
        entry.Bones[0] = CurrentPose[mesh.MeshBone].Offset;
      }

      entry.ModelOpacity = mesh.Opacity;
      entry.HasShadowColorMap =
          mesh.Material == MT_DaSH_Generic && mesh.HasShadowColorMap;

      memcpy(MeshUniformBuffersMapped[id][CurrentFrameIndex], &entry,
             sizeof(MeshUniformBufferType));

      UniformsUpdated[id] = true;
    }
  } else if (StaticModel->Type == ModelType_Background) {
    BgMVPUniformBufferType entry{};
    entry.MVP = ViewProjection * mesh.ModelTransform.Matrix();
    memcpy(BackgroundMvpMapped[id][CurrentFrameIndex], &entry,
           sizeof(BgMVPUniformBufferType));
  }
}

void Renderable3D::UnloadSync() {
  Animator.CurrentAnimation = 0;
  PrevPoseWeight = 0.0f;
  if (StaticModel) {
    ImpLog(LL_Info, LC_Renderable3D, "Unloading model %d\n", StaticModel->Id);
    if (IsSubmitted) {
      if (StaticModel->Type == ModelType_Background) {
        for (uint32_t i = 0; i < StaticModel->MeshCount; i++) {
          for (uint32_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j++) {
            vmaUnmapMemory(Allocator, BackgroundMvpBuffers[i][j].Allocation);
            vmaDestroyBuffer(Allocator, BackgroundMvpBuffers[i][j].Buffer,
                             BackgroundMvpBuffers[i][j].Allocation);
          }
        }
      } else {
        for (uint32_t i = 0; i < StaticModel->MeshCount; i++) {
          for (uint32_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j++) {
            vmaUnmapMemory(Allocator, MeshUniformBuffers[i][j].Allocation);
            vmaDestroyBuffer(Allocator, MeshUniformBuffers[i][j].Buffer,
                             MeshUniformBuffers[i][j].Allocation);
          }
        }
      }

      for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vmaUnmapMemory(Allocator, ModelUniformBuffers[i].Allocation);
        vmaDestroyBuffer(Allocator, ModelUniformBuffers[i].Buffer,
                         ModelUniformBuffers[i].Allocation);
      }
    }
    delete StaticModel;
    StaticModel = 0;
  }
  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (CurrentMorphedVerticesVkMapped[i]) {
      vmaUnmapMemory(Allocator, CurrentMorphedVerticesVk[i].Allocation);
      CurrentMorphedVerticesVkMapped[i] = 0;
      vmaDestroyBuffer(Allocator, CurrentMorphedVerticesVk[i].Buffer,
                       CurrentMorphedVerticesVk[i].Allocation);
    }
  }
  if (CurrentMorphedVertices) {
    free(CurrentMorphedVertices);
    CurrentMorphedVertices = 0;
  }
  ModelTransform = Transform();
  IsSubmitted = false;
  IsUsed = false;
  IsVisible = false;
}

void Renderable3D::MainThreadOnLoad() {
  assert(IsSubmitted == false);

  ImpLog(LL_Info, LC_Renderable3D, "Submitting data to GPU for model ID %d\n",
         StaticModel->Id);

  if (StaticModel->Type == ModelType_Background) {
    for (uint32_t i = 0; i < StaticModel->MeshCount; i++) {
      for (uint32_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j++) {
        BackgroundMvpBuffers[i][j] = CreateBuffer(
            sizeof(BgMVPUniformBufferType), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VMA_MEMORY_USAGE_CPU_ONLY);
        void* data;
        vmaMapMemory(Allocator, BackgroundMvpBuffers[i][j].Allocation, &data);
        BackgroundMvpMapped[i][j] = (uint8_t*)data;
      }
    }
  }

  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    ModelUniformBuffers[i] = CreateBuffer(sizeof(ModelUniformBufferType),
                                          VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                          VMA_MEMORY_USAGE_CPU_ONLY);
    void* data;
    vmaMapMemory(Allocator, ModelUniformBuffers[i].Allocation, &data);
    ModelUniformBuffersMapped[i] = (uint8_t*)data;
  }

  auto stagingBuffer =
      CreateBuffer(StagingBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VMA_MEMORY_USAGE_CPU_ONLY);

  void* stagingBufferMapped;
  vmaMapMemory(Allocator, stagingBuffer.Allocation, &stagingBufferMapped);

  for (uint32_t i = 0; i < StaticModel->MeshCount; i++) {
    for (uint32_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j++) {
      MeshUniformBuffers[i][j] = CreateBuffer(
          sizeof(MeshUniformBufferType), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
          VMA_MEMORY_USAGE_CPU_ONLY);
      void* data;
      vmaMapMemory(Allocator, MeshUniformBuffers[i][j].Allocation, &data);
      MeshUniformBuffersMapped[i][j] = (uint8_t*)data;

      uint32_t vertexCopySize = 0;

      if (StaticModel->Type == ModelType_Character) {
        if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
          MeshVertexBuffers[i][j] = CreateBuffer(
              sizeof(VertexBufferDaSH) * StaticModel->Meshes[i].VertexCount,
              VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
              VMA_MEMORY_USAGE_CPU_ONLY);
          vertexCopySize =
              sizeof(VertexBufferDaSH) * StaticModel->Meshes[i].VertexCount;

          memcpy(stagingBufferMapped,
                 (VertexBufferDaSH*)StaticModel->VertexBuffers +
                     StaticModel->Meshes[i].VertexOffset,
                 sizeof(VertexBufferDaSH) * StaticModel->Meshes[i].VertexCount);
        } else {
          MeshVertexBuffers[i][j] = CreateBuffer(
              sizeof(VertexBuffer) * StaticModel->Meshes[i].VertexCount,
              VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
              VMA_MEMORY_USAGE_GPU_ONLY);
          vertexCopySize =
              sizeof(VertexBuffer) * StaticModel->Meshes[i].VertexCount;

          memcpy(stagingBufferMapped,
                 (VertexBuffer*)StaticModel->VertexBuffers +
                     StaticModel->Meshes[i].VertexOffset,
                 sizeof(VertexBuffer) * StaticModel->Meshes[i].VertexCount);
        }
      } else if (StaticModel->Type == ModelType_Background) {
        MeshVertexBuffers[i][j] = CreateBuffer(
            sizeof(BgVertexBuffer) * StaticModel->Meshes[i].VertexCount,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VMA_MEMORY_USAGE_GPU_ONLY);
        vertexCopySize =
            sizeof(BgVertexBuffer) * StaticModel->Meshes[i].VertexCount;

        memcpy(stagingBufferMapped,
               (BgVertexBuffer*)StaticModel->VertexBuffers +
                   StaticModel->Meshes[i].VertexOffset,
               sizeof(BgVertexBuffer) * StaticModel->Meshes[i].VertexCount);
      }

      MeshIndexBuffers[i][j] = CreateBuffer(
          sizeof(uint16_t) * StaticModel->Meshes[i].IndexCount,
          VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
          VMA_MEMORY_USAGE_GPU_ONLY);

      memcpy((uint8_t*)stagingBufferMapped + vertexCopySize,
             StaticModel->Indices + StaticModel->Meshes[i].IndexOffset,
             sizeof(uint16_t) * StaticModel->Meshes[i].IndexCount);

      ImmediateSubmit([=](VkCommandBuffer cmd) {
        VkBufferCopy copy;
        copy.dstOffset = 0;
        copy.srcOffset = 0;
        copy.size = vertexCopySize;
        vkCmdCopyBuffer(cmd, stagingBuffer.Buffer,
                        MeshVertexBuffers[i][j].Buffer, 1, &copy);

        copy.dstOffset = 0;
        copy.srcOffset = vertexCopySize;
        copy.size = sizeof(uint16_t) * StaticModel->Meshes[i].IndexCount;
        vkCmdCopyBuffer(cmd, stagingBuffer.Buffer,
                        MeshIndexBuffers[i][j].Buffer, 1, &copy);
      });
    }
  }

  for (uint32_t i = 0; i < StaticModel->TextureCount; i++) {
    TexBuffers[i] = StaticModel->Textures[i].Submit();
    if (TexBuffers[i] == 0) {
      ImpLog(LL_Fatal, LC_Renderable3D,
             "Submitting texture %d for model %d failed\n", i, StaticModel->Id);
    }
  }

  vmaUnmapMemory(Allocator, stagingBuffer.Allocation);
  vmaDestroyBuffer(Allocator, stagingBuffer.Buffer, stagingBuffer.Allocation);

  IsSubmitted = true;
}

}  // namespace Vulkan
}  // namespace Impacto