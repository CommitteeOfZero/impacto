#pragma once

#include <glm/glm.hpp>

#include "character3d.h"
#include "background3d.h"
#include "camera.h"

namespace Impacto {

int const Scene3DMaxBackgrounds = 8;
int const Scene3DMaxCharacters = 8;

enum MSResolveMode {
  MS_None,
  // Use a framebuffer with multisample texture
  MS_MultisampleTexture,
  // Use a framebuffer with singlesample texture provided by
  // EXT_multisampled_render_to_texture
  MS_SinglesampleTextureExt,
  // Use a framebuffer with multisample renderbuffer and blit to framebuffer
  // with singlesample texture
  MS_BlitFromRenderbuffer
};

class Scene {
 public:
  ~Scene();

  void Init();
  void Update(float dt);
  void Render();

  Camera MainCamera;

  Background3D Backgrounds[Scene3DMaxBackgrounds];
  Character3D Characters[Scene3DMaxCharacters];

  glm::vec3 LightPosition;
  glm::vec4 Tint;
  bool DarkMode;

  uint32_t CharacterToLoadId;

 private:
  void SetupFramebufferState();
  void CleanFramebufferState();
  void DrawToScreen();

  static MSResolveMode CheckMSResolveMode();

  GLuint FBO = 0;
  GLuint RenderTextureColor = 0;
  GLuint RenderTextureDS = 0;

  // Only for MS_BlitFromRenderbuffer
  GLuint FBOMultisample = 0;
  GLuint RenderbufferColor = 0;
  GLuint RenderbufferDS = 0;

  GLuint VAOScreenFillingTriangle = 0;
  GLuint VBOScreenFillingTriangle = 0;

  GLuint ShaderProgram = 0;
};

void SceneInit();

}  // namespace Impacto