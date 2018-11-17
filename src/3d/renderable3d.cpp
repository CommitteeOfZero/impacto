#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "renderable3d.h"

#include "modelanimator.h"
#include "scene.h"
#include "camera.h"
#include "../shader.h"
#include "../log.h"

namespace Impacto {

enum CommonUniform {
  CU_ViewProjection = 0,
  CU_Model = 1,
  CU_Bones = 2,
  CU_Tint = 3,
  CU_WorldLightPosition = 4,
  CU_WorldEyePosition = 5,
  CU_ModelOpacity = 6,
  CU_Count = 7
};
char const* CommonUniformNames[CU_Count] = {
    "ViewProjection",   "Model",       "Bones", "Tint", "WorldLightPosition",
    "WorldEyePosition", "ModelOpacity"};
static GLint CommonUniformOffsets[CU_Count];

static GLuint TextureDummy = 0;
// character
static GLuint ShaderProgram = 0, ShaderProgramOutline = 0, ShaderProgramEye = 0,
              UBO = 0, UniformDarkMode = 0;
// background
static GLuint ShaderProgramBackground = 0, UniformViewProjectionBackground = 0,
              UniformModelBackground = 0;

static uint8_t* LocalUniformBuffer;
static GLint UniformBlockSize;

static bool IsInit = false;

void Renderable3D::Init() {
  assert(IsInit == false);
  ImpLog(LL_Info, LC_Renderable3D, "Initializing Renderable3D system\n");
  IsInit = true;
  Model::Init();

  ShaderParamMap shaderParams;
  shaderParams["ModelMaxBonesPerMesh"] = ModelMaxBonesPerMesh;

  ShaderProgram = ShaderCompile("Renderable3D_Character", shaderParams);
  ShaderProgramOutline = ShaderCompile("Renderable3D_Outline", shaderParams);
  ShaderProgramEye = ShaderCompile("Renderable3D_Eye", shaderParams);

  ShaderProgramBackground = ShaderCompile("Renderable3D_Background");

  GLuint uniformIndices[CU_Count];
  glGetUniformIndices(ShaderProgram, CU_Count, CommonUniformNames,
                      uniformIndices);
  glGetActiveUniformsiv(ShaderProgram, CU_Count, uniformIndices,
                        GL_UNIFORM_OFFSET, CommonUniformOffsets);

  GLuint blockIndex =
      glGetUniformBlockIndex(ShaderProgram, "Character3DCommon");
  glUniformBlockBinding(ShaderProgram, blockIndex, 0);
  GLuint blockIndexOutline =
      glGetUniformBlockIndex(ShaderProgramOutline, "Character3DCommon");
  glUniformBlockBinding(ShaderProgramOutline, blockIndexOutline, 0);
  GLuint blockIndexEye =
      glGetUniformBlockIndex(ShaderProgramEye, "Character3DCommon");
  glUniformBlockBinding(ShaderProgramEye, blockIndexEye, 0);

  glGetActiveUniformBlockiv(ShaderProgram, blockIndex,
                            GL_UNIFORM_BLOCK_DATA_SIZE, &UniformBlockSize);

  glGenBuffers(1, &UBO);
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferData(GL_UNIFORM_BUFFER, UniformBlockSize, NULL, GL_DYNAMIC_DRAW);
  LocalUniformBuffer = (uint8_t*)malloc(UniformBlockSize);

  glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);

  UniformDarkMode = glGetUniformLocation(ShaderProgram, "DarkMode");

  UniformViewProjectionBackground =
      glGetUniformLocation(ShaderProgramBackground, "ViewProjection");
  UniformModelBackground =
      glGetUniformLocation(ShaderProgramBackground, "Model");

  glUseProgram(ShaderProgram);
  glUniform1i(glGetUniformLocation(ShaderProgram, "ColorMap"), TT_ColorMap);
  glUniform1i(glGetUniformLocation(ShaderProgram, "GradientMaskMap"),
              TT_GradientMaskMap);
  glUniform1i(glGetUniformLocation(ShaderProgram, "SpecularColorMap"),
              TT_SpecularColorMap);

  glUseProgram(ShaderProgramOutline);
  glUniform1i(glGetUniformLocation(ShaderProgramOutline, "ColorMap"),
              TT_ColorMap);

  glUseProgram(ShaderProgramEye);
  glUniform1i(glGetUniformLocation(ShaderProgramEye, "HighlightColorMap"),
              TT_Eye_HighlightColorMap);
  glUniform1i(glGetUniformLocation(ShaderProgramEye, "WhiteColorMap"),
              TT_Eye_WhiteColorMap);
  glUniform1i(glGetUniformLocation(ShaderProgramEye, "IrisSpecularColorMap"),
              TT_Eye_IrisSpecularColorMap);
  glUniform1i(glGetUniformLocation(ShaderProgramEye, "IrisColorMap"),
              TT_Eye_IrisColorMap);

  glUseProgram(ShaderProgramBackground);
  glUniform1i(glGetUniformLocation(ShaderProgramBackground, "ColorMap"),
              TT_ColorMap);

  Texture texDummy;
  texDummy.Load1x1();
  TextureDummy = texDummy.Submit();
}

void Renderable3D::UpdateGpu(Camera* camera) {
  memcpy(LocalUniformBuffer + CommonUniformOffsets[CU_ViewProjection],
         glm::value_ptr(camera->ViewProjection),
         sizeof(camera->ViewProjection));
  memcpy(LocalUniformBuffer + CommonUniformOffsets[CU_Tint],
         glm::value_ptr(Scene3D::Tint), sizeof(Scene3D::Tint));
  memcpy(LocalUniformBuffer + CommonUniformOffsets[CU_WorldLightPosition],
         glm::value_ptr(Scene3D::LightPosition),
         sizeof(Scene3D::LightPosition));
  memcpy(LocalUniformBuffer + CommonUniformOffsets[CU_WorldEyePosition],
         glm::value_ptr(camera->CameraTransform.Position),
         sizeof(camera->CameraTransform.Position));

  glUseProgram(ShaderProgram);
  glUniform1i(UniformDarkMode, Scene3D::DarkMode);

  glUseProgram(ShaderProgramBackground);
  glUniformMatrix4fv(UniformViewProjectionBackground, 1, GL_FALSE,
                     glm::value_ptr(camera->ViewProjection));
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
  SwitchAnimation(1, 0.0f);

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
  CurrentMorphedVertices = (MorphVertexBuffer*)malloc(
      totalMorphedVertices * sizeof(MorphVertexBuffer));
  ReloadDefaultMeshAnimStatus();
}

void Renderable3D::ReloadDefaultMeshAnimStatus() {
  int totalMorphedVertices = 0;
  for (int i = 0; i < StaticModel->MeshCount; i++) {
    MeshAnimStatus[i].Visible = 1.0f;
    if (StaticModel->Meshes[i].MorphTargetCount > 0) {
      for (int j = 0; j < StaticModel->Meshes[i].VertexCount; j++) {
        VertexBuffer* vertexBuffer =
            &((VertexBuffer*)StaticModel
                  ->VertexBuffers)[StaticModel->Meshes[i].VertexOffset + j];
        CurrentMorphedVertices[totalMorphedVertices].Position =
            vertexBuffer->Position;
        CurrentMorphedVertices[totalMorphedVertices].Normal =
            vertexBuffer->Normal;
        totalMorphedVertices++;
      }
      for (int j = 0; j < StaticModel->Meshes[i].MorphTargetCount; j++) {
        MeshAnimStatus[i].MorphInfluences[j] = 0.0f;
      }
    }
  }
}

void Renderable3D::SwitchAnimation(uint32_t animId, float transitionTime) {
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

void Renderable3D::CalculateMorphedVertices() {
  for (int i = 0; i < StaticModel->MeshCount; i++) {
    Mesh* mesh = &StaticModel->Meshes[i];
    if (mesh->MorphTargetCount == 0) continue;

    for (int j = 0; j < StaticModel->Meshes[i].VertexCount; j++) {
      VertexBuffer* vertexBuffer =
          &((VertexBuffer*)StaticModel
                ->VertexBuffers)[StaticModel->Meshes[i].VertexOffset + j];
      glm::vec3 pos = vertexBuffer->Position;
      glm::vec3 normal = vertexBuffer->Normal;

      glm::vec3 basePos = pos;
      glm::vec3 baseNormal = normal;

      for (int k = 0; k < mesh->MorphTargetCount; k++) {
        float influence;
        if (PrevPoseWeight > 0.0f) {
          influence =
              glm::mix(PrevMeshAnimStatus[i].MorphInfluences[k],
                       MeshAnimStatus[i].MorphInfluences[k],
                       glm::smoothstep(0.0f, 1.0f, 1.0f - PrevPoseWeight));
        } else {
          influence = MeshAnimStatus[i].MorphInfluences[k];
        }

        if (influence == 0.0f) continue;

        pos += (StaticModel
                    ->MorphVertexBuffers
                        [StaticModel->MorphTargets[mesh->MorphTargetIds[k]]
                             .VertexOffset +
                         j]
                    .Position -
                basePos) *
               influence;
        normal += (StaticModel
                       ->MorphVertexBuffers
                           [StaticModel->MorphTargets[mesh->MorphTargetIds[k]]
                                .VertexOffset +
                            j]
                       .Normal -
                   baseNormal) *
                  influence;
      }

      CurrentMorphedVertices[MeshAnimStatus[i].MorphedVerticesOffset + j]
          .Position = pos;
      CurrentMorphedVertices[MeshAnimStatus[i].MorphedVerticesOffset + j]
          .Normal = normal;
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
      SwitchAnimation(1, AnimationTransitionTime);
    } else {
      Animator.Update(dt);
    }
  }
  CalculateMorphedVertices();
  Pose();
  if (PrevPoseWeight > 0.0f) {
    PrevPoseWeight -= dt / AnimationTransitionTime;
    if (PrevPoseWeight < 0.0f) PrevPoseWeight = 0.0f;
  }
}

void Renderable3D::Render() {
  if (!IsUsed || !IsVisible) return;

  PrepareUniforms();

  memset(VAOsUpdated, 0, sizeof(VAOsUpdated));

  if (StaticModel->Type == ModelType_Character) {
    // Outline pass

    glUseProgram(ShaderProgramOutline);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glDepthMask(GL_FALSE);

    for (int i = 0; i < StaticModel->MeshCount; i++) {
      if (!MeshAnimStatus[i].Visible || StaticModel->Meshes[i].Flags <= 0)
        continue;

      UseVAO(i);
      SetMeshUniforms(i);

      int const outlinePassTextureTypes[] = {TT_ColorMap};
      SetTextures(i, outlinePassTextureTypes, 1);

      SubmitUniforms();
      DrawSimpleMesh(i);
    }

    // Eye pass

    glUseProgram(ShaderProgramEye);
    glCullFace(GL_BACK);
    glDepthMask(GL_TRUE);

    for (int i = 0; i < StaticModel->MeshCount; i++) {
      if (!MeshAnimStatus[i].Visible ||
          StaticModel->Meshes[i].Maps[TT_Eye_WhiteColorMap] < 0)
        continue;

      UseVAO(i);
      SetMeshUniforms(i);

      int const eyePassTextureTypes[] = {
          TT_Eye_HighlightColorMap, TT_Eye_IrisColorMap,
          TT_Eye_IrisSpecularColorMap, TT_Eye_WhiteColorMap};
      SetTextures(i, eyePassTextureTypes, 4);

      SubmitUniforms();
      DrawCharacterMesh(i);
    }

    // Color pass

    glUseProgram(ShaderProgram);

    for (int i = 0; i < StaticModel->MeshCount; i++) {
      if (!MeshAnimStatus[i].Visible ||
          StaticModel->Meshes[i].Maps[TT_Eye_WhiteColorMap] >= 0)
        continue;

      UseVAO(i);
      SetMeshUniforms(i);

      int const colorPassTextureTypes[] = {TT_ColorMap, TT_GradientMaskMap,
                                           TT_SpecularColorMap};
      SetTextures(i, colorPassTextureTypes, 3);

      SubmitUniforms();
      DrawCharacterMesh(i);
    }
  } else if (StaticModel->Type == ModelType_Background) {
    glUseProgram(ShaderProgramBackground);
    // TODO: *should* we cull backfaces? (in the default pose this covers some
    // sprites)
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);

    for (int i = 0; i < StaticModel->MeshCount; i++) {
      UseVAO(i);
      SetMeshUniforms(i);

      int const backgroundTextureTypes[] = {TT_ColorMap};
      SetTextures(i, backgroundTextureTypes, 1);

      DrawSimpleMesh(i);
    }
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

void Renderable3D::PrepareUniforms() {
  if (StaticModel->Type == ModelType_Character) {
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);

    *(glm::mat4*)(LocalUniformBuffer + CommonUniformOffsets[CU_Model]) =
        ModelTransform.Matrix();
  }
}

void Renderable3D::SubmitUniforms() {
  if (StaticModel->Type == ModelType_Character) {
    glBufferData(GL_UNIFORM_BUFFER, UniformBlockSize, LocalUniformBuffer,
                 GL_DYNAMIC_DRAW);
  }
}

void Renderable3D::UseVAO(int id) {
  glBindVertexArray(VAOs[id]);

  if (VAOsUpdated[id]) return;

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

void Renderable3D::SetMeshUniforms(int id) {
  if (StaticModel->Type == ModelType_Character) {
    if (StaticModel->Meshes[id].UsedBones > 0) {
      for (int j = 0; j < StaticModel->Meshes[id].UsedBones; j++) {
        memcpy(LocalUniformBuffer + CommonUniformOffsets[CU_Bones] +
                   j * sizeof(glm::mat4),
               glm::value_ptr(
                   CurrentPose[StaticModel->Meshes[id].BoneMap[j]].Offset),
               sizeof(glm::mat4));
      }
    } else {
      memcpy(
          LocalUniformBuffer + CommonUniformOffsets[CU_Bones],
          glm::value_ptr(CurrentPose[StaticModel->Meshes[id].MeshBone].Offset),
          sizeof(glm::mat4));
    }

    memcpy(LocalUniformBuffer + CommonUniformOffsets[CU_ModelOpacity],
           &StaticModel->Meshes[id].Opacity,
           sizeof(StaticModel->Meshes[id].Opacity));
  } else if (StaticModel->Type == ModelType_Background) {
    glm::mat4 modelMatrix = StaticModel->Meshes[id].ModelTransform.Matrix();
    glUniformMatrix4fv(UniformModelBackground, 1, GL_FALSE,
                       glm::value_ptr(modelMatrix));
  }
}

void Renderable3D::DrawSimpleMesh(int id) {
  glDrawElements(GL_TRIANGLES, StaticModel->Meshes[id].IndexCount,
                 GL_UNSIGNED_SHORT, 0);
}

void Renderable3D::DrawCharacterMesh(int id) {
  // TODO: how do they actually do this?
  if (StaticModel->Meshes[id].Opacity < 0.9) {
    glDepthMask(GL_FALSE);
  }

  if (StaticModel->Meshes[id].Flags & MeshFlag_DoubleSided) {
    glDisable(GL_CULL_FACE);
  }

  DrawSimpleMesh(id);

  // Reset common state
  if (StaticModel->Meshes[id].Opacity < 0.9) {
    glDepthMask(GL_TRUE);
  }
  if (StaticModel->Meshes[id].Flags & MeshFlag_DoubleSided) {
    glEnable(GL_CULL_FACE);
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
      glDeleteVertexArrays(StaticModel->MeshCount, VAOs);
      glDeleteTextures(StaticModel->TextureCount, TexBuffers);
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

  for (int i = 0; i < StaticModel->MeshCount; i++) {
    glBindVertexArray(VAOs[i]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);

    if (StaticModel->Type == ModelType_Character) {
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
      glVertexAttribIPointer(3, 4, GL_UNSIGNED_BYTE, sizeof(VertexBuffer),
                             (void*)offsetof(VertexBuffer, BoneIndices));
      glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBuffer),
                            (void*)offsetof(VertexBuffer, BoneWeights));
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glEnableVertexAttribArray(2);
      glEnableVertexAttribArray(3);
      glEnableVertexAttribArray(4);
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

  for (int i = 0; i < StaticModel->TextureCount; i++) {
    TexBuffers[i] = StaticModel->Textures[i].Submit();
    if (TexBuffers[i] == 0) {
      ImpLog(LL_Fatal, LC_Renderable3D,
             "Submitting texture %d for model %d failed\n", i, StaticModel->Id);
    }
  }

  IsSubmitted = true;
}

}  // namespace Impacto