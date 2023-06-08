#pragma once

#include <glad/glad.h>

#include "../../3d/renderable3d.h"
#include "../shader.h"

namespace Impacto {
namespace OpenGL {

enum RenderPass { RP_Outline = 0, RP_First = 1, RP_Second = 2, RP_Count };

class Renderable3D : public IRenderable3D {
 public:
  static void Init(GLWindow* window, ShaderCompiler* shaderCompiler);
  static void BeginFrame(IScene3D* scene, Camera* camera);

  void MakePlane() override;

  void Update(float dt) override;
  void Render() override;

  void ReloadDefaultBoneTransforms() override;
  void InitMeshAnimStatus() override;
  void ReloadDefaultMeshAnimStatus() override;

  void SwitchAnimation(int16_t animId, float transitionTime) override;

 protected:
  bool LoadSync(uint32_t modelId) override;
  void UnloadSync() override;
  void MainThreadOnLoad() override;

 private:
  void Pose();
  void PoseBone(int16_t id);

  void CalculateMorphedVertices(int id);

  void UseMaterial(MaterialType type);
  void UseMesh(int id);
  void LoadModelUniforms();
  void LoadMeshUniforms(int id);
  void SetTextures(int id, int const* textureUnits, int count);
  void DrawMesh(int id, RenderPass pass);

  GLuint UBOModel;

  GLuint VAOs[ModelMaxMeshesPerModel];
  GLuint VBOs[ModelMaxMeshesPerModel];
  GLuint MorphVBOs[ModelMaxMeshesPerModel];
  GLuint IBOs[ModelMaxMeshesPerModel];
  GLuint UBOs[ModelMaxMeshesPerModel];

  GLuint TexBuffers[ModelMaxTexturesPerModel];

  bool VAOsUpdated[ModelMaxMeshesPerModel];
  bool UniformsUpdated[ModelMaxMeshesPerModel];

  Transform PrevBoneTransforms[ModelMaxBonesPerModel];
  AnimatedMesh PrevMeshAnimStatus[ModelMaxMeshesPerModel];
  float PrevPoseWeight;
  float AnimationTransitionTime;
};

}  // namespace OpenGL
}  // namespace Impacto