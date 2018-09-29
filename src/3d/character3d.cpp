#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "character3d.h"

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

static GLuint ShaderProgram = 0, ShaderProgramOutline = 0, ShaderProgramEye = 0,
              UBO = 0, UniformDarkMode = 0;
static bool IsInit = false;

void Character3DInit() {
  assert(IsInit == false);
  ImpLog(LL_Info, LC_Object3D, "Initializing Character3D system\n");
  IsInit = true;
  Model::Init();

  ShaderProgram = ShaderCompile("Character3D");
  ShaderProgramOutline = ShaderCompile("Character3D_Outline");
  ShaderProgramEye = ShaderCompile("Character3D_Eye");

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

  GLint uniformBlockSize;
  glGetActiveUniformBlockiv(ShaderProgram, blockIndex,
                            GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlockSize);

  glGenBuffers(1, &UBO);
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferData(GL_UNIFORM_BUFFER, uniformBlockSize, NULL, GL_DYNAMIC_DRAW);

  glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);

  UniformDarkMode = glGetUniformLocation(ShaderProgram, "DarkMode");

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
}

void Character3DUpdateGpu(Scene* scene, Camera* camera) {
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferSubData(GL_UNIFORM_BUFFER, CommonUniformOffsets[CU_ViewProjection],
                  sizeof(glm::mat4), glm::value_ptr(camera->ViewProjection));
  glBufferSubData(GL_UNIFORM_BUFFER, CommonUniformOffsets[CU_Tint],
                  sizeof(glm::vec4), glm::value_ptr(scene->Tint));
  glBufferSubData(GL_UNIFORM_BUFFER,
                  CommonUniformOffsets[CU_WorldLightPosition],
                  sizeof(glm::vec3), glm::value_ptr(scene->LightPosition));
  glBufferSubData(GL_UNIFORM_BUFFER, CommonUniformOffsets[CU_WorldEyePosition],
                  sizeof(glm::vec3), glm::value_ptr(camera->Position));

  glUseProgram(ShaderProgram);
  glUniform1i(UniformDarkMode, scene->DarkMode);
}

bool Character3D::Load(uint32_t modelId) {
  assert(IsUsed == false);

  ImpLog(LL_Info, LC_Object3D, "Creating character (model ID %d)\n", modelId);

  StaticModel = Model::Load(modelId);
  ModelTransform = Transform();

  if (!StaticModel) {
    ImpLog(LL_Error, LC_Object3D,
           "Model loading failed for character with model ID %d\n");
    return false;
  }

  assert(StaticModel->Type == ModelType_Character);

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

void Character3D::ReloadDefaultBoneTransforms() {
  for (int i = 0; i < StaticModel->BoneCount; i++) {
    CurrentPose[i].LocalTransform = StaticModel->Bones[i].BaseTransform;
  }
}

void Character3D::CalculateMorphedVertices() {
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
        pos += (StaticModel
                    ->MorphVertexBuffers
                        [StaticModel->MorphTargets[mesh->MorphTargetIds[k]]
                             .VertexOffset +
                         j]
                    .Position -
                basePos) *
               MeshAnimStatus[i].MorphInfluences[k];
        normal += (StaticModel
                       ->MorphVertexBuffers
                           [StaticModel->MorphTargets[mesh->MorphTargetIds[k]]
                                .VertexOffset +
                            j]
                       .Normal -
                   baseNormal) *
                  MeshAnimStatus[i].MorphInfluences[k];
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
  Animator.Update(dt);
  CalculateMorphedVertices();
  Pose();
}

void Character3D::Render() {
  if (!IsUsed) return;

  glBindBuffer(GL_UNIFORM_BUFFER, UBO);

  glm::mat4 ModelMatrix = ModelTransform.Matrix();
  glBufferSubData(GL_UNIFORM_BUFFER, CommonUniformOffsets[CU_Model],
                  sizeof(glm::mat4), glm::value_ptr(ModelMatrix));

  for (int i = 0; i < StaticModel->MeshCount; i++) {
    if (!MeshAnimStatus[i].Visible) continue;

    glBindVertexArray(VAOs[i]);

    if (StaticModel->Meshes[i].MorphTargetCount > 0) {
      glBindBuffer(GL_ARRAY_BUFFER, MorphVBOs[i]);
      glBufferData(
          GL_ARRAY_BUFFER,
          sizeof(MorphVertexBuffer) * StaticModel->Meshes[i].VertexCount,
          CurrentMorphedVertices + MeshAnimStatus[i].MorphedVerticesOffset,
          GL_DYNAMIC_DRAW);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MorphVertexBuffer),
                            (void*)offsetof(MorphVertexBuffer, Position));
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MorphVertexBuffer),
                            (void*)offsetof(MorphVertexBuffer, Normal));
    }

    if (StaticModel->Meshes[i].UsedBones > 0) {
      for (int j = 0; j < StaticModel->Meshes[i].UsedBones; j++) {
        glBufferSubData(
            GL_UNIFORM_BUFFER,
            CommonUniformOffsets[CU_Bones] + sizeof(glm::mat4) * j,
            sizeof(glm::mat4),
            glm::value_ptr(
                CurrentPose[StaticModel->Meshes[i].BoneMap[j]].Offset));
      }
    } else {
      glBufferSubData(
          GL_UNIFORM_BUFFER, CommonUniformOffsets[CU_Bones], sizeof(glm::mat4),
          glm::value_ptr(CurrentPose[StaticModel->Meshes[i].MeshBone].Offset));
    }

    for (int j = 0; j < TT_Count; j++) {
      glActiveTexture(GL_TEXTURE0 + j);
      if (StaticModel->Meshes[i].Maps[j] >= 0) {
        glBindTexture(GL_TEXTURE_2D,
                      TexBuffers[StaticModel->Meshes[i].Maps[j]]);
      } else {
        glBindTexture(GL_TEXTURE_2D, 0);
      }
    }

    glBufferSubData(GL_UNIFORM_BUFFER, CommonUniformOffsets[CU_ModelOpacity],
                    sizeof(float), &StaticModel->Meshes[i].Opacity);

    if (StaticModel->Meshes[i].Flags > 0) {
      DrawMesh(i, true);
    }

    DrawMesh(i, false);
  }
}

void Character3D::DrawMesh(int id, bool outline) {
  if (outline) {
    glUseProgram(ShaderProgramOutline);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glDepthMask(GL_FALSE);
  } else {
    if (StaticModel->Meshes[id].Maps[TT_Eye_WhiteColorMap] >= 0) {
      // Yes, really, there's no other indication that the mesh is an eye
      // TODO: Iris/highlights should be animated
      glUseProgram(ShaderProgramEye);
    } else {
      glUseProgram(ShaderProgram);
    }

    // TODO: how do they actually do this?
    if (StaticModel->Meshes[id].Opacity < 0.9) {
      glDepthMask(GL_FALSE);
    } else {
      glDepthMask(GL_TRUE);
    }

    if (StaticModel->Meshes[id].Flags & MeshFlag_DoubleSided) {
      glDisable(GL_CULL_FACE);
    } else {
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
    }
  }

  glDrawElements(GL_TRIANGLES, StaticModel->Meshes[id].IndexCount,
                 GL_UNSIGNED_SHORT, 0);
}

void Character3D::Unload() {
  Animator.CurrentAnimation = 0;
  if (StaticModel) {
    ImpLog(LL_Info, LC_Object3D, "Unloading model %d\n", StaticModel->Id);
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
}

void Character3D::Submit() {
  assert(IsSubmitted == false);

  ImpLog(LL_Info, LC_Object3D, "Submitting data to GPU for model ID %d\n",
         StaticModel->Id);

  glGenVertexArrays(StaticModel->MeshCount, VAOs);
  glGenBuffers(StaticModel->MeshCount, VBOs);
  glGenBuffers(StaticModel->MeshCount, MorphVBOs);
  glGenBuffers(StaticModel->MeshCount, IBOs);

  for (int i = 0; i < StaticModel->MeshCount; i++) {
    glBindVertexArray(VAOs[i]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOs[i]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(uint16_t) * StaticModel->Meshes[i].IndexCount,
                 StaticModel->Indices + StaticModel->Meshes[i].IndexOffset,
                 GL_STATIC_DRAW);
  }

  for (int i = 0; i < StaticModel->TextureCount; i++) {
    TexBuffers[i] = StaticModel->Textures[i].Submit();
    if (TexBuffers[i] == 0) {
      ImpLog(LL_Fatal, LC_Object3D,
             "Submitting texture %d for model %d failed\n", i, StaticModel->Id);
    }
  }

  IsSubmitted = true;
}

}  // namespace Impacto