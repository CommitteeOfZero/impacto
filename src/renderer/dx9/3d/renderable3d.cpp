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

static IScene3D* CurrentScene;
static Camera* CurrentCamera;

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
        {0, offsetof(VertexBufferDaSH, BoneIndices), D3DDECLTYPE_UBYTE4,
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
        {0, offsetof(VertexBuffer, BoneIndices), D3DDECLTYPE_UBYTE4,
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

  CurrentScene = scene;
  CurrentCamera = camera;
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

void Renderable3D::SetSceneUniformValues() {
  if (StaticModel->Type != ModelType_Background) {
    SceneUniformBufferType entry{};
    entry.ViewProjection = CurrentCamera->ViewProjection;
    entry.Tint = CurrentScene->Tint;
    entry.WorldLightPosition = CurrentScene->LightPosition;
    entry.WorldEyePosition = CurrentCamera->CameraTransform.Position;
    entry.DarkMode = CurrentScene->DarkMode;

    Device->SetPixelShaderConstantF(0, glm::value_ptr(entry.ViewProjection), 4);
    Device->SetPixelShaderConstantF(4, glm::value_ptr(entry.Tint), 1);
    Device->SetPixelShaderConstantF(5, glm::value_ptr(entry.WorldLightPosition),
                                    1);
    Device->SetPixelShaderConstantF(6, glm::value_ptr(entry.WorldEyePosition),
                                    1);
    Device->SetPixelShaderConstantB(0, (BOOL*)&entry.DarkMode, 1);

    Device->SetVertexShaderConstantF(0, glm::value_ptr(entry.ViewProjection),
                                     4);
    Device->SetVertexShaderConstantF(4, glm::value_ptr(entry.Tint), 1);
    Device->SetVertexShaderConstantF(
        5, glm::value_ptr(entry.WorldLightPosition), 1);
    Device->SetVertexShaderConstantF(6, glm::value_ptr(entry.WorldEyePosition),
                                     1);
    Device->SetVertexShaderConstantB(0, (BOOL*)&entry.DarkMode, 1);
  }
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
      Device->CreateVertexBuffer(
          totalMorphedVertices * sizeof(VertexBufferDaSH), 0, 0,
          D3DPOOL_MANAGED, &MorphedVerticesDevice, NULL);
    } else {
      Device->CreateVertexBuffer(totalMorphedVertices * sizeof(VertexBuffer), 0,
                                 0, D3DPOOL_MANAGED, &MorphedVerticesDevice,
                                 NULL);
    }
  }
  ReloadDefaultMeshAnimStatus();
}

void Renderable3D::ReloadDefaultMeshAnimStatus() {
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
  auto res = MorphedVerticesDevice->Lock(0, 0, &CurrentMorphedVerticesDx, 0);
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

  res = MorphedVerticesDevice->Unlock();
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

  switch (CurrentMaterial) {
    case MT_Background: {
      int const backgroundTextureTypes[] = {TT_ColorMap};
      SetTextures(id, backgroundTextureTypes, 1);
      break;
    }
    case MT_Outline: {
      if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
        int const outlineTextureTypes[] = {TT_DaSH_ColorMap, TT_DaSH_NoiseMap};
        SetTextures(id, outlineTextureTypes, 2);
      } else {
        int const outlineTextureTypes[] = {TT_ColorMap};
        SetTextures(id, outlineTextureTypes, 1);
      }
      break;
    }
    case MT_Generic: {
      int const genericTextureTypes[] = {TT_ColorMap, TT_GradientMaskMap,
                                         TT_SpecularColorMap};
      SetTextures(id, genericTextureTypes, 3);
      break;
    }
    case MT_Eye: {
      int const eyeTextureTypes[] = {TT_Eye_IrisColorMap, TT_Eye_WhiteColorMap,
                                     TT_Eye_HighlightColorMap,
                                     TT_Eye_IrisSpecularColorMap};
      SetTextures(id, eyeTextureTypes, 4);
      break;
    }
    case MT_DaSH_Generic: {
      int const dashGenericTextureTypes[] = {
          TT_DaSH_ColorMap, TT_DaSH_GradientMaskMap, TT_DaSH_SpecularColorMap,
          TT_DaSH_ShadowColorMap};
      SetTextures(id, dashGenericTextureTypes, 4);
      break;
    }
    case MT_DaSH_Face:
    case MT_DaSH_Skin: {
      int const dashGenericTextureTypes[] = {
          TT_DaSH_ColorMap, TT_DaSH_GradientMaskMap, TT_DaSH_SpecularColorMap};
      SetTextures(id, dashGenericTextureTypes, 3);
      break;
    }
    case MT_DaSH_Eye: {
      int const dashEyeTextureTypes[] = {TT_DaSH_Eye_IrisColorMap,
                                         TT_DaSH_Eye_WhiteColorMap,
                                         TT_DaSH_Eye_HighlightColorMap};
      SetTextures(id, dashEyeTextureTypes, 3);
      break;
    }
  }

  if (mesh.Opacity < 0.9 && CurrentMaterialIsDepthWrite) {
    Device->SetRenderState(D3DRS_ZWRITEENABLE, 0);
  }

  size_t stride = Profile::Scene3D::Version == +LKMVersion::DaSH
                      ? sizeof(VertexBufferDaSH)
                      : sizeof(VertexBuffer);
  if (StaticModel->Type == ModelType_Background)
    stride = sizeof(BgVertexBuffer);

  if (StaticModel->Meshes[id].MorphTargetCount > 0) {
    Device->SetStreamSource(0, MorphedVerticesDevice,
                            MeshAnimStatus[id].MorphedVerticesOffset * stride,
                            stride);

  } else {
    Device->SetStreamSource(0, MeshVertexBuffersDevice[id], 0, stride);
  }
  Device->SetIndices(MeshIndexBuffersDevice[id]);

  Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mesh.IndexCount, 0,
                               mesh.IndexCount / 3);

  if (mesh.Opacity < 0.9 && CurrentMaterialIsDepthWrite) {
    Device->SetRenderState(D3DRS_ZWRITEENABLE, 1);
  }

  if (pass != RP_Outline) DrawMesh(id, RP_Outline);
}

void Renderable3D::Render() {
  if (!IsUsed || !IsVisible) return;

  memset(UniformsUpdated, 0, sizeof(UniformsUpdated));

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
    if (StaticModel->Meshes[id].Maps[t] >= 0) {
      Device->SetTexture(i,
                         Textures[TexBuffers[StaticModel->Meshes[id].Maps[t]]]);
    } else {
      Device->SetTexture(i, Textures[TextureDummy]);
    }
  }
}

void Renderable3D::LoadModelUniforms() {
  if (StaticModel->Type == ModelType_Character) {
    ModelUniformBufferType entry{};
    entry.Model = ModelTransform.Matrix();
    Device->SetVertexShaderConstantF(7, glm::value_ptr(entry.Model), 4);
    Device->SetPixelShaderConstantF(7, glm::value_ptr(entry.Model), 4);
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

  SetSceneUniformValues();
  LoadModelUniforms();

  if (type == MT_Outline) {
    Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
  } else {
    Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
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

    Device->SetVertexShaderConstantB(1, (BOOL*)&entry.HasShadowColorMap, 1);
    Device->SetVertexShaderConstantF(11, &entry.ModelOpacity, 1);
    Device->SetVertexShaderConstantF(12, (float*)&entry.Bones,
                                     4 * ModelMaxBonesPerMesh);

    Device->SetPixelShaderConstantB(1, (BOOL*)&entry.HasShadowColorMap, 1);
    Device->SetPixelShaderConstantF(11, &entry.ModelOpacity, 1);
    Device->SetPixelShaderConstantF(12, (float*)&entry.Bones,
                                    4 * ModelMaxBonesPerMesh);
  } else if (StaticModel->Type == ModelType_Background) {
    BgMVPUniformBufferType entry{};
    entry.MVP = ViewProjection * mesh.ModelTransform.Matrix();
    Device->SetVertexShaderConstantF(0, glm::value_ptr(entry.MVP), 4);
  }
}

void Renderable3D::UnloadSync() {
  Animator.CurrentAnimation = 0;
  PrevPoseWeight = 0.0f;
  if (StaticModel) {
    ImpLog(LL_Info, LC_Renderable3D, "Unloading model %d\n", StaticModel->Id);
    if (IsSubmitted) {
      for (int i = 0; i < StaticModel->MeshCount; i++) {
        MeshVertexBuffersDevice[i]->Release();
        MeshIndexBuffersDevice[i]->Release();
      }
    }
    delete StaticModel;
    StaticModel = 0;
  }
  if (MorphedVerticesDevice) {
    MorphedVerticesDevice->Release();
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

  for (int i = 0; i < StaticModel->MeshCount; i++) {
    uint32_t vertexCopySize = 0;

    if (StaticModel->Type == ModelType_Character) {
      if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
        Device->CreateVertexBuffer(
            sizeof(VertexBufferDaSH) * StaticModel->Meshes[i].VertexCount, 0, 0,
            D3DPOOL_MANAGED, &MeshVertexBuffersDevice[i], NULL);

        VOID* pVoid;
        auto res = MeshVertexBuffersDevice[i]->Lock(0, 0, (void**)&pVoid, 0);
        memcpy(pVoid,
               (VertexBufferDaSH*)StaticModel->VertexBuffers +
                   StaticModel->Meshes[i].VertexOffset,
               sizeof(VertexBufferDaSH) * StaticModel->Meshes[i].VertexCount);
        res = MeshVertexBuffersDevice[i]->Unlock();
      } else {
        Device->CreateVertexBuffer(
            sizeof(VertexBuffer) * StaticModel->Meshes[i].VertexCount, 0, 0,
            D3DPOOL_MANAGED, &MeshVertexBuffersDevice[i], NULL);

        VOID* pVoid;
        auto res = MeshVertexBuffersDevice[i]->Lock(0, 0, (void**)&pVoid, 0);
        memcpy(pVoid,
               (VertexBuffer*)StaticModel->VertexBuffers +
                   StaticModel->Meshes[i].VertexOffset,
               sizeof(VertexBuffer) * StaticModel->Meshes[i].VertexCount);
        res = MeshVertexBuffersDevice[i]->Unlock();
      }
    } else if (StaticModel->Type == ModelType_Background) {
      Device->CreateVertexBuffer(
          sizeof(BgVertexBuffer) * StaticModel->Meshes[i].VertexCount, 0, 0,
          D3DPOOL_MANAGED, &MeshVertexBuffersDevice[i], NULL);

      VOID* pVoid;
      auto res = MeshVertexBuffersDevice[i]->Lock(0, 0, (void**)&pVoid, 0);
      memcpy(pVoid,
             (BgVertexBuffer*)StaticModel->VertexBuffers +
                 StaticModel->Meshes[i].VertexOffset,
             sizeof(BgVertexBuffer) * StaticModel->Meshes[i].VertexCount);
      res = MeshVertexBuffersDevice[i]->Unlock();
    }

    Device->CreateIndexBuffer(
        sizeof(uint16_t) * StaticModel->Meshes[i].IndexCount, 0, D3DFMT_INDEX16,
        D3DPOOL_MANAGED, &MeshIndexBuffersDevice[i], NULL);
    VOID* pVoid;
    auto res = MeshIndexBuffersDevice[i]->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, StaticModel->Indices + StaticModel->Meshes[i].IndexOffset,
           sizeof(uint16_t) * StaticModel->Meshes[i].IndexCount);
    res = MeshIndexBuffersDevice[i]->Unlock();
  }

  for (int i = 0; i < StaticModel->TextureCount; i++) {
    TexBuffers[i] = StaticModel->Textures[i].Submit();
    if (TexBuffers[i] == 0) {
      ImpLog(LL_Fatal, LC_Renderable3D,
             "Submitting texture %d for model %d failed\n", i, StaticModel->Id);
    }
  }

  IsSubmitted = true;
}

}  // namespace DirectX9
}  // namespace Impacto