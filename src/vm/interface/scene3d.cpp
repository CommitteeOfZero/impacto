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
    glm::vec3 target =
        ScrWorkGetVec3(20 * pose + 5500, 20 * pose + 5501, 20 * pose + 5502);
    target.y += 12.5f;

    glm::vec3 object =
        ScrWorkGetVec3(30 * charId + SW_CHA1POSX, 30 * charId + SW_CHA1POSY,
                       30 * charId + SW_CHA1POSZ);
    object.y += ScrWorkGetFloat(30 * charId + SW_CHA1YCENTER);

    glm::vec3 lookat = LookAtEulerZYX(object, target);
    lookat.x = 0.0f;

    Scene3D::Characters[charId].ModelTransform.SetRotationFromEuler(lookat);

    ScrWorkSetAngle(30 * charId + SW_CHA1ROTY, lookat.y);
  } else {
    Scene3D::Characters[charId].ModelTransform.Rotation = glm::quat();
  }
}

static void UpdateCharacterPos(int charId) {
  Scene3D::Characters[charId].ModelTransform.Position =
      ScrWorkGetVec3(30 * charId + SW_CHA1POSX, 30 * charId + SW_CHA1POSY,
                     30 * charId + SW_CHA1POSZ);
}

static void UpdateCharacters() {
  for (int i = 0; i <= 5; i++) {
    if (Scene3D::Characters[i].Status == LS_Loaded) {
      UpdateCharacterRot(i);
      UpdateCharacterPos(i);
      Scene3D::Characters[i].IsVisible = GetFlag(SF_CHA1DISP + i);
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
  float hFovRad = ScrWorkGetAngle(SW_IRUOCAMERAHFOV);
  Scene3D::MainCamera.Fov =
      2.0f *
      atanf(tanf(hFovRad / 2.0f) * (1.0f / Scene3D::MainCamera.AspectRatio));

  // Update lighting
  Scene3D::Tint = ScrWorkGetColor(SW_MAINLIGHTCOLOR);
  Scene3D::Tint.a = ScrWorkGetFloat(SW_MAINLIGHTWEIGHT);
  Scene3D::LightPosition =
      ScrWorkGetVec3(SW_MAINLIGHTPOSX, SW_MAINLIGHTPOSY, SW_MAINLIGHTPOSZ);
  Scene3D::DarkMode = (bool)ScrWork[SW_MAINLIGHTDARKMODE];
}

void UpdateScene3D() {
  UpdateCharacters();
  UpdateCamera();
}

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto