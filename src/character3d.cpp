#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "character3d.h"

#include "camera.h"
#include "shader.h"
#include "log.h"

namespace Impacto {

static GLuint ShaderProgram = 0, UniformViewProjection = 0, UniformModel = 0,
              UniformAlpha = 0;
static GLuint UniformBones[ModelMaxBonesPerMesh] = {0};
static bool IsInit = false;

void Character3D::Init() {
  assert(IsInit == false);
  ImpLog(LL_Info, LC_Character3D, "Initializing Character3D system\n");
  IsInit = true;
  Model::Init();
  ShaderProgram = ShaderCompile("Character3D");
  UniformViewProjection = glGetUniformLocation(ShaderProgram, "ViewProjection");
  UniformModel = glGetUniformLocation(ShaderProgram, "Model");
  UniformAlpha = glGetUniformLocation(ShaderProgram, "Alpha");
  for (int i = 0; i < ModelMaxBonesPerMesh; i++) {
    char name[16];
    int sz = snprintf(name, 16, "Bones[%d]", i);
    assert(sz < sizeof(name));
    UniformBones[i] = glGetUniformLocation(ShaderProgram, name);
  }
}

Character3D* Character3D::Load(uint32_t modelId) {
  ImpLog(LL_Info, LC_Character3D, "Creating character (model ID %d)\n",
         modelId);

  Model* model = Model::Load(modelId);

  if (!model) {
    ImpLog(LL_Error, LC_Character3D,
           "Model loading failed for character with model ID %d\n");
    return NULL;
  }

  Character3D* result = new Character3D;
  result->StaticModel = model;

  result->CurrentPose = (PosedBone*)calloc(model->BoneCount, sizeof(PosedBone));

  result->ReloadDefaultBoneTransforms();

  return result;
}

void Character3D::ReloadDefaultBoneTransforms() {
  for (int i = 0; i < StaticModel->BoneCount; i++) {
    CurrentPose[i].LocalTransform = StaticModel->Bones[i].BaseTransform;
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

void Character3D::Update() { Pose(); }

void Character3D::Render() {
  glUseProgram(ShaderProgram);
  glUniformMatrix4fv(UniformViewProjection, 1, GL_FALSE,
                     glm::value_ptr(g_Camera.ViewProjection));

  for (int i = 0; i < StaticModel->MeshCount; i++) {
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

    if (StaticModel->Meshes[i].ColorMap >= 0) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, TexBuffers[StaticModel->Meshes[i].ColorMap]);
    }

    glUniform1f(UniformAlpha, StaticModel->Meshes[i].Opacity);

    // TODO: how do they actually do this?
    if (StaticModel->Meshes[i].Opacity < 0.9) {
      glDepthMask(GL_FALSE);
    }

    glDrawElements(GL_TRIANGLES, StaticModel->Meshes[i].IndexCount,
                   GL_UNSIGNED_SHORT, 0);

    glDepthMask(GL_TRUE);
  }
}

// TODO do we need to do anything to make sure this only happens on render
// thread?
Character3D::~Character3D() {
  if (StaticModel) {
    if (IsSubmitted) {
      glDeleteBuffers(StaticModel->MeshCount, IBOs);
      glDeleteBuffers(StaticModel->MeshCount, VBOs);
      glDeleteVertexArrays(StaticModel->MeshCount, VAOs);
    }
    delete StaticModel;
  }
  if (CurrentPose) free(CurrentPose);
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