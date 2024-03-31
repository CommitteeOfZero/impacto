#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "renderable3d.h"

#include "../../3d/camera.h"
#include "../../3d/modelanimator.h"
#include "../../../log.h"
#include "../../../profile/scene3d.h"

namespace Impacto {
namespace OpenGL {

enum SceneUniform {
  SU_ViewProjection = 0,
  SU_Tint = 1,
  SU_WorldLightPosition = 2,
  SU_WorldEyePosition = 3,
  SU_DarkMode = 4,
  SU_Count = 5
};
enum ModelUniform { MU_Model = 0, MU_Count = 1 };
enum MeshUniform {
  MSU_Bones = 0,
  MSU_ModelOpacity = 1,
  MSU_HasShadowColorMap = 2,
  MSU_Count = 3
};

static char const* SceneUniformNames[SU_Count] = {
    "ViewProjection", "Tint", "WorldLightPosition", "WorldEyePosition",
    "DarkMode"};
static GLint SceneUniformOffsets[SU_Count];
static char const* ModelUniformNames[MU_Count] = {"Model"};
static GLint ModelUniformOffsets[MU_Count];
static char const* MeshUniformNames[MSU_Count] = {"Bones", "ModelOpacity",
                                                  "HasShadowColorMap"};
static GLint MeshUniformOffsets[MSU_Count];

static GLuint TextureDummy = 0;
// character
static GLuint ShaderProgram = 0, ShaderProgramOutline = 0, ShaderProgramEye = 0,
              UBOScene = 0;
// background
static GLuint ShaderProgramBackground = 0, UniformMVPBackground = 0;

static uint8_t* SceneUniformBuffer;
static GLint SceneUniformBlockSize;
static uint8_t* ModelUniformBuffer;
static GLint ModelUniformBlockSize;
static uint8_t* MeshUniformBuffer;
static GLint MeshUniformBlockSize;

static glm::mat4 ViewProjection;

static bool IsInit = false;

static MaterialType CurrentMaterial = MT_None;
static bool CurrentMaterialIsDepthWrite = false;
static bool CurrentMaterialIsBackfaceCull = false;

static GLWindow* Window;

void Renderable3D::Init(GLWindow* window, ShaderCompiler* shaderCompiler) {
  assert(IsInit == false);
  ImpLog(LL_Info, LC_Renderable3D, "Initializing Renderable3D system\n");
  IsInit = true;

  Window = window;

  ShaderParamMap shaderParams;
  shaderParams["ModelMaxBonesPerMesh"] = ModelMaxBonesPerMesh;
  int isDaSH = (int)(Profile::Scene3D::Version == +LKMVersion::DaSH);
  shaderParams["DASH"] = ShaderParameter(isDaSH, true);

  ShaderProgram =
      shaderCompiler->Compile("Renderable3D_Character", shaderParams);
  ShaderProgramOutline =
      shaderCompiler->Compile("Renderable3D_Outline", shaderParams);
  ShaderProgramEye = shaderCompiler->Compile("Renderable3D_Eye", shaderParams);

  ShaderProgramBackground =
      shaderCompiler->Compile("Renderable3D_Background", shaderParams);

  GLuint sceneUniformIndices[SU_Count];
  glGetUniformIndices(ShaderProgram, SU_Count, SceneUniformNames,
                      sceneUniformIndices);
  glGetActiveUniformsiv(ShaderProgram, SU_Count, sceneUniformIndices,
                        GL_UNIFORM_OFFSET, SceneUniformOffsets);
  GLuint modelUniformIndices[MU_Count];
  glGetUniformIndices(ShaderProgram, MU_Count, ModelUniformNames,
                      modelUniformIndices);
  glGetActiveUniformsiv(ShaderProgram, MU_Count, modelUniformIndices,
                        GL_UNIFORM_OFFSET, ModelUniformOffsets);
  GLuint meshUniformIndices[MSU_Count];
  glGetUniformIndices(ShaderProgram, MSU_Count, MeshUniformNames,
                      meshUniformIndices);
  glGetActiveUniformsiv(ShaderProgram, MSU_Count, meshUniformIndices,
                        GL_UNIFORM_OFFSET, MeshUniformOffsets);

  GLuint blockIndex = glGetUniformBlockIndex(ShaderProgram, "Character3DScene");
  glUniformBlockBinding(ShaderProgram, blockIndex, 0);
  glGetActiveUniformBlockiv(ShaderProgram, blockIndex,
                            GL_UNIFORM_BLOCK_DATA_SIZE, &SceneUniformBlockSize);
  blockIndex = glGetUniformBlockIndex(ShaderProgram, "Character3DModel");
  glUniformBlockBinding(ShaderProgram, blockIndex, 1);
  glGetActiveUniformBlockiv(ShaderProgram, blockIndex,
                            GL_UNIFORM_BLOCK_DATA_SIZE, &ModelUniformBlockSize);
  blockIndex = glGetUniformBlockIndex(ShaderProgram, "Character3DMesh");
  glUniformBlockBinding(ShaderProgram, blockIndex, 2);
  glGetActiveUniformBlockiv(ShaderProgram, blockIndex,
                            GL_UNIFORM_BLOCK_DATA_SIZE, &MeshUniformBlockSize);

  blockIndex = glGetUniformBlockIndex(ShaderProgramOutline, "Character3DScene");
  glUniformBlockBinding(ShaderProgramOutline, blockIndex, 0);
  blockIndex = glGetUniformBlockIndex(ShaderProgramOutline, "Character3DModel");
  glUniformBlockBinding(ShaderProgramOutline, blockIndex, 1);
  blockIndex = glGetUniformBlockIndex(ShaderProgramOutline, "Character3DMesh");
  glUniformBlockBinding(ShaderProgramOutline, blockIndex, 2);

  blockIndex = glGetUniformBlockIndex(ShaderProgramEye, "Character3DScene");
  glUniformBlockBinding(ShaderProgramEye, blockIndex, 0);
  blockIndex = glGetUniformBlockIndex(ShaderProgramEye, "Character3DModel");
  glUniformBlockBinding(ShaderProgramEye, blockIndex, 1);
  blockIndex = glGetUniformBlockIndex(ShaderProgramEye, "Character3DMesh");
  glUniformBlockBinding(ShaderProgramEye, blockIndex, 2);

  glGenBuffers(1, &UBOScene);
  // Initialize storage
  glBindBuffer(GL_UNIFORM_BUFFER, UBOScene);
  glBufferData(GL_UNIFORM_BUFFER, SceneUniformBlockSize, NULL, GL_DYNAMIC_DRAW);

  SceneUniformBuffer = (uint8_t*)malloc(SceneUniformBlockSize);
  ModelUniformBuffer = (uint8_t*)malloc(ModelUniformBlockSize);
  MeshUniformBuffer = (uint8_t*)malloc(MeshUniformBlockSize);

  UniformMVPBackground = glGetUniformLocation(ShaderProgramBackground, "MVP");

  glUseProgram(ShaderProgram);
  if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
    glUniform1i(glGetUniformLocation(ShaderProgram, "ColorMap"),
                TT_DaSH_ColorMap);
    glUniform1i(glGetUniformLocation(ShaderProgram, "ShadowColorMap"),
                TT_DaSH_ShadowColorMap);
    glUniform1i(glGetUniformLocation(ShaderProgram, "GradientMaskMap"),
                TT_DaSH_GradientMaskMap);
    glUniform1i(glGetUniformLocation(ShaderProgram, "SpecularColorMap"),
                TT_DaSH_SpecularColorMap);
  } else {
    glUniform1i(glGetUniformLocation(ShaderProgram, "ColorMap"), TT_ColorMap);
    glUniform1i(glGetUniformLocation(ShaderProgram, "GradientMaskMap"),
                TT_GradientMaskMap);
    glUniform1i(glGetUniformLocation(ShaderProgram, "SpecularColorMap"),
                TT_SpecularColorMap);
  }

  glUseProgram(ShaderProgramOutline);
  if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
    glUniform1i(glGetUniformLocation(ShaderProgramOutline, "ColorMap"),
                TT_DaSH_ColorMap);
    glUniform1i(glGetUniformLocation(ShaderProgramOutline, "NoiseMap"),
                TT_DaSH_NoiseMap);
  } else {
    glUniform1i(glGetUniformLocation(ShaderProgramOutline, "ColorMap"),
                TT_ColorMap);
  }

  glUseProgram(ShaderProgramEye);
  if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
    glUniform1i(glGetUniformLocation(ShaderProgramEye, "HighlightColorMap"),
                TT_DaSH_Eye_HighlightColorMap);
    glUniform1i(glGetUniformLocation(ShaderProgramEye, "WhiteColorMap"),
                TT_DaSH_Eye_WhiteColorMap);
    glUniform1i(glGetUniformLocation(ShaderProgramEye, "IrisColorMap"),
                TT_DaSH_Eye_IrisColorMap);
  } else {
    glUniform1i(glGetUniformLocation(ShaderProgramEye, "HighlightColorMap"),
                TT_Eye_HighlightColorMap);
    glUniform1i(glGetUniformLocation(ShaderProgramEye, "WhiteColorMap"),
                TT_Eye_WhiteColorMap);
    glUniform1i(glGetUniformLocation(ShaderProgramEye, "IrisSpecularColorMap"),
                TT_Eye_IrisSpecularColorMap);
    glUniform1i(glGetUniformLocation(ShaderProgramEye, "IrisColorMap"),
                TT_Eye_IrisColorMap);
  }

  glUseProgram(ShaderProgramBackground);
  glUniform1i(glGetUniformLocation(ShaderProgramBackground, "ColorMap"),
              TT_ColorMap);

  Texture texDummy;
  texDummy.Load1x1();
  TextureDummy = texDummy.Submit();
}

void Renderable3D::BeginFrame(IScene3D* scene, Camera* camera) {
  CurrentMaterial = MT_None;

  glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBOScene);
  memcpy(SceneUniformBuffer + SceneUniformOffsets[SU_ViewProjection],
         glm::value_ptr(camera->ViewProjection),
         sizeof(camera->ViewProjection));
  memcpy(SceneUniformBuffer + SceneUniformOffsets[SU_Tint],
         glm::value_ptr(scene->Tint), sizeof(scene->Tint));
  memcpy(SceneUniformBuffer + SceneUniformOffsets[SU_WorldLightPosition],
         glm::value_ptr(scene->LightPosition), sizeof(scene->LightPosition));
  memcpy(SceneUniformBuffer + SceneUniformOffsets[SU_WorldEyePosition],
         glm::value_ptr(camera->CameraTransform.Position),
         sizeof(camera->CameraTransform.Position));
  *(uint32_t*)(SceneUniformBuffer + SceneUniformOffsets[SU_DarkMode]) =
      scene->DarkMode;
  glBufferSubData(GL_UNIFORM_BUFFER, 0, SceneUniformBlockSize,
                  SceneUniformBuffer);

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
  CurrentMorphedVertices = (MorphVertexBuffer*)malloc(
      totalMorphedVertices * sizeof(MorphVertexBuffer));
  ReloadDefaultMeshAnimStatus();
}

void Renderable3D::ReloadDefaultMeshAnimStatus() {
  MorphVertexBuffer* currentMorphedVertex = CurrentMorphedVertices;

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

  MorphVertexBuffer* currentMorphedVertex =
      CurrentMorphedVertices + animStatus->MorphedVerticesOffset;

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
      currentMorphedVertex->Position = currentVertexDaSH->Position;
      currentMorphedVertex->Normal = currentVertexDaSH->Normal;
      currentVertexDaSH++;
    } else {
      currentMorphedVertex->Position = currentVertexRNE->Position;
      currentMorphedVertex->Normal = currentVertexRNE->Normal;
      currentVertexRNE++;
    }
    currentMorphedVertex++;
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

    currentMorphedVertex =
        CurrentMorphedVertices + animStatus->MorphedVerticesOffset;

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
        currentMorphedVertex->Position +=
            (currentMorphTargetVbo->Position - currentVertexDaSH->Position) *
            influence;
        currentMorphedVertex->Normal +=
            (currentMorphTargetVbo->Normal - currentVertexDaSH->Normal) *
            influence;
        currentVertexDaSH++;
      } else {
        currentMorphedVertex->Position +=
            (currentMorphTargetVbo->Position - currentVertexRNE->Position) *
            influence;
        currentMorphedVertex->Normal +=
            (currentMorphTargetVbo->Normal - currentVertexRNE->Normal) *
            influence;
        currentVertexRNE++;
      }
      currentMorphedVertex++;
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
      int const eyeTextureTypes[] = {
          TT_Eye_HighlightColorMap, TT_Eye_IrisColorMap,
          TT_Eye_IrisSpecularColorMap, TT_Eye_WhiteColorMap};
      SetTextures(id, eyeTextureTypes, 4);
      break;
    }
    case MT_DaSH_Generic: {
      int const dashGenericTextureTypes[] = {
          TT_DaSH_ColorMap, TT_DaSH_ShadowColorMap, TT_DaSH_GradientMaskMap,
          TT_DaSH_SpecularColorMap};
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
      int const dashEyeTextureTypes[] = {TT_DaSH_Eye_HighlightColorMap,
                                         TT_DaSH_Eye_IrisColorMap,
                                         TT_DaSH_Eye_WhiteColorMap};
      SetTextures(id, dashEyeTextureTypes, 3);
      break;
    }
    default:
      ImpLog(LL_Warning, LC_Renderable3D, "Unknown texture type!\n");
      break;
  }

  if (mesh.Opacity < 0.9 && CurrentMaterialIsDepthWrite) {
    glDepthMask(GL_FALSE);
  }
  // if (mesh.Flags & MeshFlag_DoubleSided && CurrentMaterialIsBackfaceCull) {
  //  glDisable(GL_CULL_FACE);
  //}

  glDrawElements(GL_TRIANGLES, mesh.IndexCount, GL_UNSIGNED_SHORT, 0);

  if (mesh.Opacity < 0.9 && CurrentMaterialIsDepthWrite) {
    glDepthMask(GL_TRUE);
  }
  // if (mesh.Flags & MeshFlag_DoubleSided && CurrentMaterialIsBackfaceCull) {
  //  glEnable(GL_CULL_FACE);
  //}

  if (pass != RP_Outline) DrawMesh(id, RP_Outline);
}

void Renderable3D::Render() {
  if (!IsUsed || !IsVisible) return;

  LoadModelUniforms();

  memset(VAOsUpdated, 0, sizeof(VAOsUpdated));
  memset(UniformsUpdated, 0, sizeof(UniformsUpdated));

  for (uint32_t i = RP_First; i < RP_Count; i++) {
    for (uint32_t j = 0; j < StaticModel->MeshCount; j++) {
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
    glActiveTexture(GL_TEXTURE0 + t);
    if (StaticModel->Meshes[id].Maps[t] >= 0) {
      glBindTexture(GL_TEXTURE_2D, TexBuffers[StaticModel->Meshes[id].Maps[t]]);
    } else {
      glBindTexture(GL_TEXTURE_2D, TextureDummy);
    }
  }
}

void Renderable3D::LoadModelUniforms() {
  if (StaticModel->Type == ModelType_Character) {
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, UBOModel);
    *(glm::mat4*)(ModelUniformBuffer + ModelUniformOffsets[MU_Model]) =
        ModelTransform.Matrix();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, ModelUniformBlockSize,
                    ModelUniformBuffer);
  }
}

void Renderable3D::UseMaterial(MaterialType type) {
  if (CurrentMaterial == type) return;

  switch (type) {
    case MT_Outline: {
      glUseProgram(ShaderProgramOutline);
      break;
    }
    case MT_Background: {
      glUseProgram(ShaderProgramBackground);
      break;
    }
    case MT_Generic:
    case MT_DaSH_Generic:
    case MT_DaSH_Face:
    case MT_DaSH_Skin: {
      glUseProgram(ShaderProgram);
      break;
    }
    case MT_Eye:
    case MT_DaSH_Eye: {
      glUseProgram(ShaderProgramEye);
      break;
    }
    default:
      ImpLog(LL_Warning, LC_Renderable3D, "Unknown texture type!\n");
      break;
  }

  glEnable(GL_CULL_FACE);
  if (type == MT_Outline) {
    glCullFace(GL_FRONT);
  } else {
    glCullFace(GL_BACK);
  }

  // if (type == MT_Outline || type == MT_Generic || type == MT_Eye) {
  //  glEnable(GL_CULL_FACE);
  //  GLenum side = type == MT_Outline ? GL_FRONT : GL_BACK;
  //  glCullFace(side);
  //  CurrentMaterialIsBackfaceCull = side == GL_BACK;
  //} else {
  //  glDisable(GL_CULL_FACE);
  //  CurrentMaterialIsBackfaceCull = false;
  //}

  if (type == MT_Outline) {
    CurrentMaterialIsDepthWrite = false;
    glDepthMask(GL_FALSE);
  } else {
    CurrentMaterialIsDepthWrite = true;
    glDepthMask(GL_TRUE);
  }

  CurrentMaterial = type;
}

void Renderable3D::UseMesh(int id) {
  glBindVertexArray(VAOs[id]);

  LoadMeshUniforms(id);

  if (!VAOsUpdated[id]) {
    if (StaticModel->Meshes[id].MorphTargetCount > 0) {
      glBindBuffer(GL_ARRAY_BUFFER, MorphVBOs[id]);
      glBufferData(
          GL_ARRAY_BUFFER,
          sizeof(MorphVertexBuffer) * StaticModel->Meshes[id].VertexCount,
          CurrentMorphedVertices + MeshAnimStatus[id].MorphedVerticesOffset,
          GL_DYNAMIC_DRAW);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MorphVertexBuffer),
                            (void*)offsetof(MorphVertexBuffer, Position));
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MorphVertexBuffer),
                            (void*)offsetof(MorphVertexBuffer, Normal));
    }

    VAOsUpdated[id] = true;
  }
}

void Renderable3D::LoadMeshUniforms(int id) {
  Mesh& mesh = StaticModel->Meshes[id];

  if (StaticModel->Type == ModelType_Character) {
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, UBOs[id]);

    if (!UniformsUpdated[id]) {
      if (mesh.UsedBones > 0) {
        glm::mat4* outBone =
            (glm::mat4*)(MeshUniformBuffer + MeshUniformOffsets[MSU_Bones]);
        for (uint32_t j = 0; j < mesh.UsedBones; j++) {
          memcpy(outBone, glm::value_ptr(CurrentPose[mesh.BoneMap[j]].Offset),
                 sizeof(glm::mat4));
          outBone++;
        }
      } else {
        memcpy(MeshUniformBuffer + MeshUniformOffsets[MSU_Bones],
               glm::value_ptr(CurrentPose[mesh.MeshBone].Offset),
               sizeof(glm::mat4));
      }

      memcpy(MeshUniformBuffer + MeshUniformOffsets[MSU_ModelOpacity],
             &mesh.Opacity, sizeof(mesh.Opacity));

      *(uint32_t*)(MeshUniformBuffer +
                   MeshUniformOffsets[MSU_HasShadowColorMap]) =
          mesh.Material == MT_DaSH_Generic && mesh.HasShadowColorMap;

      glBufferSubData(GL_UNIFORM_BUFFER, 0, MeshUniformBlockSize,
                      MeshUniformBuffer);

      UniformsUpdated[id] = true;
    }
  } else if (StaticModel->Type == ModelType_Background) {
    glm::mat4 mvp = ViewProjection * mesh.ModelTransform.Matrix();
    glUniformMatrix4fv(UniformMVPBackground, 1, GL_FALSE, glm::value_ptr(mvp));
  }
}

void Renderable3D::UnloadSync() {
  Animator.CurrentAnimation = 0;
  PrevPoseWeight = 0.0f;
  if (StaticModel) {
    ImpLog(LL_Info, LC_Renderable3D, "Unloading model %d\n", StaticModel->Id);
    if (IsSubmitted) {
      glDeleteBuffers(StaticModel->MeshCount, IBOs);
      glDeleteBuffers(StaticModel->MeshCount, VBOs);
      glDeleteBuffers(StaticModel->MeshCount, MorphVBOs);
      glDeleteBuffers(StaticModel->MeshCount, UBOs);
      glDeleteVertexArrays(StaticModel->MeshCount, VAOs);
      glDeleteTextures(StaticModel->TextureCount, TexBuffers);
      glDeleteBuffers(1, &UBOModel);
    }
    delete StaticModel;
    StaticModel = 0;
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

  glGenVertexArrays(StaticModel->MeshCount, VAOs);
  glGenBuffers(StaticModel->MeshCount, VBOs);
  glGenBuffers(StaticModel->MeshCount, MorphVBOs);
  glGenBuffers(StaticModel->MeshCount, IBOs);
  glGenBuffers(StaticModel->MeshCount, UBOs);
  glGenBuffers(1, &UBOModel);

  glBindBuffer(GL_UNIFORM_BUFFER, UBOModel);
  glBufferData(GL_UNIFORM_BUFFER, ModelUniformBlockSize, NULL, GL_DYNAMIC_DRAW);

  for (uint32_t i = 0; i < StaticModel->MeshCount; i++) {
    glBindBuffer(GL_UNIFORM_BUFFER, UBOs[i]);
    glBufferData(GL_UNIFORM_BUFFER, MeshUniformBlockSize, NULL,
                 GL_DYNAMIC_DRAW);

    glBindVertexArray(VAOs[i]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);

    if (StaticModel->Type == ModelType_Character) {
      if (Profile::Scene3D::Version == +LKMVersion::DaSH) {
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(VertexBufferDaSH) * StaticModel->Meshes[i].VertexCount,
            (VertexBufferDaSH*)StaticModel->VertexBuffers +
                StaticModel->Meshes[i].VertexOffset,
            GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                              sizeof(VertexBufferDaSH),
                              (void*)offsetof(VertexBufferDaSH, Position));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                              sizeof(VertexBufferDaSH),
                              (void*)offsetof(VertexBufferDaSH, Normal));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                              sizeof(VertexBufferDaSH),
                              (void*)offsetof(VertexBufferDaSH, UV));
        // WebGL doesn't like unsigned byte here, figures...
        glVertexAttribIPointer(3, 4, GL_BYTE, sizeof(VertexBufferDaSH),
                               (void*)offsetof(VertexBufferDaSH, BoneIndices));
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE,
                              sizeof(VertexBufferDaSH),
                              (void*)offsetof(VertexBufferDaSH, BoneWeights));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
      } else {
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(VertexBuffer) * StaticModel->Meshes[i].VertexCount,
                     (VertexBuffer*)StaticModel->VertexBuffers +
                         StaticModel->Meshes[i].VertexOffset,
                     GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexBuffer),
                              (void*)offsetof(VertexBuffer, Position));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexBuffer),
                              (void*)offsetof(VertexBuffer, Normal));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexBuffer),
                              (void*)offsetof(VertexBuffer, UV));
        // WebGL doesn't like unsigned byte here, figures...
        glVertexAttribIPointer(3, 4, GL_BYTE, sizeof(VertexBuffer),
                               (void*)offsetof(VertexBuffer, BoneIndices));
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBuffer),
                              (void*)offsetof(VertexBuffer, BoneWeights));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
      }
    } else if (StaticModel->Type == ModelType_Background) {
      glBufferData(GL_ARRAY_BUFFER,
                   sizeof(BgVertexBuffer) * StaticModel->Meshes[i].VertexCount,
                   (BgVertexBuffer*)StaticModel->VertexBuffers +
                       StaticModel->Meshes[i].VertexOffset,
                   GL_STATIC_DRAW);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BgVertexBuffer),
                            (void*)offsetof(BgVertexBuffer, Position));
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(BgVertexBuffer),
                            (void*)offsetof(BgVertexBuffer, UV));
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOs[i]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(uint16_t) * StaticModel->Meshes[i].IndexCount,
                 StaticModel->Indices + StaticModel->Meshes[i].IndexOffset,
                 GL_STATIC_DRAW);
  }

  for (uint32_t i = 0; i < StaticModel->TextureCount; i++) {
    TexBuffers[i] = StaticModel->Textures[i].Submit();
    if (TexBuffers[i] == 0) {
      ImpLog(LL_Fatal, LC_Renderable3D,
             "Submitting texture %d for model %d failed\n", i, StaticModel->Id);
    }
  }

  IsSubmitted = true;
}

}  // namespace OpenGL
}  // namespace Impacto