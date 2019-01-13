#include "../../impacto.h"
#include "../../scriptvars.h"
#include "../../3d/scene.h"
#include "../../mem.h"
#include "../../util.h"

#include "../../profile/scene3d.h"

namespace Impacto {
namespace Vm {
namespace Interface {

static void UpdateRenderableRot(int charId) {
  int pose = ScrWork[30 * charId + SW_CHA1POSE] - 30;

  if (pose >= 0) {
    glm::vec3 target =
        ScrWorkGetVec3(20 * pose + 5500, 20 * pose + 5501, 20 * pose + 5502);
    target.y += Profile::Scene3D::DefaultCameraPosition.y;

    glm::vec3 object =
        ScrWorkGetVec3(30 * charId + SW_CHA1POSX, 30 * charId + SW_CHA1POSY,
                       30 * charId + SW_CHA1POSZ);
    object.y += ScrWorkGetFloat(30 * charId + SW_CHA1YCENTER);

    glm::vec3 lookat = LookAtEulerZYX(object, target);
    lookat.x = 0.0f;

    Scene3D::Renderables[charId].ModelTransform.SetRotationFromEuler(lookat);

    ScrWorkSetAngle(30 * charId + SW_CHA1ROTY, lookat.y);
  } else {
    Scene3D::Renderables[charId].ModelTransform.Rotation = glm::quat();
  }
}

static void UpdateRenderablePos(int charId) {
  Scene3D::Renderables[charId].ModelTransform.Position =
      ScrWorkGetVec3(30 * charId + SW_CHA1POSX, 30 * charId + SW_CHA1POSY,
                     30 * charId + SW_CHA1POSZ);
}

static void UpdateRenderables() {
  for (int i = 0; i <= 8; i++) {
    if (Scene3D::Renderables[i].Status == LS_Loaded) {
      UpdateRenderableRot(i);
      UpdateRenderablePos(i);
      Scene3D::Renderables[i].IsVisible =
          GetFlag(SF_CHA1DISP + i) || GetFlag(2808 + i);
    }
  }
}

static void UpdateCamera() {
  glm::vec3 posCam;
  glm::vec3 lookatCam;
  float hFovRad;
  int camera = !GetFlag(SF_IRUOENABLE);
  posCam = ScrWorkGetAngleVec3(SW_IRUOCAMERAPOSX + 20 * camera,
                               SW_IRUOCAMERAPOSY + 20 * camera,
                               SW_IRUOCAMERAPOSZ + 20 * camera);
  lookatCam =
      ScrWorkGetAngleVec3(SW_IRUOCAMERAROTX + 20 * camera,
                          SW_IRUOCAMERAROTY + 20 * camera, 5405 + 20 * camera);
  lookatCam = -lookatCam;

  if (ScrWork[SW_IRUOCAMERAHFOVDELTA + 20 * camera] == 0) {
    ScrWork[SW_IRUOCAMERAHFOVCUR] = ScrWork[SW_IRUOCAMERAHFOV];
    ScrWork[SW_MAINCAMERAHFOVCUR] = ScrWork[SW_MAINCAMERAHFOV];
  } else if (ScrWork[SW_IRUOCAMERAHFOV + 20 * camera] >
             ScrWork[SW_IRUOCAMERAHFOVCUR + 10 * camera]) {
    ScrWork[SW_IRUOCAMERAHFOVCUR + 10 * camera] +=
        ScrWork[SW_IRUOCAMERAHFOVDELTA + 20 * camera];
    if (ScrWork[SW_IRUOCAMERAHFOV + 20 * camera] <
        ScrWork[SW_IRUOCAMERAHFOVCUR + 10 * camera])
      ScrWork[SW_IRUOCAMERAHFOVCUR + 10 * camera] =
          ScrWork[SW_IRUOCAMERAHFOV + 20 * camera];
  } else if (ScrWork[SW_IRUOCAMERAHFOV + 20 * camera] <
             ScrWork[SW_IRUOCAMERAHFOVCUR + 10 * camera]) {
    ScrWork[SW_IRUOCAMERAHFOVCUR + 10 * camera] -=
        ScrWork[SW_IRUOCAMERAHFOVDELTA + 20 * camera];
    if (ScrWork[SW_IRUOCAMERAHFOV + 20 * camera] >
        ScrWork[SW_IRUOCAMERAHFOVCUR + 10 * camera])
      ScrWork[SW_IRUOCAMERAHFOVCUR + 10 * camera] =
          ScrWork[SW_IRUOCAMERAHFOV + 20 * camera];
  }
  if (GetFlag(SF_IRUOENABLE) && !GetFlag(SF_IRUOAUTO))
    hFovRad = ScrWorkGetAngle(SW_POKECOMIRUOHFOV);
  else
    hFovRad = ScrWorkGetAngle(SW_IRUOCAMERAHFOVCUR + 10 * camera);

  // Update position
  // Scene3D::MainCamera.Move(glm::vec3(0.0f, 12.5f, 23.0f));
  // Update lookat
  Scene3D::MainCamera.CameraTransform.SetRotationFromEuler(lookatCam);
  // Update fov
  Scene3D::MainCamera.Fov =
      2.0f *
      atanf(tanf(hFovRad / 2.0f) * (1.0f / Scene3D::MainCamera.AspectRatio));

  ScrWork[5980] = ScrWork[SW_IRUOCAMERAROTX];
  ScrWork[5981] = ScrWork[SW_IRUOCAMERAROTY];
  ScrWork[5990] = ScrWork[SW_MAINCAMERAROTX];
  ScrWork[5991] = ScrWork[SW_MAINCAMERAROTY];

  // Update lighting
  Scene3D::Tint = ScrWorkGetColor(SW_MAINLIGHTCOLOR);
  Scene3D::Tint.a = ScrWorkGetFloat(SW_MAINLIGHTWEIGHT);
  Scene3D::LightPosition =
      ScrWorkGetVec3(SW_MAINLIGHTPOSX, SW_MAINLIGHTPOSY, SW_MAINLIGHTPOSZ);
  Scene3D::DarkMode = (bool)ScrWork[SW_MAINLIGHTDARKMODE];
}

void UpdateScene3D() {
  UpdateRenderables();
  UpdateCamera();
}

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto