#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "character3d.h"

#include "camera.h"
#include "shader.h"
#include "log.h"

namespace Impacto {

static GLuint ShaderProgram = 0, UniformViewProjection = 0, UniformModel = 0;
static bool IsInit = false;

void Character3D::Init() {
  assert(IsInit == false);
  ImpLog(LL_Info, LC_Character3D, "Initializing Character3D system\n");
  IsInit = true;
  Model::Init();
  ShaderProgram = ShaderCompile("Character3D");
  UniformViewProjection = glGetUniformLocation(ShaderProgram, "ViewProjection");
  UniformModel = glGetUniformLocation(ShaderProgram, "Model");
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

  return result;
}

void Character3D::Render() {
  glUseProgram(ShaderProgram);
  glUniformMatrix4fv(UniformViewProjection, 1, GL_FALSE,
                     glm::value_ptr(g_Camera.ViewProjection));
  for (int i = 0; i < StaticModel->MeshCount; i++) {
    glBindVertexArray(VAOs[i]);
    glDrawElements(GL_TRIANGLES, StaticModel->Meshes[i].IndexCount,
                   GL_UNSIGNED_SHORT, 0);
  }
}

Character3D::~Character3D() {
  if (StaticModel) {
    if (IsSubmitted) {
      glDeleteBuffers(StaticModel->MeshCount, IBOs);
      glDeleteBuffers(StaticModel->MeshCount, VBOs);
      glDeleteVertexArrays(StaticModel->MeshCount, VAOs);
    }
    delete StaticModel;
  }
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

  IsSubmitted = true;
}

}  // namespace Impacto