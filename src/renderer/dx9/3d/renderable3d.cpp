#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "renderable3d.h"

#include "../renderer.h"
#include "../../3d/camera.h"
#include "../../3d/modelanimator.h"
#include "../../../log.h"
#include "../../../profile/scene3d.h"

namespace Impacto {
namespace DirectX9 {

static uint32_t TextureDummy = 0;

// character
static Shader *ShaderMain = 0, *ShaderOutline = 0, *ShaderEye = 0;

// background
static Shader* ShaderBackground = 0;

static glm::mat4 ViewProjection;

static bool IsInit = false;

static MaterialType CurrentMaterial = MT_None;
static bool CurrentMaterialIsDepthWrite = false;
static bool CurrentMaterialIsBackfaceCull = false;

static DirectX9Window* Window;
static IDirect3DDevice9* Device;

void Renderable3D::Init(DirectX9Window* window, IDirect3DDevice9* device) {
  assert(IsInit == false);
  ImpLog(LL_Info, LC_Renderable3D, "Initializing Renderable3D system\n");
  IsInit = true;

  Window = window;
  Device = device;

  IDirect3DVertexDeclaration9* modelVertexDeclaration;
  D3D_SHADER_MACRO* shaderMacros = nullptr;
  if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
    D3D_SHADER_MACRO dashMacros[] = {"IsDaSH", "1", NULL, NULL};
    shaderMacros = dashMacros;

    std::vector<D3DVERTEXELEMENT9> vertexDeclDesc{
        {0, offsetof(VertexBufferDaSH, Position), D3DDECLTYPE_FLOAT3,
         D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, offsetof(VertexBufferDaSH, Normal), D3DDECLTYPE_FLOAT3,
         D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
        {0, offsetof(VertexBufferDaSH, UV), D3DDECLTYPE_FLOAT2,
         D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        {0, offsetof(VertexBufferDaSH, BoneIndices), D3DDECLTYPE_FLOAT4,
         D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
        {0, offsetof(VertexBufferDaSH, BoneWeights), D3DDECLTYPE_FLOAT4,
         D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0},
        D3DDECL_END()};
    Device->CreateVertexDeclaration(vertexDeclDesc.data(),
                                    &modelVertexDeclaration);
  } else {
    std::vector<D3DVERTEXELEMENT9> vertexDeclDesc{
        {0, offsetof(VertexBuffer, Position), D3DDECLTYPE_FLOAT3,
         D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, offsetof(VertexBuffer, Normal), D3DDECLTYPE_FLOAT3,
         D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
        {0, offsetof(VertexBuffer, UV), D3DDECLTYPE_FLOAT2,
         D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        {0, offsetof(VertexBuffer, BoneIndices), D3DDECLTYPE_FLOAT4,
         D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
        {0, offsetof(VertexBuffer, BoneWeights), D3DDECLTYPE_FLOAT4,
         D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0},
        D3DDECL_END()};
    Device->CreateVertexDeclaration(vertexDeclDesc.data(),
                                    &modelVertexDeclaration);
  }

  ShaderMain = new Shader();
  ShaderMain->Compile("Renderable3D_Character", Device, modelVertexDeclaration,
                      shaderMacros);
  ShaderOutline = new Shader();
  ShaderOutline->Compile("Renderable3D_Outline", Device, modelVertexDeclaration,
                         shaderMacros);
  ShaderEye = new Shader();
  ShaderEye->Compile("Renderable3D_Eye", Device, modelVertexDeclaration,
                     shaderMacros);

  IDirect3DVertexDeclaration9* bgVertexDeclaration;
  std::vector<D3DVERTEXELEMENT9> vertexDeclDesc{
      {0, offsetof(BgVertexBuffer, Position), D3DDECLTYPE_FLOAT3,
       D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
      {0, offsetof(BgVertexBuffer, UV), D3DDECLTYPE_FLOAT2,
       D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
      D3DDECL_END()};
  Device->CreateVertexDeclaration(vertexDeclDesc.data(), &bgVertexDeclaration);
  ShaderBackground = new Shader();
  ShaderBackground->Compile("Renderable3D_Background", Device,
                            bgVertexDeclaration);

  Texture texDummy;
  texDummy.Load1x1();
  TextureDummy = texDummy.Submit();
}

void Renderable3D::BeginFrame(IScene3D* scene, Camera* camera) {
  CurrentMaterial = MT_None;

  /* SceneUniformBufferType entry{};
  entry.ViewProjection = camera->ViewProjection;
  entry.Tint = scene->Tint;
  entry.WorldLightPosition = scene->LightPosition;
  entry.WorldEyePosition = camera->CameraTransform.Position;
  entry.DarkMode = scene->DarkMode;
  memcpy(SceneUniformBuffersMapped[CurrentFrameIndex], &entry,
         sizeof(SceneUniformBufferType));*/

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
  for (int i = 0; i < StaticModel->MeshCount; i++) {
    MeshAnimStatus[i].MorphedVerticesOffset = totalMorphedVertices;
    if (StaticModel->Meshes[i].MorphTargetCount > 0) {
      totalMorphedVertices += StaticModel->Meshes[i].VertexCount;
    }
  }
  if (totalMorphedVertices > 0) {
    if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
      CurrentMorphedVerticesDx = (VertexBufferDaSH*)malloc(
          totalMorphedVertices * sizeof(VertexBufferDaSH));
    } else {
      CurrentMorphedVerticesDx =
          (VertexBuffer*)malloc(totalMorphedVertices * sizeof(VertexBuffer));
    }
  }
  ReloadDefaultMeshAnimStatus();
}

void Renderable3D::ReloadDefaultMeshAnimStatus() {
  void* currentMorphedVertex;
  if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
    currentMorphedVertex = ((VertexBufferDaSH*)CurrentMorphedVerticesDx);
  } else {
    currentMorphedVertex = ((VertexBuffer*)CurrentMorphedVerticesDx);
  }
  VertexBuffer* currentMorphedVertexRNE = (VertexBuffer*)currentMorphedVertex;
  VertexBufferDaSH* currentMorphedVertexDaSH =
      (VertexBufferDaSH*)currentMorphedVertex;

  for (int i = 0; i < StaticModel->MeshCount; i++) {
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
    for (int i = 0; i < StaticModel->BoneCount; i++) {
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
  for (int i = 0; i < StaticModel->BoneCount; i++) {
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
    currentMorphedVertex = ((VertexBufferDaSH*)CurrentMorphedVerticesDx) +
                           animStatus->MorphedVerticesOffset;
  } else {
    currentMorphedVertex = ((VertexBuffer*)CurrentMorphedVerticesDx) +
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

  for (int j = 0; j < mesh->VertexCount; j++) {
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
      currentMorphedVertex = ((VertexBufferDaSH*)CurrentMorphedVerticesDx) +
                             animStatus->MorphedVerticesOffset;
    } else {
      currentMorphedVertex = ((VertexBuffer*)CurrentMorphedVerticesDx) +
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

    for (int j = 0; j < mesh->VertexCount; j++) {
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

  for (int i = 0; i < StaticModel->MeshCount; i++) {
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

  /* VkDescriptorBufferInfo bgMvpBufferInfo{};
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
    }
  }*/

  if (pass != RP_Outline) DrawMesh(id, RP_Outline);
}

void Renderable3D::Render() {
  if (!IsUsed || !IsVisible) return;

  LoadModelUniforms();

  memset(UniformsUpdated, 0, sizeof(UniformsUpdated));
  // CurrentTextureBufferInfo = 0;
  // CurrentWriteDescriptorSet = 0;

  for (int i = RP_First; i < RP_Count; i++) {
    for (int j = 0; j < StaticModel->MeshCount; j++) {
      DrawMesh(j, (RenderPass)i);
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
    /*
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
    */
  }
}

void Renderable3D::LoadModelUniforms() {
  if (StaticModel->Type == ModelType_Character) {
    /*
    ModelUniformBufferType entry{};
    entry.Model = ModelTransform.Matrix();
    memcpy(ModelUniformBuffersMapped[CurrentFrameIndex], &entry,
           sizeof(ModelUniformBufferType));
           */
  }
}

void Renderable3D::UseMaterial(MaterialType type) {
  if (CurrentMaterial == type) return;

  switch (type) {
    case MT_Outline: {
      ShaderOutline->UseShader(Device);
      break;
    }
    case MT_Background: {
      ShaderBackground->UseShader(Device);
      break;
    }
    case MT_Generic:
    case MT_DaSH_Generic:
    case MT_DaSH_Face:
    case MT_DaSH_Skin: {
      ShaderMain->UseShader(Device);
      break;
    }
    case MT_Eye:
    case MT_DaSH_Eye: {
      ShaderEye->UseShader(Device);
      break;
    }
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

  /* if (StaticModel->Type == ModelType_Character) {
    MeshUniformBufferType entry{};
    if (mesh.UsedBones > 0) {
      glm::mat4* outBone = entry.Bones;
      for (int j = 0; j < mesh.UsedBones; j++) {
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

    Device->SetPixelShaderConstantF();
    Device->SetPixelShaderConstantF();
    memcpy(MeshUniformBuffersMapped[id][CurrentFrameIndex], &entry,
           sizeof(MeshUniformBufferType));
  } else if (StaticModel->Type == ModelType_Background) {
    BgMVPUniformBufferType entry{};
    entry.MVP = ViewProjection * mesh.ModelTransform.Matrix();
    memcpy(BackgroundMvpMapped[id][CurrentFrameIndex], &entry,
           sizeof(BgMVPUniformBufferType));
  }*/
}

void Renderable3D::UnloadSync() { /*
  Animator.CurrentAnimation = 0;
  PrevPoseWeight = 0.0f;
  if (StaticModel) {
    ImpLog(LL_Info, LC_Renderable3D, "Unloading model %d\n", StaticModel->Id);
    if (IsSubmitted) {
      if (StaticModel->Type == ModelType_Background) {
        for (int i = 0; i < StaticModel->MeshCount; i++) {
          for (int j = 0; j < MAX_FRAMES_IN_FLIGHT; j++) {
            vmaUnmapMemory(Allocator, BackgroundMvpBuffers[i][j].Allocation);
            vmaDestroyBuffer(Allocator, BackgroundMvpBuffers[i][j].Buffer,
                             BackgroundMvpBuffers[i][j].Allocation);
          }
        }
      } else {
        for (int i = 0; i < StaticModel->MeshCount; i++) {
          for (int j = 0; j < MAX_FRAMES_IN_FLIGHT; j++) {
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
  IsVisible = false;*/
}

void Renderable3D::MainThreadOnLoad() {
  assert(IsSubmitted == false);

  ImpLog(LL_Info, LC_Renderable3D, "Submitting data to GPU for model ID %d\n",
         StaticModel->Id);
  /*
  if (StaticModel->Type == ModelType_Background) {
    for (int i = 0; i < StaticModel->MeshCount; i++) {
      for (int j = 0; j < MAX_FRAMES_IN_FLIGHT; j++) {
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

  for (int i = 0; i < StaticModel->MeshCount; i++) {
    for (int j = 0; j < MAX_FRAMES_IN_FLIGHT; j++) {
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

  for (int i = 0; i < StaticModel->TextureCount; i++) {
    TexBuffers[i] = StaticModel->Textures[i].Submit();
    if (TexBuffers[i] == 0) {
      ImpLog(LL_Fatal, LC_Renderable3D,
             "Submitting texture %d for model %d failed\n", i, StaticModel->Id);
    }
  }

  vmaUnmapMemory(Allocator, stagingBuffer.Allocation);
  vmaDestroyBuffer(Allocator, stagingBuffer.Buffer, stagingBuffer.Allocation);*/

  IsSubmitted = true;
}

}  // namespace DirectX9
}  // namespace Impacto