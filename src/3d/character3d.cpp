#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "character3d.h"

#include "modelanimator.h"
#include "scene.h"
#include "camera.h"
#include "../shader.h"
#include "../log.h"

namespace Impacto {

static GLuint ShaderProgram = 0, UniformViewProjection = 0, UniformModel = 0,
              UniformModelOpacity = 0, UniformWorldLightPosition = 0,
              UniformTint = 0, UniformWorldEyePosition = 0, UniformColorMap = 0,
              UniformGradientMaskMap = 0, UniformSpecularColorMap = 0;
static GLuint UniformBones[ModelMaxBonesPerMesh] = {0};
static bool IsInit = false;

void Character3DInit() {
  assert(IsInit == false);
  ImpLog(LL_Info, LC_Character3D, "Initializing Character3D system\n");
  IsInit = true;
  Model::Init();
  ShaderProgram = ShaderCompile("Character3D");
  UniformViewProjection = glGetUniformLocation(ShaderProgram, "ViewProjection");
  UniformModel = glGetUniformLocation(ShaderProgram, "Model");
  UniformModelOpacity = glGetUniformLocation(ShaderProgram, "ModelOpacity");
  UniformWorldLightPosition =
      glGetUniformLocation(ShaderProgram, "WorldLightPosition");
  UniformTint = glGetUniformLocation(ShaderProgram, "Tint");
  UniformWorldEyePosition =
      glGetUniformLocation(ShaderProgram, "WorldEyePosition");
  for (int i = 0; i < ModelMaxBonesPerMesh; i++) {
    char name[16];
    int sz = snprintf(name, 16, "Bones[%d]", i);
    assert(sz < sizeof(name));
    UniformBones[i] = glGetUniformLocation(ShaderProgram, name);
  }

  UniformColorMap = glGetUniformLocation(ShaderProgram, "ColorMap");
  UniformGradientMaskMap =
      glGetUniformLocation(ShaderProgram, "GradientMaskMap");
  UniformSpecularColorMap =
      glGetUniformLocation(ShaderProgram, "SpecularColorMap");
}

bool Character3D::Load(uint32_t modelId) {
  assert(IsUsed == false);

  ImpLog(LL_Info, LC_Character3D, "Creating character (model ID %d)\n",
         modelId);

  StaticModel = Model::Load(modelId);
  ModelTransform = Transform();

  if (!StaticModel) {
    ImpLog(LL_Error, LC_Character3D,
           "Model loading failed for character with model ID %d\n");
    return false;
  }

  InitMeshAnimStatus();
  ReloadDefaultBoneTransforms();

  Animator.Character = this;
  Animator.Start(1);

  IsUsed = true;

  return true;
}

void Character3D::MakePlane() {
  assert(IsUsed == false);

  StaticModel = Model::MakePlane();
  ModelTransform = Transform();

  Animator.Character = this;

  InitMeshAnimStatus();
  ReloadDefaultBoneTransforms();

  IsUsed = true;
}

void Character3D::InitMeshAnimStatus() {
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

void Character3D::ReloadDefaultMeshAnimStatus() {
  int totalMorphedVertices = 0;
  for (int i = 0; i < StaticModel->MeshCount; i++) {
    MeshAnimStatus[i].Visible = 1.0f;
    MeshAnimStatus[i].UsedMorphTargetCount = 0;
    if (StaticModel->Meshes[i].MorphTargetCount > 0) {
      for (int j = 0; j < StaticModel->Meshes[i].VertexCount; j++) {
        CurrentMorphedVertices[totalMorphedVertices].Position =
            StaticModel->VertexBuffers[StaticModel->Meshes[i].VertexOffset + j]
                .Position;
        CurrentMorphedVertices[totalMorphedVertices].Normal =
            StaticModel->VertexBuffers[StaticModel->Meshes[i].VertexOffset + j]
                .Normal;
        totalMorphedVertices++;
      }
    }
  }
}

void Character3D::ReloadDefaultBoneTransforms() {
  for (int i = 0; i < StaticModel->BoneCount; i++) {
    CurrentPose[i].LocalTransform = StaticModel->Bones[i].BaseTransform;
  }
}

void Character3D::CalculateMorphedVertices() {
  for (int i = 0; i < StaticModel->MeshCount; i++) {
    if (MeshAnimStatus[i].UsedMorphTargetCount == 0) continue;

    for (int j = 0; j < StaticModel->Meshes[i].VertexCount; j++) {
      glm::vec3 pos =
          StaticModel->VertexBuffers[StaticModel->Meshes[i].VertexOffset + j]
              .Position;
      glm::vec3 normal =
          StaticModel->VertexBuffers[StaticModel->Meshes[i].VertexOffset + j]
              .Normal;

      glm::vec3 basePos = pos;
      glm::vec3 baseNormal = normal;

      CurrentMorphedVertices[MeshAnimStatus[i].MorphedVerticesOffset + j]
          .Position = pos;
      CurrentMorphedVertices[MeshAnimStatus[i].MorphedVerticesOffset + j]
          .Normal = normal;

      for (int k = 0; k < MeshAnimStatus->UsedMorphTargetCount; k++) {
        pos +=
            (StaticModel
                 ->MorphVertexBuffers
                     [StaticModel
                          ->MorphTargets[MeshAnimStatus->UsedMorphTargetIds[k]]
                          .VertexOffset]
                 .Position -
             basePos) *
            MeshAnimStatus->MorphInfluences[k];
        normal +=
            (StaticModel
                 ->MorphVertexBuffers
                     [StaticModel
                          ->MorphTargets[MeshAnimStatus->UsedMorphTargetIds[k]]
                          .VertexOffset]
                 .Normal -
             baseNormal) *
            MeshAnimStatus->MorphInfluences[k];
      }

      CurrentMorphedVertices[MeshAnimStatus[i].MorphedVerticesOffset + j]
          .Position = pos;
      CurrentMorphedVertices[MeshAnimStatus[i].MorphedVerticesOffset + j]
          .Normal = normal;
    }
  }
}

void Character3D::Pose() {
  for (int i = 0; i < StaticModel->RootBoneCount; i++) {
    PoseBone(StaticModel->RootBones[i]);
  }
}

void Character3D::PoseBone(int16_t id) {
  StaticBone* bone = &StaticModel->Bones[id];
  PosedBone* transformed = &CurrentPose[id];

  glm::mat4 parentWorld;
  if (bone->Parent < 0) {
    parentWorld = glm::mat4(1.0);
  } else {
    parentWorld = CurrentPose[bone->Parent].World;
  }

  glm::mat4 local = transformed->LocalTransform.Matrix();

  transformed->World = parentWorld * local;

  for (int i = 0; i < bone->ChildrenCount; i++) {
    PoseBone(bone->Children[i]);
  }

  transformed->Offset = transformed->World * bone->BindInverse;
}

void Character3D::Update(float dt) {
  if (!IsUsed) return;
  if (Animator.CurrentAnimation) Animator.Update(dt);
  CalculateMorphedVertices();
  Pose();
}

void Character3D::Render() {
  if (!IsUsed) return;

  glUseProgram(ShaderProgram);

  glUniformMatrix4fv(UniformViewProjection, 1, GL_FALSE,
                     glm::value_ptr(g_Camera.ViewProjection));
  glm::mat4 ModelMatrix = ModelTransform.Matrix();
  glUniformMatrix4fv(UniformModel, 1, GL_FALSE, glm::value_ptr(ModelMatrix));

  glUniform3fv(UniformWorldEyePosition, 1, glm::value_ptr(g_Camera.Position));

  glUniform3fv(UniformWorldLightPosition, 1,
               glm::value_ptr(g_Scene.LightPosition));

  glUniform4fv(UniformTint, 1, glm::value_ptr(g_Scene.Tint));

  glUniform1i(UniformColorMap, 0);
  glUniform1i(UniformGradientMaskMap, 1);
  glUniform1i(UniformSpecularColorMap, 2);

  for (int i = 0; i < StaticModel->MeshCount; i++) {
    if (!MeshAnimStatus[i].Visible) continue;

    glBindVertexArray(VAOs[i]);

    if (StaticModel->Meshes[i].UsedBones > 0) {
      for (int j = 0; j < StaticModel->Meshes[i].UsedBones; j++) {
        glUniformMatrix4fv(
            UniformBones[j], 1, GL_FALSE,
            glm::value_ptr(
                CurrentPose[StaticModel->Meshes[i].BoneMap[j]].Offset));
      }
    } else {
      glUniformMatrix4fv(
          UniformBones[0], 1, GL_FALSE,
          glm::value_ptr(CurrentPose[StaticModel->Meshes[i].MeshBone].Offset));
    }

    glActiveTexture(GL_TEXTURE0);
    if (StaticModel->Meshes[i].ColorMap >= 0) {
      glBindTexture(GL_TEXTURE_2D, TexBuffers[StaticModel->Meshes[i].ColorMap]);
    }
    glActiveTexture(GL_TEXTURE1);
    if (StaticModel->Meshes[i].GradientMaskMap >= 0) {
      glBindTexture(GL_TEXTURE_2D,
                    TexBuffers[StaticModel->Meshes[i].GradientMaskMap]);
    }
    glActiveTexture(GL_TEXTURE2);
    if (StaticModel->Meshes[i].SpecularColorMap >= 0) {
      glBindTexture(GL_TEXTURE_2D,
                    TexBuffers[StaticModel->Meshes[i].SpecularColorMap]);
    }

    glUniform1f(UniformModelOpacity, StaticModel->Meshes[i].Opacity);

    // TODO: how do they actually do this?
    if (StaticModel->Meshes[i].Opacity < 0.9) {
      glDepthMask(GL_FALSE);
    }

    glDrawElements(GL_TRIANGLES, StaticModel->Meshes[i].IndexCount,
                   GL_UNSIGNED_SHORT, 0);

    glDepthMask(GL_TRUE);
  }
}

void Character3D::Unload() {
  Animator.CurrentAnimation = 0;
  if (StaticModel) {
    ImpLog(LL_Info, LC_Character3D, "Unloading model %d\n", StaticModel->Id);
    if (IsSubmitted) {
      glDeleteBuffers(StaticModel->MeshCount, IBOs);
      glDeleteBuffers(StaticModel->MeshCount, VBOs);
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
}

void Character3D::Submit() {
  assert(IsSubmitted == false);

  ImpLog(LL_Info, LC_Character3D, "Submitting data to GPU for model ID %d\n",
         StaticModel->Id);

  glGenVertexArrays(StaticModel->MeshCount, VAOs);
  glGenBuffers(StaticModel->MeshCount, VBOs);
  glGenBuffers(StaticModel->MeshCount, IBOs);

  for (int i = 0; i < StaticModel->MeshCount; i++) {
    glBindVertexArray(VAOs[i]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(VertexBuffer) * StaticModel->Meshes[i].VertexCount,
        StaticModel->VertexBuffers + StaticModel->Meshes[i].VertexOffset,
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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOs[i]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(uint16_t) * StaticModel->Meshes[i].IndexCount,
                 StaticModel->Indices + StaticModel->Meshes[i].IndexOffset,
                 GL_STATIC_DRAW);
  }

  for (int i = 0; i < StaticModel->TextureCount; i++) {
    TexBuffers[i] = StaticModel->Textures[i].Submit();
    if (TexBuffers[i] == 0) {
      ImpLog(LL_Fatal, LC_Character3D,
             "Submitting texture %d for model %d failed\n", i, StaticModel->Id);
    }
  }

  IsSubmitted = true;
}

}  // namespace Impacto