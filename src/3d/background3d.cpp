#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "background3d.h"

#include "../log.h"
#include "scene.h"
#include "camera.h"
#include "../shader.h"

namespace Impacto {

static GLuint ShaderProgram = 0, UniformViewProjection = 0, UniformModel = 0,
              TextureDummy = 0;
static bool IsInit = false;

void Background3DInit() {
  assert(IsInit == false);
  ImpLog(LL_Info, LC_Object3D, "Initializing Background3D system\n");
  IsInit = true;
  Model::Init();

  ShaderProgram = ShaderCompile("Background3D");

  UniformViewProjection = glGetUniformLocation(ShaderProgram, "ViewProjection");
  UniformModel = glGetUniformLocation(ShaderProgram, "Model");

  glUseProgram(ShaderProgram);
  glUniform1i(glGetUniformLocation(ShaderProgram, "ColorMap"), TT_ColorMap);

  Texture texDummy;
  texDummy.Load1x1();
  TextureDummy = texDummy.Submit();
}

void Background3DUpdateGpu(Scene* scene, Camera* camera) {
  glUseProgram(ShaderProgram);
  glUniformMatrix4fv(UniformViewProjection, 1, GL_FALSE,
                     glm::value_ptr(camera->ViewProjection));
}

bool Background3D::Load(uint32_t modelId) {
  assert(IsUsed == false);

  ImpLog(LL_Info, LC_Object3D, "Creating background (model ID %d)\n", modelId);

  StaticModel = Model::Load(modelId);

  if (!StaticModel) {
    ImpLog(LL_Error, LC_Object3D,
           "Model loading failed for background with model ID %d\n");
    return false;
  }

  assert(StaticModel->Type == ModelType_Background);

  IsUsed = true;

  return true;
}
void Background3D::Unload() {
  if (StaticModel) {
    ImpLog(LL_Info, LC_Object3D, "Unloading background model %d\n",
           StaticModel->Id);
    if (IsSubmitted) {
      glDeleteBuffers(StaticModel->MeshCount, IBOs);
      glDeleteBuffers(StaticModel->MeshCount, VBOs);
      glDeleteVertexArrays(StaticModel->MeshCount, VAOs);
      glDeleteTextures(StaticModel->TextureCount, TexBuffers);
    }
    delete StaticModel;
    StaticModel = 0;
  }
  IsSubmitted = false;
  IsUsed = false;
}

void Background3D::MainThreadOnLoad() {
  assert(IsSubmitted == false);

  ImpLog(LL_Info, LC_Object3D,
         "Submitting data to GPU for background model ID %d\n",
         StaticModel->Id);

  glGenVertexArrays(StaticModel->MeshCount, VAOs);
  glGenBuffers(StaticModel->MeshCount, VBOs);
  glGenBuffers(StaticModel->MeshCount, IBOs);

  for (int i = 0; i < StaticModel->MeshCount; i++) {
    glBindVertexArray(VAOs[i]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
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
             "Submitting texture %d for background model %d failed\n", i,
             StaticModel->Id);
    }
  }

  IsSubmitted = true;
}

void Background3D::Render() {
  if (!IsUsed) return;

  glUseProgram(ShaderProgram);
  // TODO: *should* we cull backfaces? (in the default pose this covers some
  // sprites)
  glDisable(GL_CULL_FACE);
  glDepthMask(GL_TRUE);

  glActiveTexture(GL_TEXTURE0);

  for (int i = 0; i < StaticModel->MeshCount; i++) {
    Mesh* mesh = &StaticModel->Meshes[i];
    glBindVertexArray(VAOs[i]);

    glm::mat4 modelMatrix = mesh->ModelTransform.Matrix();
    glUniformMatrix4fv(UniformModel, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    if (StaticModel->Meshes[i].Maps[TT_ColorMap] >= 0) {
      glBindTexture(GL_TEXTURE_2D,
                    TexBuffers[StaticModel->Meshes[i].Maps[TT_ColorMap]]);
    } else {
      glBindTexture(GL_TEXTURE_2D, TextureDummy);
    }

    glDrawElements(GL_TRIANGLES, StaticModel->Meshes[i].IndexCount,
                   GL_UNSIGNED_SHORT, 0);
  }
}

}  // namespace Impacto