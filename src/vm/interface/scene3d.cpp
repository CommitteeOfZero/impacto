#include "../../impacto.h"
#include "../../scriptvars.h"
#include "../../3d/scene.h"
#include "../../mem.h"
#include "../../util.h"

namespace Impacto {
namespace Vm {
namespace Interface {

static void UpdateCharacterRot(int charId) {
  int pose = ScrWork[30 * charId + SW_CHA1POSE] - 30;

  if (pose >= 0) {
    float targetX = (int)ScrWork[20 * pose + 5500] / 1000.0f;
    float targetY = ((int)ScrWork[20 * pose + 5501] / 1000.0f) + 12.5f;
    float targetZ = (int)ScrWork[20 * pose + 5502] / 1000.0f;
    float objectX = (int)ScrWork[30 * charId + SW_CHA1POSX] / 1000.0f;
    float objectY = ((int)ScrWork[30 * charId + SW_CHA1POSY] / 1000.0f) +
                    ((int)ScrWork[30 * charId + 5111] / 1000.0f);
    float objectZ = (int)ScrWork[30 * charId + SW_CHA1POSZ] / 1000.0f;
    glm::vec3 lookat = LookAtEulerZYX(glm::vec3(objectX, objectY, objectZ),
                                      glm::vec3(targetX, targetY, targetZ));
    lookat.x = 0.0f;
    Scene3D::Characters[charId].ModelTransform.SetRotationFromEuler(lookat);
    ScrWork[30 * charId + SW_CHA1ROTY] =
        -((lookat.y / (2 * M_PI)) * 360) * 1000.0f;
  } else {
    Scene3D::Characters[charId].ModelTransform.SetRotationFromEuler(
        glm::vec3(0.0f, 0.0f, 0.0f));
  }
}

static void UpdateCharacterPos(int charId) {}

static void UpdateCharacters() {
  for (int i = 0; i <= 5; i++) {
    if (Scene3D::Characters[i].Status == LS_Loaded) {
      UpdateCharacterRot(i);
      UpdateCharacterPos(i);
      Scene3D::Characters[i].IsVisible = GetFlag(SF_CHA1DISP + i);
      Scene3D::Characters[i].ModelTransform.Position.x =
          (int)ScrWork[SW_CHA1POSX + i * 30] / 1000.0f;
      Scene3D::Characters[i].ModelTransform.Position.y =
          ((int)ScrWork[SW_CHA1POSY + i * 30]) / 1000.0f;
      Scene3D::Characters[i].ModelTransform.Position.z =
          (int)ScrWork[SW_CHA1POSZ + i * 30] / 1000.0f;
    }
  }
  if (Scene3D::Backgrounds[0].Status == LS_Loaded) {
    Scene3D::Backgrounds[0].IsVisible = GetFlag(SF_CHA1DISP);
  }
}

static void UpdateCamera() {
  // Update position
  // Scene3D::MainCamera.Move(glm::vec3(0.0f, 12.5f, 23.0f));

  // Update lookat
  Scene3D::MainCamera.LookAt(glm::vec3(0.0f, 12.5f, 0.0f));

  // Update fov
  float hFovRad = ((int)ScrWork[SW_IRUOCAMERAHFOV] / 1000.0f) * M_PI / 180.0f;
  Scene3D::MainCamera.Fov = 2 * atan(tan(hFovRad / 2.0f) * (9.0f / 16.0f));

  // Update lighting
  uint32_t lightColor = ScrWork[SW_MAINLIGHTTINT];
  glm::vec4 lightC = RgbaIntToFloat(lightColor);
  lightC.a = (int)ScrWork[SW_MAINLIGHTINT] / 1000.0f;
  float lightX = (int)ScrWork[SW_MAINLIGHTPOSX] / 1000.0f;
  float lightY = (int)ScrWork[SW_MAINLIGHTPOSY] / 1000.0f;
  float lightZ = (int)ScrWork[SW_MAINLIGHTPOSZ] / 1000.0f;
  Scene3D::Tint = lightC;
  Scene3D::LightPosition = glm::vec3(lightX, lightY, lightZ);
  Scene3D::DarkMode = (bool)ScrWork[SW_MAINLIGHTDARKMODE];
}

void UpdateScene3D() {
  UpdateCharacters();
  UpdateCamera();
}

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto