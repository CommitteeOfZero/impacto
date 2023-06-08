#include "scene3d.h"

#include "../../impacto.h"
#include "../../profile/scriptvars.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../util.h"

#include "../../profile/scene3d.h"
#include "../../profile/vm.h"

namespace Impacto {
namespace Vm {
namespace Interface {

using namespace Impacto::Profile::ScriptVars;

ska::flat_hash_map<int, ScrWorkAnimation> ScrWorkAnimations;
std::vector<int> CurrentScrWorkAnimations;

static void UpdateScrWorkAnimations() {
  for (int i = 0; i < CurrentScrWorkAnimations.size(); i++) {
    int id = CurrentScrWorkAnimations[i];
    ScrWorkAnimations[id].MainAnimation.Update(
        1 /
        60.0f);  // TODO: Nice hack you have here (get the proper dt in there)
    for (int i = 0; i < ScrWorkAnimations[id].AnimationData.size(); i++) {
      ScrWork[ScrWorkAnimations[id].AnimationData[i].Target] = glm::mix(
          ScrWorkAnimations[id].AnimationData[i].From,
          ScrWorkAnimations[id].AnimationData[i].To,
          std::pow(ScrWorkAnimations[id].MainAnimation.Progress - 1, 3) + 1);
    }
    if (ScrWorkAnimations[id].MainAnimation.IsIn()) {
      CurrentScrWorkAnimations.erase(CurrentScrWorkAnimations.begin() + i);
    }
  }
}

static void UpdateRenderableRot(int charId) {
  int pose = ScrWork[30 * charId + SW_MDL1TARDIR] - 30;

  switch (Profile::Vm::GameInstructionSet) {
    case InstructionSet::RNE: {
      if (pose >= 0) {
        glm::vec3 target = ScrWorkGetVec3(20 * pose + 5500, 20 * pose + 5501,
                                          20 * pose + 5502);
        target.y += Profile::Scene3D::DefaultCameraPosition.y;

        glm::vec3 object =
            ScrWorkGetVec3(30 * charId + SW_MDL1POSX, 30 * charId + SW_MDL1POSY,
                           30 * charId + SW_MDL1POSZ);
        object.y += ScrWorkGetFloat(30 * charId + SW_MDL1CENY);

        glm::vec3 lookat = LookAtEulerZYX(object, target);
        lookat.x = 0.0f;

        Renderer->Scene->Renderables[charId]
            ->ModelTransform.SetRotationFromEuler(lookat);

        ScrWorkSetAngle(30 * charId + SW_MDL1ROTY, lookat.y);
      } else {
        Renderer->Scene->Renderables[charId]
            ->ModelTransform.SetRotationFromEuler(ScrWorkGetAngleVec3(
                30 * charId + SW_MDL1ROTX, 30 * charId + SW_MDL1ROTY,
                30 * charId + SW_MDL1ROTZ));
      }
    } break;
    case InstructionSet::Dash: {
      Renderer->Scene->Renderables[charId]->ModelTransform.SetRotationFromEuler(
          ScrWorkGetAngleVec3(30 * charId + SW_MDL1ROTX,
                              30 * charId + SW_MDL1ROTY,
                              30 * charId + SW_MDL1ROTZ));
    } break;
  }
}

static void UpdateRenderablePos(int charId) {
  Renderer->Scene->Renderables[charId]->ModelTransform.Position =
      ScrWorkGetVec3(30 * charId + SW_MDL1POSX, 30 * charId + SW_MDL1POSY,
                     30 * charId + SW_MDL1POSZ);
}

static void UpdateRenderables() {
  for (int i = 0; i < Profile::Scene3D::MaxRenderables; i++) {
    if (Renderer->Scene->Renderables[i]->Status == LS_Loaded) {
      UpdateRenderableRot(i);
      UpdateRenderablePos(i);
      if (GetFlag(SF_IRUOENABLE) && GetFlag(SF_Pokecon_Open)) {
        Renderer->Scene->Renderables[i]->IsVisible = GetFlag(SF_MDL1SHDISP + i);
      } else {
        Renderer->Scene->Renderables[i]->IsVisible = GetFlag(SF_MDL1DISP + i);
      }
    }
  }
}

static void UpdateCamera() {
  glm::vec3 posCam;
  glm::vec3 lookatCam;
  float hFovRad;
  int camera = 0;
  if (Profile::Vm::GameInstructionSet == +InstructionSet::RNE)
    camera = !GetFlag(SF_IRUOENABLE);
  posCam = ScrWorkGetVec3(SW_IRUOCAMERAPOSX + 20 * camera,
                          SW_IRUOCAMERAPOSY + 20 * camera,
                          SW_IRUOCAMERAPOSZ + 20 * camera);
  lookatCam = ScrWorkGetAngleVec3(SW_IRUOCAMERAROTX + 20 * camera,
                                  SW_IRUOCAMERAROTY + 20 * camera,
                                  SW_CAMSHTARDIR + 20 * camera);

  switch (Profile::Vm::GameInstructionSet) {
    case InstructionSet::Dash: {
      posCam += ScrWorkGetVec3(2580, 2581, 2582);
      lookatCam += ScrWorkGetAngleVec3(2583, 2584, 2585);
      hFovRad = ScrWorkGetAngle(SW_IRUOCAMERAHFOV + 10 * camera);
      lookatCam.z = -lookatCam.z;
    } break;
    case InstructionSet::RNE: {
      posCam += Profile::Scene3D::DefaultCameraPosition;
      // Camera zoom
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

      // Copy some values
      ScrWork[SW_CAMSHROT_WORK] = ScrWork[SW_IRUOCAMERAROTX];
      ScrWork[SW_CAMSHELV_WORK] = ScrWork[SW_IRUOCAMERAROTY];
      ScrWork[SW_CAMSHPOSX_WORK] = ScrWork[SW_IRUOCAMERAPOSX];
      ScrWork[SW_CAMSHPOSY_WORK] = ScrWork[SW_IRUOCAMERAPOSY];
      ScrWork[SW_CAMSHPOSZ_WORK] = ScrWork[SW_IRUOCAMERAPOSZ];
      ScrWork[SW_CAMROT_WORK] = ScrWork[SW_MAINCAMERAROTX];
      ScrWork[SW_CAMELV_WORK] = ScrWork[SW_MAINCAMERAROTY];
      ScrWork[SW_CAMPOSX_WORK] = ScrWork[SW_MAINCAMERAPOSX];
      ScrWork[SW_CAMPOSY_WORK] = ScrWork[SW_MAINCAMERAPOSY];
      ScrWork[SW_CAMPOSZ_WORK] = ScrWork[SW_MAINCAMERAPOSZ];

      if (GetFlag(SF_IRUOENABLE) && !GetFlag(SF_IRUOAUTO))
        hFovRad = ScrWorkGetAngle(SW_POKECOMIRUOHFOV);
      else
        hFovRad = ScrWorkGetAngle(SW_IRUOCAMERAHFOVCUR + 10 * camera);
    }
  }

  lookatCam.x = -lookatCam.x;

  // Update position
  Renderer->Scene->MainCamera.CameraTransform.Position = posCam;
  // Update lookat
  Renderer->Scene->MainCamera.CameraTransform.SetRotationFromEuler(lookatCam);
  // Update fov
  Renderer->Scene->MainCamera.Fov =
      2.0f * atanf(tanf(hFovRad / 2.0f) *
                   (1.0f / Renderer->Scene->MainCamera.AspectRatio));

  // Update lighting
  switch (Profile::Vm::GameInstructionSet) {
    case InstructionSet::RNE: {
      Renderer->Scene->Tint = ScrWorkGetColor(SW_MAINLIGHTCOLOR);
      Renderer->Scene->Tint.a = ScrWorkGetFloat(SW_MAINLIGHTWEIGHT);
      Renderer->Scene->LightPosition =
          ScrWorkGetVec3(SW_MAINLIGHTPOSX, SW_MAINLIGHTPOSY, SW_MAINLIGHTPOSZ);
      Renderer->Scene->DarkMode = (bool)ScrWork[SW_MAINLIGHTDARKMODE];
    } break;
    case InstructionSet::Dash: {
      Renderer->Scene->Tint.r = ScrWork[SW_LIGHT1_COLORR] / 255.0f;
      Renderer->Scene->Tint.g = ScrWork[SW_LIGHT1_COLORG] / 255.0f;
      Renderer->Scene->Tint.b = ScrWork[SW_LIGHT1_COLORB] / 255.0f;
      Renderer->Scene->Tint.a = 1.0f;
      Renderer->Scene->LightPosition =
          ScrWorkGetVec3(SW_LIGHT1_POSX, SW_LIGHT1_POSY, SW_LIGHT1_POSZ);
    } break;
  }
}

void UpdateScene3D() {
  UpdateScrWorkAnimations();
  UpdateRenderables();
  UpdateCamera();
}

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto